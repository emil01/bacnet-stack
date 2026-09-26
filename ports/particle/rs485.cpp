/**
 * @file
 * @brief Particle Device OS UART/RS-485 driver for BACnet MS/TP
 * @copyright SPDX-License-Identifier: MIT
 */
#include "hardware.h"
#include "rs485.h"

namespace {

bool Rs485_Transmitting;
uint32_t Rs485_Baud_Rate = BACNET_MSTP_BAUD_RATE;
uint32_t Rs485_Silence_Start;
uint32_t Rs485_Bytes_Tx;
uint32_t Rs485_Bytes_Rx;
uint8_t Rs485_Rx_Buffer[BACNET_MSTP_RX_BUFFER_SIZE];
uint8_t Rs485_Tx_Buffer[BACNET_MSTP_TX_BUFFER_SIZE];

void rs485_driver_guard_delay()
{
    /* Two bit periods cover transceiver enable/disable propagation time. */
    const uint32_t delay_us =
        (2000000UL + Rs485_Baud_Rate - 1UL) / Rs485_Baud_Rate;
    delayMicroseconds(delay_us);
}

} // namespace

hal_usart_buffer_config_t acquireSerial1Buffer()
{
    hal_usart_buffer_config_t config = {};
    config.size = sizeof(hal_usart_buffer_config_t);
    config.rx_buffer = Rs485_Rx_Buffer;
    config.rx_buffer_size = BACNET_MSTP_RX_BUFFER_SIZE;
    config.tx_buffer = Rs485_Tx_Buffer;
    config.tx_buffer_size = BACNET_MSTP_TX_BUFFER_SIZE;

    return config;
}

extern "C" {

void rs485_init(void)
{
    pinMode(BACNET_MSTP_DE_PIN, OUTPUT);
    rs485_rts_enable(false);
    BACNET_MSTP_SERIAL.begin(Rs485_Baud_Rate, SERIAL_8N1);
    while (BACNET_MSTP_SERIAL.available() > 0) {
        (void)BACNET_MSTP_SERIAL.read();
    }
    rs485_silence_reset();
}

void rs485_rts_enable(bool enable)
{
    digitalWrite(BACNET_MSTP_DE_PIN, enable ? HIGH : LOW);
    Rs485_Transmitting = enable;
}

bool rs485_rts_enabled(void)
{
    return Rs485_Transmitting;
}

bool rs485_byte_available(uint8_t *data_register)
{
    int value;

    if (BACNET_MSTP_SERIAL.available() <= 0) {
        return false;
    }
    if (!data_register) {
        return true;
    }

    value = BACNET_MSTP_SERIAL.read();
    if (value < 0) {
        return false;
    }
    *data_register = (uint8_t)value;
    Rs485_Bytes_Rx++;
    rs485_silence_reset();

    return true;
}

bool rs485_receive_error(void)
{
    /*
     * Wiring does not expose UART framing/parity/overrun status. The MS/TP
     * frame CRC still rejects damaged frames.
     */
    return false;
}

void rs485_bytes_send(const uint8_t *buffer, uint16_t nbytes)
{
    if (!buffer || !nbytes) {
        return;
    }

    rs485_rts_enable(true);
    rs485_driver_guard_delay();
    BACNET_MSTP_SERIAL.write(buffer, nbytes);
    BACNET_MSTP_SERIAL.flush();
    rs485_driver_guard_delay();
    rs485_rts_enable(false);
    Rs485_Bytes_Tx += nbytes;
    rs485_silence_reset();
}

uint32_t rs485_baud_rate(void)
{
    return Rs485_Baud_Rate;
}

bool rs485_baud_rate_set(uint32_t baud)
{
    switch (baud) {
        case 9600:
        case 19200:
        case 38400:
        case 57600:
        case 76800:
        case 115200:
            break;
        default:
            return false;
    }

    Rs485_Baud_Rate = baud;
    BACNET_MSTP_SERIAL.end();
    BACNET_MSTP_SERIAL.begin(Rs485_Baud_Rate, SERIAL_8N1);
    rs485_silence_reset();
    return true;
}

uint32_t rs485_silence_milliseconds(void)
{
    return millis() - Rs485_Silence_Start;
}

void rs485_silence_reset(void)
{
    Rs485_Silence_Start = millis();
}

uint32_t rs485_bytes_transmitted(void)
{
    return Rs485_Bytes_Tx;
}

uint32_t rs485_bytes_received(void)
{
    return Rs485_Bytes_Rx;
}

} // extern "C"
