#include <micro/utils/timer.hpp>

#include <cfg_board.hpp>
#include <system_init.h>

using namespace micro;

extern "C" void Error_Handler(void);

extern "C" void system_init(void) {
    time_init(tim_System);
}
