/**
 * @file
 * @brief Particle P2 and B5 SoM hardware defaults for BACnet MS/TP
 * @copyright SPDX-License-Identifier: MIT
 */
#ifndef BACNET_PARTICLE_HARDWARE_H
#define BACNET_PARTICLE_HARDWARE_H

#include "Particle.h"

#if (PLATFORM_ID != PLATFORM_P2) && (PLATFORM_ID != PLATFORM_B5SOM)
#error "The BACnet Particle port supports only P2 and B5 SoM"
#endif

#ifndef BACNET_MSTP_SERIAL
#define BACNET_MSTP_SERIAL Serial1
#endif

/*
 * Connect DE and active-high /RE together to this pin. Override this macro
 * before including this header when the carrier board uses another pin.
 */
#ifndef BACNET_MSTP_DE_PIN
#define BACNET_MSTP_DE_PIN D2
#endif

#ifndef BACNET_MSTP_BAUD_RATE
#define BACNET_MSTP_BAUD_RATE 38400UL
#endif

#ifndef BACNET_MSTP_RX_BUFFER_SIZE
#define BACNET_MSTP_RX_BUFFER_SIZE 512U
#endif

#ifndef BACNET_MSTP_TX_BUFFER_SIZE
#define BACNET_MSTP_TX_BUFFER_SIZE 512U
#endif

#endif
