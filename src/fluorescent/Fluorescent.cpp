#include "Fluorescent.h"
#include "../kernel/Random.h"
#include "../kernel/io/UART.h"

int random(int min, int max)
{
    auto random = Kernel::Random::instance()->get();
    return random % (max - min);
}
