/**
 * @file
 * @brief Particle Device OS millisecond clock for BACnet timers
 * @copyright SPDX-License-Identifier: MIT
 */
#include "Particle.h"

#include "bacnet/basic/sys/mstimer.h"
#include "mstimer-init.h"

extern "C" {

void systimer_init(void)
{
}

void mstimer_init(void)
{
}

unsigned long mstimer_now(void)
{
    return millis();
}

}
