# AeroSense – Diagnostics Design

**Project:** AeroSense – Embedded Condition Monitoring & Sensor Node  
**Document ID:** ARS-SDD-DIAG-001  
**Revision:** A  
**Version:** 0.1  
**Status:** Draft  

---

## 1. Purpose

This document defines the diagnostic design of the AeroSense Rev A firmware.

The diagnostic system detects, records, evaluates, and reports failures affecting sensor acquisition, supply monitoring, communication, initialization, and firmware execution.

The diagnostic design supports the AeroSense operational states:

- `INITIALIZATION`
- `SELF_TEST`
- `NORMAL_OPERATION`
- `DEGRADED_OPERATION`
- `FAULT`

The Diagnostic Manager provides centralized fault information to the System Manager and communication functions.

---

## 2. Applicable Documents

The diagnostic design is based on:

- `01_Requirements/Software_Requirements.md`
- `02_System_Architecture/System_Architecture.md`
- `02_System_Architecture/Interfaces/Interface_Definition.md`
- `04_Firmware/Detailed_Software_Design/Software_Architecture.md`
- `04_Firmware/Detailed_Software_Design/Driver_Design.md`

Related detailed software design documents:

- `CAN_Interface.md`
- `State_Machine.md`
- `Scheduling_Design.md`

---

## 3. Diagnostic Architecture

Diagnostic processing is centralized in the Diagnostic Manager.

The intended fault propagation is:

```text
Physical Hardware / MCU Peripheral
               |
               v
          Device Driver
               |
               v
       Driver Status Result
               |
               v
      Diagnostic Manager
               |
       +-------+-------+
       |               |
       v               v
 System Manager   Communication
       |               |
       v               v
System State      Fault Reporting
```

Device drivers detect hardware-access failures but shall not directly determine the global AeroSense operational state.

---

## 4. Diagnostic Objectives

The diagnostic system shall:

1. detect failures that affect required AeroSense functions;
2. distinguish individual sensor failures from system-critical failures;
3. prevent failed measurements from being treated as valid;
4. allow unaffected functions to continue where technically possible;
5. provide diagnostic information to the System Manager;
6. provide diagnostic information for external reporting;
7. support startup self-test;
8. support runtime fault detection;
9. support fault recovery where appropriate;
10. provide deterministic and testable fault behavior.

---

# 5. Fault Classification

AeroSense Rev A uses three diagnostic severity classes.

| Severity | Meaning | Typical System Effect |
|---|---|---|
| `INFO` | Diagnostic information that does not prevent required operation | No state degradation required |
| `DEGRADED` | One or more functions are unavailable, but useful operation remains possible | `DEGRADED_OPERATION` |
| `CRITICAL` | Safe or meaningful continued system operation cannot be guaranteed | `FAULT` |

The Diagnostic Manager determines active diagnostic conditions.

The System Manager uses the active diagnostic severity to determine the required operational state.

---

# 6. Diagnostic Fault Identification

Each diagnostic condition shall have a unique fault identifier.

The Rev A baseline fault set is:

| Fault ID | Symbolic Name | Severity |
|---:|---|---|
| 0x0001 | `TEMP_SENSOR_COMM_FAULT` | DEGRADED |
| 0x0002 | `PRESS_SENSOR_COMM_FAULT` | DEGRADED |
| 0x0003 | `ACCEL_SENSOR_COMM_FAULT` | DEGRADED |
| 0x0004 | `VIN_ADC_FAULT` | DEGRADED |
| 0x0005 | `VIN_UNDERVOLTAGE` | CRITICAL |
| 0x0006 | `CAN_CONTROLLER_FAULT` | DEGRADED |
| 0x0007 | `USB_COMM_FAULT` | INFO |
| 0x0008 | `INIT_FAILURE` | Context dependent |
| 0x0009 | `EXECUTION_FAULT` | CRITICAL |
| 0x000A | `WATCHDOG_RESET_DETECTED` | INFO |

Additional fault identifiers may be added while preserving uniqueness.

---

# 7. Diagnostic Data Representation

The Diagnostic Manager shall maintain the current status of defined diagnostic conditions.

A conceptual fault representation is:

```c
typedef enum
{
    DIAG_SEVERITY_INFO = 0,
    DIAG_SEVERITY_DEGRADED,
    DIAG_SEVERITY_CRITICAL
} DiagnosticSeverity_t;

typedef struct
{
    uint16_t fault_id;
    DiagnosticSeverity_t severity;
    bool active;
} DiagnosticFault_t;
```

The exact implementation may use bit fields or another memory-efficient representation while preserving the externally visible diagnostic behavior.

---

# 8. Diagnostic Manager Interface

The proposed public interface is:

```c
void DIAG_Init(void);

void DIAG_Update(void);

void DIAG_SetFault(uint16_t fault_id);

void DIAG_ClearFault(uint16_t fault_id);

bool DIAG_IsFaultActive(uint16_t fault_id);

bool DIAG_HasDegradedFault(void);

bool DIAG_HasCriticalFault(void);
```

An interface for retrieving diagnostic information for CAN or USB reporting may additionally be provided.

Example:

```c
uint32_t DIAG_GetActiveFaultMask(void);
```

The final representation used for communication shall be coordinated with `CAN_Interface.md`.

---

# 9. Sensor Communication Diagnostics

## 9.1 Temperature Sensor

The fault:

```text
TEMP_SENSOR_COMM_FAULT
```

shall be activated when communication with the STTS22H cannot be completed successfully according to the defined communication-failure criteria.

Effects:

```text
Temperature measurement → INVALID
Fault severity          → DEGRADED
Other measurements      → continue
```

The system may enter `DEGRADED_OPERATION`.

---

## 9.2 Pressure Sensor

The fault:

```text
PRESS_SENSOR_COMM_FAULT
```

shall be activated when communication with the LPS22HH fails according to the defined failure criteria.

Effects:

```text
Pressure measurement → INVALID
Fault severity       → DEGRADED
Other measurements   → continue
```

---

## 9.3 Accelerometer

The fault:

```text
ACCEL_SENSOR_COMM_FAULT
```

shall be activated when communication with the LIS2DW12 fails according to the defined failure criteria.

Effects:

```text
Acceleration X/Y/Z → INVALID
Fault severity     → DEGRADED
Other measurements → continue
```

All three acceleration axes shall be marked invalid when valid acceleration data cannot be obtained from the device.

---

# 10. Measurement Validity

Each measurement shall have associated validity information.

Conceptually:

```c
typedef enum
{
    MEASUREMENT_INVALID = 0,
    MEASUREMENT_VALID
} MeasurementValidity_t;
```

A measurement shall not be reported as valid if the corresponding acquisition operation failed.

Example:

```text
STTS22H_ReadTemperature()
          |
          +---- OK ----> Temperature updated
          |             Validity = VALID
          |
          +---- ERROR -> Previous value not treated
                        as current valid measurement
                        Validity = INVALID
```

This prevents stale data from being interpreted as a successful current measurement.

---

# 11. VIN Monitoring Diagnostics

## 11.1 ADC Acquisition Failure

The fault:

```text
VIN_ADC_FAULT
```

shall indicate that the firmware cannot obtain a valid VIN monitoring measurement.

Possible causes include:

- ADC conversion failure;
- ADC timeout;
- invalid ADC acquisition.

The VIN measurement shall be marked invalid while the fault is active.

Severity:

```text
DEGRADED
```

---

## 11.2 Supply Undervoltage

The fault:

```text
VIN_UNDERVOLTAGE
```

shall indicate that the monitored external supply voltage is below the defined minimum operating threshold.

The threshold shall be consistent with the electrical operating limits defined for AeroSense.

The diagnostic shall use filtering or persistence logic to avoid unnecessary state transitions caused by a single noisy ADC sample.

Conceptually:

```text
VIN measurement
      |
      v
Compare with threshold
      |
      +---- acceptable ----> no undervoltage fault
      |
      +---- too low -------> persistence evaluation
                                  |
                                  v
                         VIN_UNDERVOLTAGE
```

`VIN_UNDERVOLTAGE` is classified as `CRITICAL` because insufficient supply voltage may affect multiple hardware functions simultaneously.

The final threshold and persistence time shall be coordinated with the hardware operating range and `Scheduling_Design.md`.

---

# 12. CAN Diagnostics

The CAN Driver shall report controller or transmission conditions that prevent required CAN communication.

The Diagnostic Manager shall map relevant failures to:

```text
CAN_CONTROLLER_FAULT
```

Possible diagnostic sources include:

- FDCAN initialization failure;
- inability to start the CAN controller;
- persistent controller error;
- bus-off condition.

A temporary failed transmission does not necessarily require immediate activation of a persistent system fault.

The detailed CAN error-handling strategy shall be coordinated with the final CAN implementation.

Severity:

```text
DEGRADED
```

A CAN communication failure does not automatically invalidate locally acquired sensor measurements.

---

# 13. USB Diagnostics

The USB interface is not required for continuous sensor acquisition.

A USB communication failure therefore shall not normally prevent the primary AeroSense monitoring functions from operating.

The diagnostic condition:

```text
USB_COMM_FAULT
```

is classified as:

```text
INFO
```

The system may continue `NORMAL_OPERATION` when the USB interface is unavailable, provided all required monitoring and CAN functions remain operational.

---

# 14. Initialization Diagnostics

During startup, each required hardware and software component shall report its initialization result.

Conceptually:

```text
System Initialization
       |
       +--> Temperature sensor
       |
       +--> Pressure sensor
       |
       +--> Accelerometer
       |
       +--> VIN monitor
       |
       +--> CAN
       |
       +--> USB
       |
       +--> Watchdog / supervision
```

Individual initialization failures shall be mapped to their corresponding diagnostic condition where possible.

For example:

```text
STTS22H initialization failure
        ↓
TEMP_SENSOR_COMM_FAULT
        ↓
DEGRADED_OPERATION
```

A generic `INIT_FAILURE` may be used when initialization fails in a manner that cannot be represented by a more specific fault identifier.

The resulting system state depends on the severity of the failed function.

---

# 15. Startup Self-Test

The `SELF_TEST` state shall verify that the required hardware interfaces and devices are operational before normal cyclic operation begins.

The Rev A startup self-test shall include, where applicable:

| Function | Self-Test |
|---|---|
| STTS22H | Communication and device identification |
| LPS22HH | Communication and device identification |
| LIS2DW12 | Communication and device identification |
| VIN monitor | Valid ADC acquisition |
| CAN | Controller initialization/start status |
| USB | Initialization status |
| Internal software | Initialization status |

Failure of one non-critical sensor shall not automatically prevent operation of unaffected monitoring functions.

---

# 16. Runtime Diagnostics

Diagnostics shall continue during normal cyclic operation.

The runtime concept is:

```text
Acquire Function
      |
      v
Check Result
      |
      +---- SUCCESS ----> update valid data
      |
      +---- FAILURE ----> update diagnostic evidence
                               |
                               v
                         Diagnostic Manager
```

Runtime diagnostics shall detect faults independently of startup self-test.

A device that passed startup self-test may therefore still become faulty during operation.

---

# 17. Fault Confirmation

Where appropriate, AeroSense shall avoid activating persistent faults from a single transient event.

The diagnostic system may therefore use confirmation counters.

Conceptually:

```c
failure_counter++;

if (failure_counter >= FAILURE_CONFIRMATION_LIMIT)
{
    DIAG_SetFault(FAULT_ID);
}
```

Successful operations may reset or reduce the corresponding failure counter depending on the diagnostic.

This mechanism is particularly appropriate for:

- sensor communication errors;
- CAN communication errors;
- supply-voltage threshold monitoring.

Critical software-execution failures may require immediate action instead.

The exact confirmation counts shall be selected together with the scheduling periods so that the resulting detection time is known.

---

# 18. Fault Recovery

Recoverable diagnostic conditions shall support automatic recovery where technically appropriate.

Example:

```text
Sensor communication failure
          |
          v
Fault confirmed
          |
          v
DEGRADED_OPERATION
          |
          v
Periodic communication retry
          |
          +---- failure ----> remain DEGRADED
          |
          +---- success ----> recovery confirmation
                                  |
                                  v
                              clear fault
                                  |
                                  v
                          NORMAL_OPERATION
```

Fault clearing shall require evidence that the underlying failure is no longer present.

A fault shall not be cleared merely because a fixed amount of time has passed.

---

# 19. Recovery Confirmation

To avoid rapid fault activation and clearing, recoverable faults may require multiple successful operations before being cleared.

Conceptually:

```c
if (operation_successful)
{
    recovery_counter++;
}
else
{
    recovery_counter = 0;
}

if (recovery_counter >= RECOVERY_CONFIRMATION_LIMIT)
{
    DIAG_ClearFault(FAULT_ID);
}
```

The final confirmation limits shall be coordinated with the scheduling design.

---

# 20. Execution Supervision

The firmware shall detect loss of correct cyclic execution using execution supervision and the MCU hardware watchdog.

The fault:

```text
EXECUTION_FAULT
```

represents detection of a failure that prevents required cyclic execution from being considered valid.

Examples may include:

- required cyclic functions not executed;
- scheduler supervision failure;
- internal execution consistency failure.

Severity:

```text
CRITICAL
```

The watchdog shall only be refreshed after required execution-supervision conditions have been satisfied.

---

# 21. Watchdog Reset Detection

Following MCU reset, the firmware should inspect the available MCU reset-cause information.

If the previous reset was caused by the watchdog, the diagnostic event:

```text
WATCHDOG_RESET_DETECTED
```

shall be recorded where possible.

This information supports debugging and system-health reporting.

A previous watchdog reset does not by itself prove that the current execution cycle is faulty.

---

# 22. Diagnostic Influence on System State

The Diagnostic Manager does not directly perform state transitions.

Instead, it provides diagnostic status to the System Manager.

The intended relationship is:

```text
No DEGRADED or CRITICAL faults
              |
              v
      NORMAL_OPERATION


One or more DEGRADED faults
No CRITICAL faults
              |
              v
     DEGRADED_OPERATION


One or more CRITICAL faults
              |
              v
             FAULT
```

Detailed state-transition logic is defined in `State_Machine.md`.

---

# 23. Failure Containment

The diagnostic architecture shall prevent an isolated sensor fault from unnecessarily disabling unrelated monitoring functions.

Example:

```text
LIS2DW12 failure
      |
      v
ACCEL_SENSOR_COMM_FAULT
      |
      +--> Acceleration INVALID
      |
      +--> DEGRADED_OPERATION
      |
      +--> Temperature continues
      |
      +--> Pressure continues
      |
      +--> VIN monitoring continues
      |
      +--> CAN reporting continues
```

This behavior supports graceful degradation of the AeroSense node.

---

# 24. Diagnostic Reporting

Active diagnostic information shall be available to communication functions.

Diagnostic information may be transmitted through:

```text
CAN
USB
```

The Diagnostic Manager provides the diagnostic state.

The CAN Message Manager and Configuration Manager are responsible for converting that information into the corresponding communication protocol.

The Diagnostic Manager shall therefore not directly construct CAN frames or USB packets.

---

# 25. Diagnostic Bitmask

For compact communication and internal processing, the Rev A implementation may represent active faults using a bitmask.

Example:

```text
Bit 0  TEMP_SENSOR_COMM_FAULT
Bit 1  PRESS_SENSOR_COMM_FAULT
Bit 2  ACCEL_SENSOR_COMM_FAULT
Bit 3  VIN_ADC_FAULT
Bit 4  VIN_UNDERVOLTAGE
Bit 5  CAN_CONTROLLER_FAULT
Bit 6  USB_COMM_FAULT
Bit 7  INIT_FAILURE
Bit 8  EXECUTION_FAULT
Bit 9  WATCHDOG_RESET_DETECTED
```

This permits multiple faults to be active simultaneously.

Example:

```text
Bit 0 = 1
Bit 2 = 1

→ Temperature sensor fault
→ Accelerometer fault

Both faults are active simultaneously.
```

The final CAN representation of this information is defined in `CAN_Interface.md`.

---

# 26. Diagnostic Processing Sequence

A typical cyclic diagnostic sequence is:

```text
Sensor / Peripheral Operation
             |
             v
        Driver Result
             |
             v
 Update Failure / Recovery Evidence
             |
             v
      Diagnostic Manager
             |
             v
     Update Active Faults
             |
             +----------------+
             |                |
             v                v
       System Manager    Communication
             |
             v
      Operational State
```

Diagnostic processing shall remain bounded and shall not intentionally block cyclic execution.

---

# 27. Proposed Implementation Structure

The diagnostic implementation is expected to use dedicated source files:

```text
AeroSense/
└── Services/
    ├── diagnostic_manager.c
    └── diagnostic_manager.h
```

The public header shall expose diagnostic information required by the System Manager and communication services.

Device-specific diagnostic detection remains associated with the corresponding driver or service, while centralized storage and evaluation belongs to the Diagnostic Manager.

---

# 28. Diagnostic Verification

Each diagnostic shall be independently testable where practical.

Examples include:

| Diagnostic | Verification Method |
|---|---|
| Temperature communication fault | Disconnect or simulate unavailable STTS22H |
| Pressure communication fault | Disconnect or simulate unavailable LPS22HH |
| Accelerometer communication fault | Disconnect or simulate unavailable LIS2DW12 |
| VIN ADC fault | Inject/simulate ADC acquisition failure |
| VIN undervoltage | Apply controlled input voltage below threshold |
| CAN fault | Disconnect bus / induce controller error where appropriate |
| USB fault | Remove or prevent USB communication |
| Execution fault | Intentionally suppress required execution supervision |
| Watchdog reset | Intentionally stop watchdog refresh |

Verification shall confirm both:

```text
Fault detection
AND
Required system response
```

Detailed test procedures and acceptance criteria shall be defined under `06_Verification_Validation`.

---

# 29. Requirement Traceability

The diagnostic design primarily supports:

| Requirement Group | Diagnostic Design Contribution |
|---|---|
| SW-INIT-* | Startup initialization monitoring |
| SW-ACQ-* | Detection of acquisition failures |
| SW-PROC-* | Measurement validity management |
| SW-CAN-* | CAN communication status |
| SW-CFG-* | USB/configuration status |
| SW-DIAG-* | Centralized diagnostic handling |
| SW-CTRL-* | Diagnostic input to operational state |
| SW-WDG-* | Execution supervision and watchdog handling |
| SW-TEST-* | Fault injection and diagnostic verification |

Detailed bidirectional traceability shall be maintained in the project traceability matrix.

---

# 30. Rev A Diagnostic Baseline

The following diagnostic behavior is established for AeroSense Rev A:

| Condition | Measurement Effect | Severity | System Effect |
|---|---|---|---|
| Temperature sensor unavailable | Temperature invalid | DEGRADED | Degraded operation |
| Pressure sensor unavailable | Pressure invalid | DEGRADED | Degraded operation |
| Accelerometer unavailable | XYZ acceleration invalid | DEGRADED | Degraded operation |
| VIN ADC unavailable | VIN invalid | DEGRADED | Degraded operation |
| VIN below operating threshold | VIN low | CRITICAL | Fault |
| CAN controller unavailable | CAN unavailable | DEGRADED | Degraded operation |
| USB unavailable | USB unavailable | INFO | Monitoring continues |
| Execution supervision failure | Execution unreliable | CRITICAL | Fault |

Multiple faults may be active simultaneously.

The highest active diagnostic severity determines the diagnostic input provided to the System Manager.

---

# 31. Detailed Design Status

This document defines the baseline diagnostic architecture for AeroSense Rev A.

The following parameters are coordinated with subsequent detailed design and implementation:

- exact diagnostic confirmation counts;
- exact recovery confirmation counts;
- VIN undervoltage threshold and hysteresis;
- CAN controller recovery behavior;
- execution-supervision timing;
- watchdog timeout;
- external diagnostic encoding.

The next detailed software design activity is the definition of the AeroSense CAN application interface in `CAN_Interface.md`.

---

## 32. Revision History

| Revision | Version | Description | Status |
|---|---|---|---|
| A | 0.1 | Initial AeroSense Rev A diagnostics design | Draft |
