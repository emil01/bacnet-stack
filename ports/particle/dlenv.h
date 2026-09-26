/**
 * @file
 * @brief BACnet MS/TP datalink environment for Particle Device OS
 * @copyright SPDX-License-Identifier: MIT
 */
#ifndef BACNET_PARTICLE_DLENV_H
#define BACNET_PARTICLE_DLENV_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool particle_dlenv_init(
    uint8_t mac_address,
    uint8_t max_master,
    uint8_t max_info_frames,
    uint32_t baud_rate);
void dlenv_maintenance_timer(uint16_t seconds);
void dlenv_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif
