/**
 * @file
 * @brief Application-facing BACnet API for the Particle MS/TP port
 * @copyright SPDX-License-Identifier: MIT
 */
#ifndef BACNET_PARTICLE_BACNET_H
#define BACNET_PARTICLE_BACNET_H

#include <stdbool.h>
#include <stdint.h>

#include "bacnet/bacapp.h"
#include "bacnet/bacenum.h"
#include "bacnet/rp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*bacnet_read_property_callback)(
    uint8_t invoke_id,
    uint8_t source_mac,
    const BACNET_READ_PROPERTY_DATA *property,
    const BACNET_APPLICATION_DATA_VALUE *value);

typedef enum bacnet_request_failure {
    BACNET_REQUEST_ERROR,
    BACNET_REQUEST_ABORT,
    BACNET_REQUEST_REJECT,
    BACNET_REQUEST_TIMEOUT
} BACNET_REQUEST_FAILURE;

typedef void (*bacnet_request_failure_callback)(
    BACNET_REQUEST_FAILURE failure,
    uint8_t invoke_id,
    uint8_t source_mac,
    uint16_t error_class_or_reason,
    uint16_t error_code);

bool bacnet_init(
    uint32_t device_instance,
    uint8_t mstp_mac,
    uint8_t max_master,
    uint8_t max_info_frames,
    uint32_t baud_rate);
void bacnet_task(void);
void bacnet_cleanup(void);

typedef void (*bacnet_write_property_ack_callback)(
    uint8_t invoke_id, uint8_t source_mac);

void bacnet_set_read_property_callback(bacnet_read_property_callback callback);
void bacnet_set_request_failure_callback(
    bacnet_request_failure_callback callback);
void bacnet_set_write_property_ack_callback(
    bacnet_write_property_ack_callback callback);

uint8_t bacnet_read_present_value(
    uint8_t destination_mac,
    BACNET_OBJECT_TYPE object_type,
    uint32_t object_instance);
uint8_t bacnet_write_present_value(
    uint8_t destination_mac,
    BACNET_OBJECT_TYPE object_type,
    uint32_t object_instance,
    uint32_t unsigned_value);

#ifdef __cplusplus
}
#endif

#endif
