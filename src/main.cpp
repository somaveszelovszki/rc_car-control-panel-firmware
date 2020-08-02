#include <micro/hw/VL53L1X_DistanceSensor.hpp>
#include <micro/math/unit_utils.hpp>
#include <micro/panel/DistSensorPanelData.hpp>
#include <micro/utils/timer.hpp>
#include <micro/utils/task.hpp>
#include <cfg_board.h>

using namespace micro;

namespace {

hw::VL53L1X_DistanceSensor sensor(i2c_Sensor, 0x52);
PanelLink<DistSensorPanelInData, DistSensorPanelOutData> panelLink(panelLinkRole_t::Slave, uart_PanelLink);

meter_t distance;

void parseDistSensorPanelData(const DistSensorPanelInData& rxData) {
    UNUSED(rxData);
}

void fillDistSensorPanelData(DistSensorPanelOutData& txData) {
    txData.distance_mm = static_cast<uint16_t>(static_cast<millimeter_t>(distance).get());
}

} // namespace

extern "C" void run(void) {
    sensor.initialize();

    os_delay(50);

    DistSensorPanelInData rxData;
    DistSensorPanelOutData txData;

    millisecond_t prevReadTime = getTime();
    bool isSensorOk = false;

    Timer sensorReadTimer(millisecond_t(10));
    Timer ledBlinkTimer(millisecond_t(250));

    while (true) {
        panelLink.update();

        if (sensorReadTimer.checkTimeout()) {
            if (isOk(sensor.readDistance(distance))) {
                isSensorOk = true;

                if (distance > centimeter_t(200)) {
                    distance = micro::numeric_limits<meter_t>::infinity();
                }

                prevReadTime = getTime();
            } else if (getTime() - prevReadTime > millisecond_t(100)) {
                distance = meter_t(0);
                isSensorOk = false;
                prevReadTime = getTime();
                // TODO
            }
        }

        if (panelLink.readAvailable(rxData)) {
            parseDistSensorPanelData(rxData);
        }

        if (panelLink.shouldSend()) {
            fillDistSensorPanelData(txData);
            panelLink.send(txData);
        }

        ledBlinkTimer.setPeriod(millisecond_t(isSensorOk && panelLink.isConnected() ? 500 : 250));
        if (ledBlinkTimer.checkTimeout()) {
            HAL_GPIO_TogglePin(gpio_Led, gpioPin_Led);
        }
    }
}

