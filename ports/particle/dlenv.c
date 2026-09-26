/**
 * @file
 * @brief BACnet MS/TP datalink environment for Particle Device OS
 * @copyright SPDX-License-Identifier: MIT
 */
#include <string.h>

#include "bacnet/datalink/dlmstp.h"
#include "bacnet/datalink/mstp.h"
#include "dlenv.h"
#include "rs485.h"

static struct mstp_port_struct_t MSTP_Port;
static struct dlmstp_user_data_t MSTP_User_Data;
static uint8_t Input_Buffer[DLMSTP_MPDU_MAX];
static uint8_t Output_Buffer[DLMSTP_MPDU_MAX];

static struct dlmstp_rs485_driver RS485_Driver = {
    .init = rs485_init,
    .send = rs485_bytes_send,
    .read = rs485_byte_available,
    .transmitting = rs485_rts_enabled,
    .baud_rate = rs485_baud_rate,
    .baud_rate_set = rs485_baud_rate_set,
    .silence_milliseconds = rs485_silence_milliseconds,
    .silence_reset = rs485_silence_reset
};

bool particle_dlenv_init(
    uint8_t mac_address,
    uint8_t max_master,
    uint8_t max_info_frames,
    uint32_t baud_rate)
{
    memset(&MSTP_Port, 0, sizeof(MSTP_Port));
    memset(&MSTP_User_Data, 0, sizeof(MSTP_User_Data));

    RS485_Driver.init();
    MSTP_Port.Nmax_info_frames = max_info_frames;
    MSTP_Port.Nmax_master = max_master;
    MSTP_Port.InputBuffer = Input_Buffer;
    MSTP_Port.InputBufferSize = sizeof(Input_Buffer);
    MSTP_Port.OutputBuffer = Output_Buffer;
    MSTP_Port.OutputBufferSize = sizeof(Output_Buffer);
    MSTP_Port.ZeroConfigEnabled = false;
    MSTP_Port.SlaveNodeEnabled = false;
    MSTP_Port.CheckAutoBaud = false;
    MSTP_Zero_Config_UUID_Init(&MSTP_Port);

    MSTP_User_Data.RS485_Driver = &RS485_Driver;
    MSTP_Port.UserData = &MSTP_User_Data;
    if (!dlmstp_init((const char *)&MSTP_Port)) {
        return false;
    }

    dlmstp_set_mac_address(mac_address);
    dlmstp_set_max_master(max_master);
    dlmstp_set_max_info_frames(max_info_frames);
    dlmstp_set_baud_rate(baud_rate);

    return dlmstp_baud_rate() == baud_rate;
}

void dlenv_maintenance_timer(uint16_t seconds)
{
    (void)seconds;
}

void dlenv_cleanup(void)
{
    /* Particle applications retain the UART and static port for their life. */
}
