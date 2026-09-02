# AeroSense – Software Requirements Specification

**Project:** AeroSense – Embedded Condition Monitoring & Sensor Node  
**Document ID:** ARS-SWR-001  
**Version:** 0.1  
**Status:** Draft  

---

## 1. Scope

This document defines the software requirements for AeroSense.

The requirements are derived from the system requirements and the
system architecture.

Applicable source documents:

- `System_Requirements.md`
- `Hardware_Requirements.md`
- `System_Architecture.md`
- `Traceability_Matrix.md`

---

## 2. Requirement Convention

Mandatory requirements use the keyword **shall**.

Verification methods:

- **T** – Test
- **A** – Analysis
- **I** – Inspection
- **D** – Demonstration

---

## 3. Software Initialization

| ID | Requirement | Parent | Verification |
|---|---|---|---|
| SW-INIT-001 | The software shall initialize all required hardware peripherals after system startup. | SYS-DIAG-005 | T |
| SW-INIT-002 | The software shall initialize all configured sensor interfaces after system startup. | SYS-DIAG-005 | T |
| SW-INIT-003 | The software shall initialize the CAN communication function after system startup. | SYS-IF-001 | T |
| SW-INIT-004 | The software shall initialize the configuration communication function after system startup. | SYS-IF-003 | T |
| SW-INIT-005 | The software shall perform an initialization check of each connected digital sensor. | SYS-DIAG-005 | T |
| SW-INIT-006 | The software shall determine the initial operational status after completion of initialization checks. | SYS-FUN-006, SYS-DIAG-005 | T |

---

## 4. Sensor Acquisition

### 4.1 Temperature Acquisition

| ID | Requirement | Parent | Verification |
|---|---|---|---|
| SW-ACQ-001 | The software shall acquire ambient temperature measurement data from the temperature sensing hardware. | SYS-FUN-001 | T |
| SW-ACQ-002 | The software shall acquire ambient temperature data at a rate of at least 10 Hz. | SYS-PERF-001 | T |
| SW-ACQ-003 | The software shall make the latest valid temperature measurement available to the measurement processing function. | SYS-FUN-001 | T |

### 4.2 Pressure Acquisition

| ID | Requirement | Parent | Verification |
|---|---|---|---|
| SW-ACQ-004 | The software shall acquire atmospheric pressure measurement data from the pressure sensing hardware. | SYS-FUN-002 | T |
| SW-ACQ-005 | The software shall acquire atmospheric pressure data at a rate of at least 10 Hz. | SYS-PERF-002 | T |
| SW-ACQ-006 | The software shall make the latest valid pressure measurement available to the measurement processing function. | SYS-FUN-002 | T |

### 4.3 Acceleration Acquisition

| ID | Requirement | Parent | Verification |
|---|---|---|---|
| SW-ACQ-007 | The software shall acquire acceleration measurement data along three orthogonal axes. | SYS-FUN-003 | T |
| SW-ACQ-008 | The software shall acquire three-axis acceleration data at a rate of at least 50 Hz. | SYS-PERF-003 | T |
| SW-ACQ-009 | The software shall make the latest valid three-axis acceleration measurement available to the measurement processing function. | SYS-FUN-003 | T |

### 4.4 Supply-Voltage Acquisition

| ID | Requirement | Parent | Verification |
|---|---|---|---|
| SW-ACQ-010 | The software shall acquire supply-voltage measurement data from the supply monitoring hardware. | SYS-FUN-004 | T |
| SW-ACQ-011 | The software shall acquire supply-voltage data at a rate of at least 10 Hz. | SYS-PERF-004 | T |
| SW-ACQ-012 | The software shall make the latest valid supply-voltage measurement available to the measurement processing function. | SYS-FUN-004 | T |

---

## 5. Measurement Processing

| ID | Requirement | Parent | Verification |
|---|---|---|---|
| SW-PROC-001 | The software shall process acquired temperature measurement data for use by other software functions. | SYS-FUN-001, SYS-FUN-005 | T |
| SW-PROC-002 | The software shall process acquired atmospheric pressure measurement data for use by other software functions. | SYS-FUN-002, SYS-FUN-005 | T |
| SW-PROC-003 | The software shall process acquired three-axis acceleration measurement data for use by other software functions. | SYS-FUN-003, SYS-FUN-005 | T |
| SW-PROC-004 | The software shall process acquired supply-voltage measurement data for use by other software functions. | SYS-FUN-004, SYS-FUN-005 | T |
| SW-PROC-005 | The software shall maintain validity information for each measurement type. | SYS-FUN-009 | T |
| SW-PROC-006 | The software shall mark measurement data as invalid when a corresponding sensor failure is detected. | SYS-FUN-009, SYS-DIAG-002 | T |

---

## 6. CAN Communication

| ID | Requirement | Parent | Verification |
|---|---|---|---|
| SW-CAN-001 | The software shall provide measurement data to the CAN communication interface. | SYS-FUN-005, SYS-IF-001 | T |
| SW-CAN-002 | The software shall provide diagnostic information to the CAN communication interface. | SYS-FUN-007, SYS-DIAG-004 | T |
| SW-CAN-003 | The software shall configure the CAN communication function for a nominal bit rate of 500 kbit/s. | SYS-IF-002 | T |
| SW-CAN-004 | The software shall transmit defined measurement messages at their specified periodic update rates. | SYS-PERF-005, SYS-IF-004 | T |
| SW-CAN-005 | The software shall encode CAN message payloads according to the defined message format. | SYS-IF-004 | T |
| SW-CAN-006 | The software shall apply the defined scaling and units to transmitted measurement information. | SYS-IF-004 | A/T |
| SW-CAN-007 | The software shall include or provide the defined validity information for transmitted measurement data. | SYS-IF-004, SYS-FUN-009 | T |
| SW-CAN-008 | The software shall transmit detected fault information through the external CAN communication interface. | SYS-DIAG-004 | T |

---

## 7. Configuration Communication

| ID | Requirement | Parent | Verification |
|---|---|---|---|
| SW-CFG-001 | The software shall support communication with an external host computer through the USB interface. | SYS-IF-003 | T |
| SW-CFG-002 | The software shall provide access to configurable system parameters through the configuration communication interface. | SYS-FUN-008 | T/D |
| SW-CFG-003 | The software shall validate received configuration values before applying them. | SYS-FUN-008 | T |
| SW-CFG-004 | The software shall reject unsupported or invalid configuration commands. | SYS-FUN-008 | T |
| SW-CFG-005 | The software shall provide system identification information through the configuration communication interface. | SYS-FUN-010 | T |

---

## 8. Diagnostics

| ID | Requirement | Parent | Verification |
|---|---|---|---|
| SW-DIAG-001 | The software shall monitor communication with each digital sensor. | SYS-DIAG-001 | T |
| SW-DIAG-002 | The software shall detect loss of communication with a digital sensor. | SYS-DIAG-001 | T |
| SW-DIAG-003 | The software shall set the corresponding measurement validity state to invalid following detection of a sensor communication failure. | SYS-DIAG-002 | T |
| SW-DIAG-004 | The software shall detect an undervoltage condition using the monitored supply-voltage measurement. | SYS-DIAG-003 | T |
| SW-DIAG-005 | The software shall maintain diagnostic status information for detected system faults. | SYS-FUN-007 | T |
| SW-DIAG-006 | The software shall provide detected fault information to the communication function. | SYS-DIAG-004 | T |
| SW-DIAG-007 | The software shall preserve operation of unaffected monitoring functions following failure of an individual sensor. | SYS-DIAG-007 | T |
| SW-DIAG-008 | The software shall update the system operational status based on detected system faults. | SYS-FUN-006 | T |

---

## 9. System Control and Operational States

| ID | Requirement | Parent | Verification |
|---|---|---|---|
| SW-CTRL-001 | The software shall implement the operational states INITIALIZATION, SELF_TEST, NORMAL_OPERATION, DEGRADED_OPERATION, and FAULT. | SYS-FUN-006 | I/T |
| SW-CTRL-002 | The software shall enter INITIALIZATION following software startup. | SYS-DIAG-005 | T |
| SW-CTRL-003 | The software shall enter SELF_TEST after completion of required initialization activities. | SYS-DIAG-005 | T |
| SW-CTRL-004 | The software shall enter NORMAL_OPERATION when all required initialization and self-test checks have completed without a detected fault requiring degraded or fault operation. | SYS-FUN-006, SYS-DIAG-005 | T |
| SW-CTRL-005 | The software shall support DEGRADED_OPERATION when an individual sensor failure permits unaffected monitoring functions to continue operating. | SYS-DIAG-007 | T |
| SW-CTRL-006 | The software shall support transition to FAULT when a detected failure prevents the system from providing its defined minimum functionality. | SYS-FUN-006 | T |
| SW-CTRL-007 | The software shall provide the current operational state to the diagnostic and communication functions. | SYS-FUN-006, SYS-FUN-007 | T |

---

## 10. Watchdog and Execution Supervision

| ID | Requirement | Parent | Verification |
|---|---|---|---|
| SW-WDG-001 | The software shall initialize the hardware watchdog during system startup. | SYS-DIAG-006 | T |
| SW-WDG-002 | The software shall periodically service the hardware watchdog during correct software execution. | SYS-DIAG-006 | T |
| SW-WDG-003 | The software shall not intentionally service the watchdog when the required execution supervision conditions are not satisfied. | SYS-DIAG-006 | T |
| SW-WDG-004 | The software architecture shall support recovery through hardware reset when watchdog servicing is not performed within the configured watchdog period. | SYS-DIAG-006 | T |

---

## 11. Identification and Version Information

| ID | Requirement | Parent | Verification |
|---|---|---|---|
| SW-ID-001 | The software shall provide firmware version information. | SYS-FUN-010, SYS-MFG-005 | T |
| SW-ID-002 | The software shall provide available hardware revision information to the communication function. | SYS-FUN-010, SYS-MFG-005 | T |
| SW-ID-003 | The software shall provide the device identifier to the communication function. | SYS-MFG-004 | T |

---

## 12. Modularity and Maintainability

| ID | Requirement | Parent | Verification |
|---|---|---|---|
| SW-ARCH-001 | The software architecture shall separate sensor acquisition, measurement processing, diagnostics, communication, and system-control functions into defined software components. | SYS-DEV-002 | I |
| SW-ARCH-002 | Sensor-specific communication and control functions shall be separated from application-level measurement processing. | SYS-DEV-002 | I |
| SW-ARCH-003 | CAN communication functions shall be separated from sensor acquisition functions. | SYS-DEV-002 | I |
| SW-ARCH-004 | Diagnostic functions shall be separated from individual sensor driver implementations where practical. | SYS-DEV-002 | I |
| SW-ARCH-005 | The software architecture shall support incremental integration of individual sensor and communication functions. | SYS-DEV-003 | I/D |

---

## 13. Testability

| ID | Requirement | Parent | Verification |
|---|---|---|---|
| SW-TEST-001 | The software shall provide observable measurement outputs required for verification of the sensing functions. | SYS-DEV-003, SYS-DIAG-004 | T |
| SW-TEST-002 | The software shall provide observable diagnostic information required for verification of fault-detection functions. | SYS-FUN-007, SYS-DIAG-004 | T |
| SW-TEST-003 | The software architecture shall support verification of sensor communication failure handling. | SYS-DIAG-001, SYS-DIAG-002 | D/T |
| SW-TEST-004 | The software architecture shall support verification of undervoltage detection behavior. | SYS-DIAG-003 | D/T |
| SW-TEST-005 | The software architecture shall support verification of watchdog recovery behavior. | SYS-DIAG-006 | D/T |
| SW-TEST-006 | The software architecture shall support verification of degraded operation following an individual sensor failure. | SYS-DIAG-007 | D/T |

---

## 14. Software Interface Summary

The preliminary software interfaces are:

| Interface | Software Function |
|---|---|
| Temperature sensor interface | Temperature acquisition |
| Pressure sensor interface | Pressure acquisition |
| Acceleration sensor interface | Three-axis acceleration acquisition |
| Supply-voltage measurement interface | Supply monitoring |
| CAN interface | Measurement and diagnostic communication |
| USB interface | Configuration and development communication |
| Watchdog interface | Software execution supervision |
| Device identification interface | Hardware/Firmware identification |

Detailed driver interfaces and application programming interfaces will be defined during software design.

---

## 15. Open Software Design Parameters

The following software parameters remain TBD:

- sensor communication protocols;
- sensor driver implementation;
- task scheduling architecture;
- bare-metal or RTOS implementation;
- CAN message identifiers;
- CAN payload definitions;
- CAN message scaling;
- CAN transmission periods;
- sensor communication timeout values;
- undervoltage threshold;
- diagnostic fault codes;
- fault recovery strategy;
- configuration command protocol;
- configuration parameter set;
- USB communication class or protocol;
- watchdog timeout;
- detailed state-transition conditions;
- non-volatile configuration storage strategy.

These parameters will be defined during interface definition and detailed software design.

---

## 16. Traceability

Software requirements are linked to their originating system requirements.

The development traceability chain is:

**Stakeholder Need → System Requirement → Software Requirement → Software Design → Implementation → Verification**

Example:

```text
UN-005
The user needs to identify the operational status
and detected faults of the device.
        |
        v
SYS-DIAG-001
The system shall detect loss of communication
with each digital sensor.
        |
        v
SW-DIAG-002
The software shall detect loss of communication
with a digital sensor.
        |
        v
Diagnostic Manager / Sensor Driver
        |
        v
Fault-Injection Test
```

---

## 17. Software Requirement Status

This document defines the preliminary software requirements for AeroSense Rev A.

Detailed implementation decisions are intentionally deferred to the
software design phase.
