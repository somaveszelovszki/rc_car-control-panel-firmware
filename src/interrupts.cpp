#include <cfg_board.hpp>

// INTERRUPT CALLBACKS - Must be defined in a task's source file!

extern void tim_RcCtrlAccel_IC_CaptureCallback();
extern void tim_RcCtrlSteer_IC_CaptureCallback();

namespace {

uint32_t getChannel(const HAL_TIM_ActiveChannel chnl) {
    uint32_t result = 0xFFFFFFFFU;
    switch (chnl) {
    case HAL_TIM_ACTIVE_CHANNEL_1: result = TIM_CHANNEL_1; break;
    case HAL_TIM_ACTIVE_CHANNEL_2: result = TIM_CHANNEL_2; break;
    case HAL_TIM_ACTIVE_CHANNEL_3: result = TIM_CHANNEL_3; break;
    case HAL_TIM_ACTIVE_CHANNEL_4: result = TIM_CHANNEL_4; break;
    default: /* should not get here */                     break;
    }
    return result;
}

} // namespace

extern "C" void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim == tim_RcCtrl.handle) {
        const uint32_t chnl = getChannel(htim->Channel);
        switch (chnl) {
        case timChnl_RcCtrlAccel: tim_RcCtrlAccel_IC_CaptureCallback(); break;
        case timChnl_RcCtrlSteer: tim_RcCtrlSteer_IC_CaptureCallback(); break;
        default: /* should not get here */                              break;
        }
    }
}
