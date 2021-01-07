#include <micro/debug/DebugLed.hpp>
#include <micro/math/unit_utils.hpp>
#include <micro/utils/timer.hpp>
#include <micro/port/task.hpp>

#include <cfg_board.hpp>

using namespace micro;

namespace {

constexpr uint32_t INPUT_CHANNEL_ACCEL_OFFSET = 80;
constexpr uint32_t INPUT_CHANNEL_STEER_OFFSET = 35;
constexpr float INPUT_ZERO_DEADBAND           = 0.1f;

void onRcCtrlInputCapture(const uint32_t chnl, uint32_t& prevCntr, const uint32_t offset, float& measuredValue) {
    uint32_t cntr = 0;
    timer_getCaptured(tim_RcCtrl, chnl, cntr);

    uint32_t duty = (cntr >= prevCntr ? cntr - prevCntr : tim_RcCtrl.handle->Instance->ARR - prevCntr + cntr) - offset;
    if (duty > 850 && duty < 2150) {
        float input = map<uint32_t, float>(duty, 1000, 2000, -1.0f, 1.0f);
        if (abs(input) < INPUT_ZERO_DEADBAND) {
            input = 0.0f;
        }

        criticalSection_t criticalSection;
        criticalSection.lock();
        measuredValue = input;
        criticalSection.unlock();
    }
    prevCntr = cntr;
}

float acceleration = 0.0f;
float steering     = 0.0f;

} // namespace

extern "C" void run(void) {
    DebugLed debugLed(gpio_Led);

    while (true) {
        criticalSection_t criticalSection;
        criticalSection.lock();
        const float accel = acceleration;
        const float steer = steering;
        criticalSection.unlock();

        timer_setDuty(tim_Drive, timChnl_DriveAccel, accel);
        timer_setCompare(tim_Drive, timChnl_DriveSteer, map<float, uint32_t>(steer, -1.0f, 1.0f, 1000, 2000));
        debugLed.update(true);
    }
}

void tim_RcCtrlAccel_IC_CaptureCallback() {
    static uint32_t cntr = 0;
    onRcCtrlInputCapture(timChnl_RcCtrlAccel, cntr, INPUT_CHANNEL_ACCEL_OFFSET, acceleration);
}

void tim_RcCtrlSteer_IC_CaptureCallback() {
    static uint32_t cntr = 0;
    onRcCtrlInputCapture(timChnl_RcCtrlSteer, cntr, INPUT_CHANNEL_STEER_OFFSET, steering);
}
