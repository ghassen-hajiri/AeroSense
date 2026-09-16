# AeroSense – CAN Interface Design

**Project:** AeroSense – Embedded Condition Monitoring & Sensor Node  
**Document ID:** ARS-SDD-CAN-001  
**Revision:** A  
**Version:** 0.1  
**Status:** Draft  

---

## 1. Purpose

This document defines the application-level CAN interface of the AeroSense Rev A embedded condition monitoring node.

It specifies:

- CAN network parameters;
- message identifiers;
- message transmission behavior;
- payload layouts;
- signal data types;
- physical-value scaling;
- byte order;
- measurement validity information;
- diagnostic information.

The document defines the interface between AeroSense and external CAN nodes independently of the internal firmware implementation.

---

## 2. Applicable Documents

The CAN interface design is based on:

- `01_Requirements/Software_Requirements.md`
- `02_System_Architecture/System_Architecture.md`
- `02_System_Architecture/Interfaces/Interface_Definition.md`
- `04_Firmware/Detailed_Software_Design/Software_Architecture.md`
- `04_Firmware/Detailed_Software_Design/Driver_Design.md`
- `04_Firmware/Detailed_Software_Design/Diagnostics_Design.md`

Related detailed software design documents:

- `State_Machine.md`
- `Scheduling_Design.md`

---

# 3. CAN Network Configuration

The AeroSense Rev A CAN interface uses the following baseline configuration:

| Parameter | Value |
|---|---|
| CAN implementation | Classical CAN |
| Identifier format | 11-bit Standard Identifier |
| Nominal bit rate | 500 kbit/s |
| Maximum payload | 8 bytes |
| MCU peripheral | STM32 FDCAN1 |
| CAN transceiver | TCAN332 |
| MCU CAN RX | PB8 |
| MCU CAN TX | PB9 |
| Physical termination | 120 Ω selectable on AeroSense PCB |
| Byte order | Little-endian |
| Remote frames | Not used |

AeroSense shall use Classical CAN frames even though the STM32G431 FDCAN peripheral is capable of CAN FD operation.

CAN FD is outside the Rev A baseline.

---

# 4. CAN Application Architecture

Application-level CAN messages are handled by the CAN Message Manager.

The software flow is:

```text
Sensor Drivers
      |
      v
Measurement Manager
      |
      v
CAN Message Manager
      |
      v
CAN Driver
      |
      v
STM32 FDCAN1
      |
      v
TCAN332
      |
      v
CAN Bus
```

Diagnostic data follows:

```text
Diagnostic Manager
        |
        v
CAN Message Manager
        |
        v
CAN Driver
        |
        v
CAN Bus
```

The CAN Driver is responsible only for low-level frame transmission and reception.

Signal encoding and decoding belong to the CAN Message Manager.

---

# 5. Message Set

AeroSense Rev A defines the following transmitted CAN messages:

| CAN ID | Message Name | DLC | Transmission |
|---:|---|---:|---|
| `0x100` | `AEROSENSE_ENVIRONMENT` | 8 | Cyclic |
| `0x101` | `AEROSENSE_ACCELERATION` | 8 | Cyclic |
| `0x102` | `AEROSENSE_POWER` | 8 | Cyclic |
| `0x103` | `AEROSENSE_STATUS` | 8 | Cyclic |

The baseline transmission periods are:

| CAN ID | Period | Rate |
|---:|---:|---:|
| `0x100` | 100 ms | 10 Hz |
| `0x101` | 20 ms | 50 Hz |
| `0x102` | 100 ms | 10 Hz |
| `0x103` | 1000 ms | 1 Hz |

The final scheduler implementation shall preserve these externally visible message periods.

---

# 6. General Signal Encoding

Unless explicitly stated otherwise:

- multi-byte values use little-endian byte order;
- signed physical values use signed integer representation;
- unsigned physical values use unsigned integer representation;
- reserved bytes shall be transmitted as `0x00`;
- invalid measurement values shall be identified through validity information rather than through special numeric values.

Example little-endian encoding:

```text
Value = 0x1234

Byte N     = 0x34
Byte N + 1 = 0x12
```

---

# 7. Environmental Measurement Message

## 7.1 Message Definition

```text
Message Name: AEROSENSE_ENVIRONMENT
CAN ID:       0x100
DLC:          8
Period:       100 ms
```

The message contains temperature and atmospheric pressure.

---

## 7.2 Payload Layout

| Byte | Signal | Type | Scale | Unit |
|---:|---|---|---:|---|
| 0–1 | Temperature | `int16` | 0.01 | °C |
| 2–5 | Pressure | `uint32` | 0.01 | hPa |
| 6 | Validity | `uint8` | Bit field | — |
| 7 | Reserved | `uint8` | — | — |

---

## 7.3 Temperature Encoding

Temperature is encoded as:

```text
CAN_Temperature = Temperature_C / 0.01
```

Equivalent:

```text
CAN_Temperature = Temperature_C × 100
```

Example:

```text
Temperature = 23.45 °C

23.45 × 100 = 2345
2345 decimal = 0x0929
```

Little-endian transmission:

```text
Byte 0 = 0x29
Byte 1 = 0x09
```

Decoding:

```text
Temperature_C = CAN_Temperature × 0.01
```

The `int16` representation supports positive and negative temperatures.

---

# 8. Pressure Encoding

Pressure is encoded using an unsigned 32-bit integer.

```text
CAN_Pressure = Pressure_hPa / 0.01
```

Equivalent:

```text
CAN_Pressure = Pressure_hPa × 100
```

Example:

```text
Pressure = 1013.25 hPa

1013.25 × 100 = 101325
```

The receiver reconstructs:

```text
Pressure_hPa = CAN_Pressure × 0.01
```

Using a 32-bit representation avoids unnecessary restriction of the physical pressure range while maintaining 0.01 hPa resolution.

---

# 9. Environmental Validity Byte

Byte 6 contains measurement validity information.

| Bit | Meaning |
|---:|---|
| 0 | Temperature valid |
| 1 | Pressure valid |
| 2–7 | Reserved |

Encoding:

```text
Bit = 1 → measurement VALID
Bit = 0 → measurement INVALID
```

Example:

```text
Temperature valid = 1
Pressure valid    = 1

Validity byte = 00000011b
              = 0x03
```

If the temperature sensor fails while pressure remains valid:

```text
Validity byte = 00000010b
              = 0x02
```

---

# 10. Acceleration Message

## 10.1 Message Definition

```text
Message Name: AEROSENSE_ACCELERATION
CAN ID:       0x101
DLC:          8
Period:       20 ms
```

This message contains three-axis acceleration.

---

## 10.2 Payload Layout

| Byte | Signal | Type | Scale | Unit |
|---:|---|---|---:|---|
| 0–1 | Acceleration X | `int16` | 0.001 | g |
| 2–3 | Acceleration Y | `int16` | 0.001 | g |
| 4–5 | Acceleration Z | `int16` | 0.001 | g |
| 6 | Validity | `uint8` | Bit field | — |
| 7 | Reserved | `uint8` | — | — |

---

# 11. Acceleration Encoding

Each acceleration axis is encoded as:

```text
CAN_Acceleration = Acceleration_g / 0.001
```

Equivalent:

```text
CAN_Acceleration = Acceleration_g × 1000
```

Example:

```text
Acceleration Z = 0.981 g

0.981 × 1000 = 981
```

The receiver reconstructs:

```text
Acceleration_g = CAN_Acceleration × 0.001
```

The signed representation allows both positive and negative acceleration.

The representable range is approximately:

```text
-32.768 g to +32.767 g
```

which exceeds the expected configured operating range of the LIS2DW12.

---

# 12. Acceleration Validity

Byte 6 contains acceleration validity.

| Bit | Meaning |
|---:|---|
| 0 | X-axis valid |
| 1 | Y-axis valid |
| 2 | Z-axis valid |
| 3–7 | Reserved |

For a normal three-axis acquisition:

```text
Validity = 00000111b
         = 0x07
```

If communication with the LIS2DW12 fails, all three axes shall be considered invalid:

```text
Validity = 0x00
```

---

# 13. Power Message

## 13.1 Message Definition

```text
Message Name: AEROSENSE_POWER
CAN ID:       0x102
DLC:          8
Period:       100 ms
```

The message reports the monitored AeroSense input voltage.

---

## 13.2 Payload Layout

| Byte | Signal | Type | Scale | Unit |
|---:|---|---|---:|---|
| 0–1 | Input Voltage | `uint16` | 0.001 | V |
| 2 | VIN Validity | `uint8` | Boolean | — |
| 3–7 | Reserved | — | — | — |

---

# 14. Input Voltage Encoding

The input voltage is encoded as:

```text
CAN_VIN = VIN_V / 0.001
```

Equivalent:

```text
CAN_VIN = VIN_V × 1000
```

Example:

```text
VIN = 12.40 V

12.40 × 1000 = 12400
```

The receiver reconstructs:

```text
VIN_V = CAN_VIN × 0.001
```

The `uint16` representation supports values up to:

```text
65.535 V
```

which exceeds the AeroSense Rev A hardware input range.

---

# 15. VIN Validity

Byte 2 is defined as:

```text
0x00 → VIN measurement INVALID
0x01 → VIN measurement VALID
```

Other values are reserved.

The validity indicates whether a valid ADC-based voltage measurement is available.

An undervoltage condition does not necessarily make the measurement itself invalid.

Example:

```text
VIN = 7.50 V
Measurement successfully acquired

VIN validity = VALID

Diagnostic status may simultaneously contain:
VIN_UNDERVOLTAGE
```

This distinction allows the receiver to distinguish an invalid measurement from a valid measurement that indicates an abnormal system condition.

---

# 16. System Status Message

## 16.1 Message Definition

```text
Message Name: AEROSENSE_STATUS
CAN ID:       0x103
DLC:          8
Period:       1000 ms
```

This message provides the current AeroSense operational state and active diagnostic information.

---

## 16.2 Payload Layout

| Byte | Signal |
|---:|---|
| 0 | System State |
| 1 | Overall Diagnostic Severity |
| 2–5 | Active Fault Bitmask |
| 6 | Hardware Revision |
| 7 | Firmware Major Version |

---

# 17. System State Encoding

Byte 0 uses the following encoding:

| Value | State |
|---:|---|
| `0x00` | INITIALIZATION |
| `0x01` | SELF_TEST |
| `0x02` | NORMAL_OPERATION |
| `0x03` | DEGRADED_OPERATION |
| `0x04` | FAULT |
| `0x05–0xFF` | Reserved |

---

# 18. Diagnostic Severity Encoding

Byte 1 contains the highest currently active diagnostic severity.

| Value | Meaning |
|---:|---|
| `0x00` | No active fault / INFO only |
| `0x01` | INFO |
| `0x02` | DEGRADED |
| `0x03` | CRITICAL |
| `0x04–0xFF` | Reserved |

If several faults are active, the highest severity shall be transmitted.

---

# 19. Active Fault Bitmask

Bytes 2–5 contain a 32-bit active diagnostic bitmask.

The bitmask is transmitted little-endian.

| Bit | Diagnostic |
|---:|---|
| 0 | TEMP_SENSOR_COMM_FAULT |
| 1 | PRESS_SENSOR_COMM_FAULT |
| 2 | ACCEL_SENSOR_COMM_FAULT |
| 3 | VIN_ADC_FAULT |
| 4 | VIN_UNDERVOLTAGE |
| 5 | CAN_CONTROLLER_FAULT |
| 6 | USB_COMM_FAULT |
| 7 | INIT_FAILURE |
| 8 | EXECUTION_FAULT |
| 9 | WATCHDOG_RESET_DETECTED |
| 10–31 | Reserved |

Multiple bits may be set simultaneously.

Example:

```text
TEMP_SENSOR_COMM_FAULT  = active
ACCEL_SENSOR_COMM_FAULT = active

Fault mask:

bit 0 = 1
bit 2 = 1

Fault mask = 0x00000005
```

Payload:

```text
Byte 2 = 0x05
Byte 3 = 0x00
Byte 4 = 0x00
Byte 5 = 0x00
```

---

# 20. Hardware Revision Encoding

Byte 6 contains the hardware revision.

For AeroSense Rev A:

```text
0x01 = Hardware Revision A
```

Future revisions shall increment this numerical representation.

---

# 21. Firmware Version Encoding

Byte 7 contains the firmware major version.

For the initial firmware baseline:

```text
0x01 = Firmware major version 1
```

Minor and patch version information may be provided through the USB identification interface if required.

---

# 22. Message Timing

The nominal cyclic CAN schedule is:

```text
Every 20 ms:
    AEROSENSE_ACCELERATION

Every 100 ms:
    AEROSENSE_ENVIRONMENT
    AEROSENSE_POWER

Every 1000 ms:
    AEROSENSE_STATUS
```

The implementation shall avoid transmitting all lower-rate messages at exactly the same scheduler instant where practical.

The detailed phase distribution is defined in `Scheduling_Design.md`.

---

# 23. Relationship Between Acquisition and Transmission

Sensor acquisition and CAN transmission are logically separated.

Example:

```text
Sensor acquisition
       |
       v
Measurement Manager
       |
       v
Latest Valid Measurement
       |
       v
CAN transmission task
       |
       v
CAN frame
```

CAN message transmission therefore uses the latest available measurement representation maintained by the Measurement Manager.

The CAN Message Manager shall not directly initiate sensor communication.

---

# 24. Invalid Measurement Handling

If a measurement is invalid:

1. its validity bit shall be cleared;
2. the corresponding numeric field shall not be interpreted by the receiver as valid measurement data.

For deterministic payload generation, AeroSense Rev A shall transmit the affected numeric field as zero when the measurement is invalid.

Example:

```text
Temperature sensor unavailable

Temperature field = 0
Temperature valid = 0
```

The validity indication is authoritative.

A numeric value of zero with validity set to `VALID` remains a legitimate physical measurement.

---

# 25. Reserved Fields

All reserved payload bytes and bits shall be transmitted as zero by AeroSense Rev A.

Receivers shall not depend on reserved fields remaining zero in future protocol revisions.

This permits future extension without changing existing signal definitions.

---

# 26. CAN Reception

The primary Rev A CAN function is transmission of monitoring and diagnostic information.

The architecture nevertheless permits reception of CAN frames.

Unsupported received CAN identifiers shall be ignored without affecting normal sensor acquisition.

Any future command or configuration messages shall be explicitly defined before implementation.

Configuration of AeroSense through arbitrary undocumented CAN messages is not permitted.

---

# 27. CAN Error Handling

Low-level CAN controller errors are detected by the CAN Driver.

Relevant persistent errors are reported to the Diagnostic Manager as:

```text
CAN_CONTROLLER_FAULT
```

The application-level flow is:

```text
FDCAN Error
    |
    v
CAN Driver
    |
    v
Diagnostic Manager
    |
    v
CAN_CONTROLLER_FAULT
```

A temporary unsuccessful transmission shall not automatically cause a permanent fault.

Bus-off detection and recovery shall follow the diagnostic and driver design.

---

# 28. CAN Bus Termination

AeroSense Rev A contains a selectable 120 Ω CAN termination resistor.

The termination shall only be enabled when AeroSense is physically located at an end of the CAN bus.

Conceptually:

```text
Node A                         AeroSense
120 Ω                            120 Ω
 |                                |
 +-------- CAN_H / CAN_L ---------+
```

If AeroSense is connected as an intermediate node, its local termination shall remain disabled.

Termination is a hardware configuration and is not controlled by firmware.

---

# 29. Bus Load

The baseline message set produces a low bus load at 500 kbit/s.

The highest-rate AeroSense message is the acceleration message at 50 Hz.

Environmental and power messages are transmitted at 10 Hz and status information at 1 Hz.

The resulting load leaves substantial capacity for additional CAN nodes and future AeroSense messages.

Exact bus utilization may be verified during system integration.

---

# 30. Example Normal Operation

Assume:

```text
Temperature = 23.45 °C
Pressure    = 1013.25 hPa
Accel X     = 0.012 g
Accel Y     = -0.021 g
Accel Z     = 0.998 g
VIN         = 12.40 V

All measurements valid
System state = NORMAL_OPERATION
No active faults
```

AeroSense cyclically transmits:

```text
0x100 AEROSENSE_ENVIRONMENT
0x101 AEROSENSE_ACCELERATION
0x102 AEROSENSE_POWER
0x103 AEROSENSE_STATUS
```

with the corresponding scaled signal values and validity information.

---

# 31. Example Degraded Operation

Assume the STTS22H temperature sensor becomes unavailable.

The resulting behavior is:

```text
STTS22H communication failure
          |
          v
TEMP_SENSOR_COMM_FAULT
          |
          +--> Temperature INVALID
          |
          +--> Pressure remains VALID
          |
          +--> Acceleration remains VALID
          |
          +--> VIN remains VALID
          |
          v
DEGRADED_OPERATION
```

`AEROSENSE_ENVIRONMENT` continues to be transmitted.

The temperature field is transmitted as zero with the temperature-valid bit cleared.

The pressure value remains valid.

`AEROSENSE_STATUS` reports:

```text
System State:
DEGRADED_OPERATION

Fault Mask:
TEMP_SENSOR_COMM_FAULT
```

This permits the receiving system to continue using unaffected AeroSense measurements.

---

# 32. CAN Message Manager Interface

A proposed application-level interface is:

```c
void CANMSG_Init(void);

void CANMSG_TransmitEnvironment(void);

void CANMSG_TransmitAcceleration(void);

void CANMSG_TransmitPower(void);

void CANMSG_TransmitStatus(void);

void CANMSG_ProcessReceivedFrame(
    const AeroSense_CAN_Frame_t *frame);
```

The exact function organization may be adapted during implementation while preserving the externally defined CAN protocol.

---

# 33. Implementation Data Conversion

Physical values shall be converted to CAN raw values using explicit scaling.

Examples:

```c
temperature_raw =
    (int16_t)(temperature_c * 100.0f);

pressure_raw =
    (uint32_t)(pressure_hpa * 100.0f);

acceleration_raw =
    (int16_t)(acceleration_g * 1000.0f);

vin_raw =
    (uint16_t)(vin_v * 1000.0f);
```

Implementation shall handle numerical range limits before integer conversion.

CAN encoding shall not rely on direct memory casting of multi-byte C types because explicit byte packing provides deterministic endianness independent of compiler behavior.

---

# 34. Verification

The CAN interface shall be verified using a CAN analyzer or a second CAN-capable node.

Verification shall include:

| Verification | Expected Result |
|---|---|
| CAN bit rate | 500 kbit/s communication |
| Environmental message | ID `0x100`, DLC 8 |
| Acceleration message | ID `0x101`, DLC 8 |
| Power message | ID `0x102`, DLC 8 |
| Status message | ID `0x103`, DLC 8 |
| Temperature scaling | ±0.01 °C encoding resolution |
| Pressure scaling | 0.01 hPa encoding resolution |
| Acceleration scaling | 0.001 g encoding resolution |
| VIN scaling | 0.001 V encoding resolution |
| Byte order | Little-endian |
| Validity behavior | Invalid signals correctly indicated |
| Diagnostic mask | Active faults correctly represented |
| Message periods | Match defined cyclic rates |

Detailed test procedures shall be maintained under `06_Verification_Validation`.

---

# 35. Requirement Traceability

The CAN interface primarily supports the following software requirement groups:

| Requirement Group | CAN Interface Contribution |
|---|---|
| SW-ACQ-* | Transmission rates coordinated with acquisition rates |
| SW-PROC-* | Physical-value representation and validity |
| SW-CAN-* | CAN communication and message definitions |
| SW-DIAG-* | External diagnostic reporting |
| SW-CTRL-* | External operational-state reporting |
| SW-ID-* | Hardware and firmware identification |
| SW-ARCH-* | Separation of CAN encoding from hardware driver |
| SW-TEST-* | Defined externally verifiable CAN behavior |

Detailed bidirectional traceability shall be maintained in the project traceability matrix.

---

# 36. Rev A CAN Baseline

The AeroSense Rev A CAN protocol is therefore defined by four cyclic messages:

```text
0x100  Environmental Data     10 Hz
0x101  Acceleration Data      50 Hz
0x102  Power Data             10 Hz
0x103  System Status           1 Hz
```

All messages use:

```text
Classical CAN
11-bit identifiers
500 kbit/s
8-byte payload
Little-endian encoding
```

The protocol intentionally remains compact and deterministic to simplify firmware implementation, external decoding, integration, and verification.

---

# 37. Detailed Design Status

This document establishes the AeroSense Rev A CAN application interface.

Future protocol extensions shall preserve existing signal interpretation where practical and shall be documented through a controlled document revision.

The next detailed software design activity is the definition of the AeroSense operational state machine in `State_Machine.md`.

---

## 38. Revision History

| Revision | Version | Description | Status |
|---|---|---|---|
| A | 0.1 | Initial AeroSense Rev A CAN interface design | Draft |
