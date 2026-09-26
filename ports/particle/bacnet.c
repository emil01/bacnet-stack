/**
 * @file
 * @brief BACnet client integration for Particle Device OS
 * @copyright SPDX-License-Identifier: MIT
 */
#include <stddef.h>
#include <string.h>

#include "bacnet/basic/binding/address.h"
#include "bacnet/basic/npdu/h_npdu.h"
#include "bacnet/basic/object/device.h"
#include "bacnet/basic/service/h_apdu.h"
#include "bacnet/basic/service/h_iam.h"
#include "bacnet/basic/service/h_noserv.h"
#include "bacnet/basic/service/h_rp.h"
#include "bacnet/basic/service/h_whois.h"
#include "bacnet/basic/service/s_iam.h"
#include "bacnet/basic/service/s_rp.h"
#include "bacnet/basic/service/s_wp.h"
#include "bacnet/basic/sys/mstimer.h"
#include "bacnet/basic/tsm/tsm.h"
#include "bacnet/datalink/datalink.h"
#include "bacnet/datalink/dlmstp.h"
#include "bacnet/dcc.h"
#include "bacnet/npdu.h"
#include "bacnet/rp.h"
#include "bacnet.h"
#include "dlenv.h"
#include "mstimer-init.h"

#define BACNET_DCC_CYCLE_MILLISECONDS 1000UL
#define BACNET_CLIENT_WRITE_PRIORITY 16

static uint8_t PDU_Buffer[MAX_MPDU + 16];
static struct mstimer DCC_Timer;
static unsigned long TSM_Last_Milliseconds;
static bacnet_read_property_callback Read_Property_Callback;
static bacnet_request_failure_callback Request_Failure_Callback;
static bacnet_write_property_ack_callback Write_Property_Ack_Callback;

static bool bacnet_supported_object_type(BACNET_OBJECT_TYPE object_type)
{
    switch (object_type) {
        case OBJECT_ANALOG_INPUT:
        case OBJECT_ANALOG_OUTPUT:
        case OBJECT_ANALOG_VALUE:
        case OBJECT_BINARY_VALUE:
        case OBJECT_MULTI_STATE_VALUE:
            return true;
        default:
            return false;
    }
}

static uint8_t bacnet_source_mac(const BACNET_ADDRESS *src)
{
    if (src && src->mac_len) {
        return src->mac[0];
    }
    return 0xFF;
}

static void bacnet_read_property_ack_handler(
    uint8_t *service_request,
    uint16_t service_len,
    BACNET_ADDRESS *src,
    BACNET_CONFIRMED_SERVICE_ACK_DATA *service_data)
{
    int len;
    BACNET_READ_PROPERTY_DATA property = { 0 };
    BACNET_APPLICATION_DATA_VALUE value = { 0 };

    len = rp_ack_decode_service_request(
        service_request, service_len, &property);
    if (len <= 0) {
        return;
    }
    len = bacapp_decode_application_data(
        property.application_data, property.application_data_len, &value);
    if ((len > 0) && Read_Property_Callback) {
        Read_Property_Callback(
            service_data->invoke_id, bacnet_source_mac(src), &property, &value);
    }
}

static void bacnet_write_property_simple_ack(
    BACNET_ADDRESS *src, uint8_t invoke_id)
{
    if (Write_Property_Ack_Callback) {
        Write_Property_Ack_Callback(invoke_id, bacnet_source_mac(src));
    }
}

static void bacnet_error_handler(
    BACNET_ADDRESS *src,
    uint8_t invoke_id,
    BACNET_ERROR_CLASS error_class,
    BACNET_ERROR_CODE error_code)
{
    if (Request_Failure_Callback) {
        Request_Failure_Callback(
            BACNET_REQUEST_ERROR, invoke_id, bacnet_source_mac(src),
            (uint16_t)error_class, (uint16_t)error_code);
    }
}

static void bacnet_abort_handler(
    BACNET_ADDRESS *src,
    uint8_t invoke_id,
    uint8_t abort_reason,
    bool server)
{
    (void)server;
    if (Request_Failure_Callback) {
        Request_Failure_Callback(
            BACNET_REQUEST_ABORT, invoke_id, bacnet_source_mac(src),
            abort_reason, 0);
    }
}

static void bacnet_reject_handler(
    BACNET_ADDRESS *src, uint8_t invoke_id, uint8_t reject_reason)
{
    if (Request_Failure_Callback) {
        Request_Failure_Callback(
            BACNET_REQUEST_REJECT, invoke_id, bacnet_source_mac(src),
            reject_reason, 0);
    }
}

static void bacnet_tsm_timeout_handler(uint8_t invoke_id)
{
    if (Request_Failure_Callback) {
        Request_Failure_Callback(
            BACNET_REQUEST_TIMEOUT, invoke_id, 0xFF, 0, 0);
    }
}

bool bacnet_init(
    uint32_t device_instance,
    uint8_t mstp_mac,
    uint8_t max_master,
    uint8_t max_info_frames,
    uint32_t baud_rate)
{
    if ((device_instance > BACNET_MAX_INSTANCE) ||
        (mstp_mac > DEFAULT_MAX_MASTER) || (max_master > DEFAULT_MAX_MASTER) ||
        (mstp_mac > max_master) || (max_info_frames == 0)) {
        return false;
    }

    systimer_init();
    mstimer_init();
    address_init();
    Device_Set_Object_Instance_Number(device_instance);
    Device_Init(NULL);
    if (!particle_dlenv_init(
            mstp_mac, max_master, max_info_frames, baud_rate)) {
        return false;
    }

    apdu_set_unrecognized_service_handler_handler(handler_unrecognized_service);
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_WHO_IS, handler_who_is);
    apdu_set_unconfirmed_handler(SERVICE_UNCONFIRMED_I_AM, handler_i_am_add);
    apdu_set_confirmed_handler(
        SERVICE_CONFIRMED_READ_PROPERTY, handler_read_property);
    apdu_set_confirmed_ack_handler(
        SERVICE_CONFIRMED_READ_PROPERTY, bacnet_read_property_ack_handler);
    apdu_set_confirmed_simple_ack_handler(
        SERVICE_CONFIRMED_WRITE_PROPERTY, bacnet_write_property_simple_ack);
    apdu_set_error_handler(
        SERVICE_CONFIRMED_READ_PROPERTY, bacnet_error_handler);
    apdu_set_error_handler(
        SERVICE_CONFIRMED_WRITE_PROPERTY, bacnet_error_handler);
    apdu_set_abort_handler(bacnet_abort_handler);
    apdu_set_reject_handler(bacnet_reject_handler);
    tsm_set_timeout_handler(bacnet_tsm_timeout_handler);

    mstimer_set(&DCC_Timer, BACNET_DCC_CYCLE_MILLISECONDS);
    TSM_Last_Milliseconds = mstimer_now();
    Send_I_Am(&Handler_Transmit_Buffer[0]);

    return true;
}

void bacnet_task(void)
{
    BACNET_ADDRESS src = { 0 };
    uint16_t pdu_len;
    unsigned long now;
    unsigned long elapsed;

    now = mstimer_now();
    elapsed = now - TSM_Last_Milliseconds;
    if (elapsed) {
        while (elapsed > UINT16_MAX) {
            tsm_timer_milliseconds(UINT16_MAX);
            elapsed -= UINT16_MAX;
        }
        tsm_timer_milliseconds((uint16_t)elapsed);
        TSM_Last_Milliseconds = now;
    }
    if (mstimer_expired(&DCC_Timer)) {
        mstimer_reset(&DCC_Timer);
        dcc_timer_seconds(BACNET_DCC_CYCLE_MILLISECONDS / 1000UL);
    }

    pdu_len =
        datalink_receive(&src, &PDU_Buffer[0], MAX_MPDU, 0);
    if (pdu_len) {
        npdu_handler(&src, &PDU_Buffer[0], pdu_len);
    }
}

void bacnet_cleanup(void)
{
    dlenv_cleanup();
}

void bacnet_set_read_property_callback(bacnet_read_property_callback callback)
{
    Read_Property_Callback = callback;
}

void bacnet_set_request_failure_callback(
    bacnet_request_failure_callback callback)
{
    Request_Failure_Callback = callback;
}

void bacnet_set_write_property_ack_callback(
    bacnet_write_property_ack_callback callback)
{
    Write_Property_Ack_Callback = callback;
}

uint8_t bacnet_read_present_value(
    uint8_t destination_mac,
    BACNET_OBJECT_TYPE object_type,
    uint32_t object_instance)
{
    BACNET_ADDRESS destination = { 0 };

    if ((destination_mac > 254) || !bacnet_supported_object_type(object_type)) {
        return 0;
    }
    dlmstp_fill_bacnet_address(&destination, destination_mac);
    return Send_Read_Property_Request_Address(
        &destination, MAX_APDU, object_type, object_instance,
        PROP_PRESENT_VALUE, BACNET_ARRAY_ALL);
}

uint8_t bacnet_write_present_value(
    uint8_t destination_mac,
    BACNET_OBJECT_TYPE object_type,
    uint32_t object_instance,
    uint32_t unsigned_value)
{
    BACNET_ADDRESS destination = { 0 };
    BACNET_APPLICATION_DATA_VALUE value = { 0 };

    if ((destination_mac > 254) ||
        (object_type != OBJECT_MULTI_STATE_VALUE)) {
        return 0;
    }
    value.tag = BACNET_APPLICATION_TAG_UNSIGNED_INT;
    value.type.Unsigned_Int = unsigned_value;
    dlmstp_fill_bacnet_address(&destination, destination_mac);
    return Send_Write_Property_Request_Address(
        &destination, MAX_APDU, object_type, object_instance,
        PROP_PRESENT_VALUE, &value, BACNET_CLIENT_WRITE_PRIORITY,
        BACNET_ARRAY_ALL);
}
