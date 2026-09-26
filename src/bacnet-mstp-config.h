/**
 * @file
 * @brief Application configuration for the Particle BACnet MS/TP VAV reader
 * @copyright SPDX-License-Identifier: MIT
 */
#ifndef BACNET_MSTP_CONFIG_H
#define BACNET_MSTP_CONFIG_H

#include <stddef.h>
#include <stdint.h>

#include "bacnet/bacenum.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BACNET_LOCAL_DEVICE_INSTANCE 1018u
#define BACNET_LOCAL_MSTP_MAC 16u
#define BACNET_MSTP_MAX_MASTER 16u
#define BACNET_MSTP_MAX_INFO_FRAMES 3u
#define BACNET_APP_MSTP_BAUD_RATE 76800u
#define BACNET_APDU_TIMEOUT_MS 1500u
#define BACNET_APDU_RETRIES 0u

#define BACNET_REMOTE_MSTP_MAC 4u
#define BACNET_REMOTE_DEVICE_INSTANCE 1004u

#define BACNET_READ_INTERVAL_MS 10000u
#define BACNET_BETWEEN_REQUESTS_MS 50u
#define BACNET_SETTLE_MS 750u
#define BACNET_READ_ATTEMPTS 2u
#define BACNET_RESPONSE_TIMEOUT_MS (2u * BACNET_APDU_TIMEOUT_MS)
#define BACNET_THREAD_STACK_SIZE 6144u
#define BACNET_PUBLISH_BUFFER_SIZE 8192u
#define BACNET_TEMP_BAND_C 0.5f
#define BACNET_UNITEN_DWELL_MS (15u * 60u * 1000u)
#define BACNET_WRITE_PRIORITY 16u

#define BACNET_UNITEN_SHUTDOWN 1u
#define BACNET_UNITEN_ENABLE 2u
#define BACNET_UNITEN_MODE_INSTANCE 77u
#define BACNET_UNITEN_STATE_INSTANCE 3393u

#define BACNET_PUBLISH_EVENT_NAME "bacnet"

#define BACNET_POINT_ZN_SP_NS "ZN-SP-NS"
#define BACNET_POINT_ZN_T "ZN-T"
#define BACNET_POINT_UNITEN_MODE "UNITEN-MODE"
#define BACNET_POINT_UNITEN_STATE "UNITEN-STATE"

typedef struct {
    const char *name;
    BACNET_OBJECT_TYPE object_type;
    uint32_t instance;
    const char *const *states;
    uint8_t state_count;
} bacnet_point_t;

static const char *const kOccModeStates[] = {
    "Occupied", "UnOccupied", "Bypass", "Standby"};
static const char *const kSadOutstateStates[] = {
    "Bypass", "Hold", "Control Flow Unreliable", "Control Flow Reliable"};
static const char *const kSaflowOutstateStates[] = {
    "Min", "Max", "Hold", "Control", "Heating", "Warmup", "Failsoft"};
static const char *const kEffOccStates[] = {
    "Occupied", "UnOccupied", "Bypass", "Standby"};
static const char *const kWcSStates[] = {
    "Normal", "Warmup", "Cooldown", "Coast"};
static const char *const kAutocalStateStates[] = {
    "Uncalibrated", "Waiting to Calibrate", "Waiting for Damper",
    "Autocalibrate", "Normal"};
static const char *const kZntStateStates[] = {
    "Satisfied", "Supp Htg", "Supp Htg + Box Htg", "Box Htg",
    "Box Htg + Supp Htg", "Prmy Clg", "Temperature Unreliable"};
static const char *const kBvFalseTrue[] = {"False", "True"};
static const char *const kUnitenStates[] = {"Shutdown", "Enable"};
static const char *const kUnoccStateStates[] = {
    "Satisfied", "Prmy Clg", "Supp Htg", "Box Htg", "Box Htg + Supp Htg",
    "Temperature Unreliable", "Unocc Ctrl Not Req'd"};
static const char *const kWcStateStates[] = {
    "Normal", "Warmup Satisfied", "Warmup Unreliable", "Cooldown",
    "Cooldown Unreliable", "Prmy Htg", "Prmy Htg + Supp Htg",
    "Prmy Htg + Supp Htg + Box Htg", "Prmy Htg + Box Htg",
    "Prmy Htg + Box Htg + Supp Htg", "Coast"};
static const char *const kOccScheduleStates[] = {
    "Occupied", "UnOccupied", "Standby", "Not Set"};
static const char *const kSystemModeStates[] = {
    "Cool Only", "Heat Only", "Fan Only", "Water Flush", "Purge", "Auto"};
static const char *const kWcCStates[] = {
    "Normal", "Warmup", "Cooldown", "Coast"};

#define BACNET_MV_STATES(list) (list), (uint8_t)(sizeof(list) / sizeof((list)[0]))

static const bacnet_point_t BACNET_POINTS[] = {
    {"DA-VP", OBJECT_ANALOG_INPUT, 1094, NULL, 0},
    {BACNET_POINT_ZN_SP_NS, OBJECT_ANALOG_VALUE, 1103, NULL, 0},
    {BACNET_POINT_ZN_T, OBJECT_ANALOG_INPUT, 1106, NULL, 0},
    {"DPR-O", OBJECT_ANALOG_OUTPUT, 2131, NULL, 0},
    {"OCC-MODE", OBJECT_MULTI_STATE_VALUE, 2086, BACNET_MV_STATES(kOccModeStates)},
    {"SAD-OUTSTATE", OBJECT_MULTI_STATE_VALUE, 3385,
        BACNET_MV_STATES(kSadOutstateStates)},
    {"SAFLOW-ABSEFFORT", OBJECT_ANALOG_VALUE, 4410, NULL, 0},
    {"SAFLOW-ABSERROR", OBJECT_ANALOG_VALUE, 4409, NULL, 0},
    {"SAFLOW-ERROR", OBJECT_ANALOG_VALUE, 4411, NULL, 0},
    {"SAFLOW-EWMA", OBJECT_ANALOG_VALUE, 4412, NULL, 0},
    {"SAFLOW-SP", OBJECT_ANALOG_VALUE, 3384, NULL, 0},
    {"SPMAXPOS", OBJECT_ANALOG_VALUE, 5715, NULL, 0},
    {"CLG-ABSEFFORT", OBJECT_ANALOG_VALUE, 4246, NULL, 0},
    {"CLG-ABSERROR", OBJECT_ANALOG_VALUE, 4245, NULL, 0},
    {"CLG-ERROR", OBJECT_ANALOG_VALUE, 4247, NULL, 0},
    {"CLG-EWMA", OBJECT_ANALOG_VALUE, 4248, NULL, 0},
    {"CLG-O", OBJECT_ANALOG_VALUE, 3615, NULL, 0},
    {"SAFLOW-OUTSTATE", OBJECT_MULTI_STATE_VALUE, 3387,
        BACNET_MV_STATES(kSaflowOutstateStates)},
    {"CLDWN-MINFLOW", OBJECT_ANALOG_VALUE, 3266, NULL, 0},
    {"CLG-MINFLOW", OBJECT_ANALOG_VALUE, 3267, NULL, 0},
    {"CLGUNOCC-MINFLOW", OBJECT_ANALOG_VALUE, 3272, NULL, 0},
    {"HTG-MINFLOW", OBJECT_ANALOG_VALUE, 3268, NULL, 0},
    {"HTGUNOCC-MINFLOW", OBJECT_ANALOG_VALUE, 3273, NULL, 0},
    {"MINFLOWCO2-PB", OBJECT_ANALOG_VALUE, 3270, NULL, 0},
    {"MINFLOWCO2-SP", OBJECT_ANALOG_VALUE, 3271, NULL, 0},
    {"OCC-LEVEL", OBJECT_ANALOG_VALUE, 3269, NULL, 0},
    {"WU-MINFLOW", OBJECT_ANALOG_VALUE, 3274, NULL, 0},
    {"EFF-OCC", OBJECT_MULTI_STATE_VALUE, 3290, BACNET_MV_STATES(kEffOccStates)},
    {"OCCMODE-BYPASSTIME", OBJECT_ANALOG_VALUE, 3289, NULL, 0},
    {"PRESS-REQ", OBJECT_ANALOG_VALUE, 5871, NULL, 0},
    {"SAD-ST", OBJECT_ANALOG_VALUE, 3386, NULL, 0},
    {"SA-F", OBJECT_ANALOG_VALUE, 3515, NULL, 0},
    {"CLG-REQ", OBJECT_ANALOG_VALUE, 5869, NULL, 0},
    {"HTG-REQ", OBJECT_ANALOG_VALUE, 5870, NULL, 0},
    {"WC-S", OBJECT_MULTI_STATE_VALUE, 3468, BACNET_MV_STATES(kWcSStates)},
    {"WCT-DIFF", OBJECT_ANALOG_VALUE, 3469, NULL, 0},
    {"CLGOCC-SP", OBJECT_ANALOG_VALUE, 3474, NULL, 0},
    {"CLGUNOCC-SP", OBJECT_ANALOG_VALUE, 3478, NULL, 0},
    {"CLGSTBY-SP", OBJECT_ANALOG_VALUE, 3476, NULL, 0},
    {"EFFCLG-SP", OBJECT_ANALOG_VALUE, 3472, NULL, 0},
    {"EFFHTG-SP", OBJECT_ANALOG_VALUE, 3473, NULL, 0},
    {"HTGOCC-SP", OBJECT_ANALOG_VALUE, 3475, NULL, 0},
    {"HTGSTBY-SP", OBJECT_ANALOG_VALUE, 3477, NULL, 0},
    {"HTGUNOCC-SP", OBJECT_ANALOG_VALUE, 3479, NULL, 0},
    {"AUTOCAL-STATE", OBJECT_MULTI_STATE_VALUE, 3091,
        BACNET_MV_STATES(kAutocalStateStates)},
    {"CLG-MAXFLOW", OBJECT_ANALOG_VALUE, 3108, NULL, 0},
    {"CLGOCC-MINFLOW", OBJECT_ANALOG_VALUE, 3109, NULL, 0},
    {"HTGOCC-MINFLOW", OBJECT_ANALOG_VALUE, 3110, NULL, 0},
    {"SA-AREA", OBJECT_ANALOG_VALUE, 3111, NULL, 0},
    {"SA-KFACTOR", OBJECT_ANALOG_VALUE, 3112, NULL, 0},
    {"ZNT-STATE", OBJECT_MULTI_STATE_VALUE, 3466,
        BACNET_MV_STATES(kZntStateStates)},
    {"PFRESTART-EN", OBJECT_BINARY_VALUE, 3391, BACNET_MV_STATES(kBvFalseTrue)},
    {"PFRESTART-TIME", OBJECT_ANALOG_VALUE, 3392, NULL, 0},
    {BACNET_POINT_UNITEN_STATE, OBJECT_MULTI_STATE_VALUE,
        BACNET_UNITEN_STATE_INSTANCE, BACNET_MV_STATES(kUnitenStates)},
    {"UNOCC-SATTIME", OBJECT_ANALOG_VALUE, 3450, NULL, 0},
    {"UNOCC-STATE", OBJECT_MULTI_STATE_VALUE, 3452,
        BACNET_MV_STATES(kUnoccStateStates)},
    {"WC-STATE", OBJECT_MULTI_STATE_VALUE, 3471,
        BACNET_MV_STATES(kWcStateStates)},
    {"AUTOCAL-C", OBJECT_BINARY_VALUE, 4, BACNET_MV_STATES(kBvFalseTrue)},
    {"OCC-SCHEDULE", OBJECT_MULTI_STATE_VALUE, 59,
        BACNET_MV_STATES(kOccScheduleStates)},
    {"SA-T", OBJECT_ANALOG_VALUE, 75, NULL, 0},
    {"SYSTEM-MODE", OBJECT_MULTI_STATE_VALUE, 76,
        BACNET_MV_STATES(kSystemModeStates)},
    {"TUNING-RESET", OBJECT_BINARY_VALUE, 769, BACNET_MV_STATES(kBvFalseTrue)},
    {BACNET_POINT_UNITEN_MODE, OBJECT_MULTI_STATE_VALUE,
        BACNET_UNITEN_MODE_INSTANCE, BACNET_MV_STATES(kUnitenStates)},
    {"WC-C", OBJECT_MULTI_STATE_VALUE, 80, BACNET_MV_STATES(kWcCStates)},
    {"CLG-REQ SRC", OBJECT_ANALOG_VALUE, 25270, NULL, 0},
    {"PRESS-REQ SRC", OBJECT_ANALOG_VALUE, 25269, NULL, 0},
};

#define BACNET_POINT_COUNT \
    ((unsigned)(sizeof(BACNET_POINTS) / sizeof(BACNET_POINTS[0])))

#ifdef __cplusplus
}
#endif

#endif
