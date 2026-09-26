# BACnet MS/TP Port for Particle P2 and B5 SoM

This port runs the shared BACnet Stack MS/TP master state machine on Particle
Device OS. It supports the P2 and B5 SoM using the Wiring `Serial1` API.

## Hardware

Use a 3.3 V half-duplex RS-485 transceiver. Connect its driver-enable and
active-high receiver-disable inputs together.

| Particle signal | P2 | B5 SoM | RS-485 transceiver |
|---|---:|---:|---|
| `TX` / `Serial1` TX | D8 | TX (module pin 9) | DI |
| `RX` / `Serial1` RX | D9 | RX (module pin 10) | RO |
| Direction control | D2 (default) | D2 (default) | DE and /RE |
| 3V3 | 3V3 | 3V3 | VCC |
| Ground | GND | GND | GND |

The A/B pair requires normal MS/TP biasing and 120 ohm termination at each
physical end of the trunk. Confirm the A/B labeling against the transceiver
datasheet because vendor naming is not consistent.

Override `BACNET_MSTP_DE_PIN` in `hardware.h` when the carrier board uses
another direction-control pin. `Serial1` is intentionally used on both
targets; the B5 SoM's other UART is reserved for the cellular modem.

Supported baud rates are 9600, 19200, 38400, 57600, 76800, and 115200.
The default is 38400 baud.

## Add the port to a Particle project

From the BACnet Stack repository root:

```sh
make particle PARTICLE_PROJECT_DIR=/absolute/path/to/particle-project
```

This creates `lib/bacnet-stack/` in an extended-layout Particle project. It
vendors all stack headers and only the C implementation files listed in
`sources.txt`, keeping the Particle project self-contained for Workbench and
cloud builds.

## Application API

Include `bacnet.h`, configure result callbacks, and initialize the local
master:

```cpp
bacnet_set_read_property_callback(onReadProperty);
bacnet_set_request_failure_callback(onRequestFailure);
bacnet_init(12345, 2, 127, 1, 38400);
```

The MS/TP receive state machine must run at least every millisecond with no
more than 5 ms jitter. Run `bacnet_task()` continuously from a dedicated
Particle `Thread`; issue requests from that same thread because the stack is
not thread-safe.

Read an Analog Value or Multi-state Value present-value by destination MAC:

```cpp
bacnet_read_present_value(5, OBJECT_ANALOG_VALUE, 0);
bacnet_read_present_value(5, OBJECT_MULTI_STATE_VALUE, 0);
```

## Build

From the Particle project:

```sh
particle compile p2 . --target 6.4.1
particle compile b5som . --target 6.4.1
```

The default serial buffers are replaced with 512-byte RX and TX buffers so a
maximum-size MS/TP frame is not truncated. Device OS does not expose UART
error flags or byte-arrival timestamps; CRC validation detects corrupted
frames, while the dedicated task minimizes silence-timer polling jitter.
