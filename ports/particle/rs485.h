/**
 * @file
 * @brief RS-485 API for the Particle BACnet MS/TP port
 * @copyright SPDX-License-Identifier: MIT
 */
#ifndef BACNET_PARTICLE_RS485_H
#define BACNET_PARTICLE_RS485_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void rs485_init(void);
void rs485_rts_enable(bool enable);
bool rs485_rts_enabled(void);
bool rs485_byte_available(uint8_t *data_register);
bool rs485_receive_error(void);
void rs485_bytes_send(const uint8_t *buffer, uint16_t nbytes);
uint32_t rs485_baud_rate(void);
bool rs485_baud_rate_set(uint32_t baud);
uint32_t rs485_silence_milliseconds(void);
void rs485_silence_reset(void);
uint32_t rs485_bytes_transmitted(void);
uint32_t rs485_bytes_received(void);

#ifdef __cplusplus
}
#endif

#endif
