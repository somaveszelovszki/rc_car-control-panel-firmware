#include <micro/debug/DebugLed.hpp>
#include <micro/math/unit_utils.hpp>
#include <micro/utils/timer.hpp>
#include <micro/port/task.hpp>

#include <cfg_board.hpp>

using namespace micro;

namespace {

constexpr millisecond_t INPUT_TIMEOUT        = millisecond_t(100);
constexpr int32_t INPUT_CHANNEL_ACCEL_OFFSET = -65;
constexpr int32_t INPUT_CHANNEL_STEER_OFFSET = -10;
constexpr float INPUT_ZERO_DEADBAND          = 0.25f;
constexpr float MAX_ACCEL                    = 1.0f;
constexpr float MAX_STEER                    = 0.7f;
constexpr int32_t OUTPUT_STEER_OFFSET        = 200;

void onRcCtrlInputCapture(const uint32_t chnl, uint32_t& prevCntr, const int32_t offset, float& measuredValue) {
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

millisecond_t lastAccelTime;
millisecond_t lastSteerTime;

float acceleration = 0.0f;
float steering     = 0.0f;

} // namespace

extern "C" void run(void) {
    DebugLed debugLed(gpio_Led);

    while (true) {
        criticalSection_t criticalSection;
        criticalSection.lock();
        float accel = acceleration;
        float steer = steering;
        criticalSection.unlock();

        if (getTime() - lastAccelTime > INPUT_TIMEOUT) {
            accel = 0.0f;
        }

        if (getTime() - lastSteerTime > INPUT_TIMEOUT) {
            steer = 0.0f;
        }

        accel = clamp(accel, -MAX_ACCEL, MAX_ACCEL);
        steer = clamp(steer, -MAX_STEER, MAX_STEER);

        timer_setDuty(tim_Drive, timChnl_DriveAccel, abs(accel));
        gpio_write(gpio_SpeedDir, accel >= 0.0f ? gpioPinState_t::RESET : gpioPinState_t::SET);
        timer_setCompare(tim_Drive, timChnl_DriveSteer, map<float, uint32_t>(steer, -1.0f, 1.0f, 2000, 1000) - OUTPUT_STEER_OFFSET);
        debugLed.update(true);
    }
}

void tim_RcCtrlAccel_IC_CaptureCallback() {
    static uint32_t cntr = 0;
    onRcCtrlInputCapture(timChnl_RcCtrlAccel, cntr, INPUT_CHANNEL_ACCEL_OFFSET, acceleration);
    lastAccelTime = getTime();
}

void tim_RcCtrlSteer_IC_CaptureCallback() {
    static uint32_t cntr = 0;
    onRcCtrlInputCapture(timChnl_RcCtrlSteer, cntr, INPUT_CHANNEL_STEER_OFFSET, steering);
    lastSteerTime = getTime();
}
