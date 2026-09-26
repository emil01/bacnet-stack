/**
 * @file
 * @brief Compact BACnet Stack configuration for Particle P2 and B5 SoM
 * @copyright SPDX-License-Identifier: MIT
 */
#ifndef BACNET_PARTICLE_CONFIG_H
#define BACNET_PARTICLE_CONFIG_H

#define BACDL_MSTP 1
#define BACNET_BIG_ENDIAN 0
#define BACNET_PROTOCOL_REVISION 16
#define BACNET_STACK_DEPRECATED_DISABLE

#define MAX_APDU 480
#define MAX_TSM_TRANSACTIONS 4
#define MAX_ADDRESS_CACHE 8
#define DLMSTP_MAX_INFO_FRAMES 3

#define PRINT_ENABLED 0
#define CRC_USE_TABLE
#define BACAPP_MINIMAL

/* Enable the confirmed client services used by this port. */
#define BACNET_SVC_RP_A 1
#define BACNET_SVC_WP_A 1

#endif
