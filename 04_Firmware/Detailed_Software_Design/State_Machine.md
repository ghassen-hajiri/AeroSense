# AeroSense – State Machine Design

**Project:** AeroSense – Embedded Condition Monitoring & Sensor Node  
**Document ID:** ARS-SDD-STM-001  
**Revision:** A  
**Version:** 0.1  
**Status:** Draft  

---

## 1. Purpose

This document defines the operational state machine of the AeroSense Rev A firmware.

The state machine coordinates the high-level behavior of the embedded system during:

- startup;
- initialization;
- self-test;
- normal monitoring operation;
- degraded operation;
- critical fault conditions;
- recovery from recoverable faults.

The System Manager owns the operational state and uses information from the Diagnostic Manager and other software services to determine state transitions.

---

## 2. Applicable Documents

The state-machine design is based on:

- `01_Requirements/Software_Requirements.md`
- `02_System_Architecture/System_Architecture.md`
- `02_System_Architecture/Interfaces/Interface_Definition.md`
- `04_Firmware/Detailed_Software_Design/Software_Architecture.md`
- `04_Firmware/Detailed_Software_Design/Driver_Design.md`
- `04_Firmware/Detailed_Software_Design/Diagnostics_Design.md`
- `04_Firmware/Detailed_Software_Design/CAN_Interface.md`

Related detailed software design document:

- `Scheduling_Design.md`

---

# 3. State Machine Objectives

The AeroSense operational state machine shall:

1. provide deterministic startup behavior;
2. distinguish initialization from hardware self-test;
3. enter normal operation when required functions are available;
4. support continued useful operation following non-critical failures;
5. enter a defined fault state following critical failures;
6. support automatic recovery from recoverable degraded conditions;
7. provide the current operational state to communication services;
8. separate system-level decisions from individual device drivers.

---

# 4. Operational States

AeroSense Rev A defines five executing firmware states:

| Value | State | Purpose |
|---:|---|---|
| `0x00` | `INITIALIZATION` | Initialize firmware and MCU resources |
| `0x01` | `SELF_TEST` | Verify required hardware and software functions |
| `0x02` | `NORMAL_OPERATION` | Perform full monitoring operation |
| `0x03` | `DEGRADED_OPERATION` | Continue operation with one or more unavailable non-critical functions |
| `0x04` | `FAULT` | Handle critical system failure |

`POWER_OFF` is a physical system condition and is not implemented as an executing firmware state.

A processor reset causes the software to begin again with `INITIALIZATION`.

---

# 5. State Representation

The state may be represented in firmware as:

```c
typedef enum
{
    SYSTEM_STATE_INITIALIZATION = 0x00,
    SYSTEM_STATE_SELF_TEST      = 0x01,
    SYSTEM_STATE_NORMAL         = 0x02,
    SYSTEM_STATE_DEGRADED       = 0x03,
    SYSTEM_STATE_FAULT          = 0x04
} SystemState_t;
```

The numerical representation is consistent with the state encoding defined in `CAN_Interface.md`.

---

# 6. State Machine Overview

```text
                         Reset
                           |
                           v
                  +------------------+
                  |  INITIALIZATION  |
                  +--------+---------+
                           |
                  initialization complete
                           |
                           v
                  +------------------+
                  |    SELF_TEST     |
                  +---+----------+---+
                      |          |
           no degraded/          | critical fault
           critical fault       |
                      |          |
                      v          v
             +---------------+  +-------+
             |    NORMAL     |  | FAULT |
             |   OPERATION   |  +-------+
             +------+--------+      ^
                    |               |
           degraded |               | critical
           fault    |               | fault
                    v               |
             +---------------+------+
             |   DEGRADED    |
             |   OPERATION   |
             +------+--------+
                    |
             degraded faults
                recovered
                    |
                    v
             +---------------+
             |    NORMAL     |
             |   OPERATION   |
             +---------------+
```

A critical fault detected during any operational phase requiring immediate system-level fault handling shall cause transition to `FAULT`.

---

# 7. Transition Summary

| Current State | Condition | Next State |
|---|---|---|
| Reset | Processor starts | `INITIALIZATION` |
| `INITIALIZATION` | Initialization sequence completed | `SELF_TEST` |
| `INITIALIZATION` | Critical initialization failure | `FAULT` |
| `SELF_TEST` | No degraded or critical fault | `NORMAL_OPERATION` |
| `SELF_TEST` | One or more degraded faults, no critical fault | `DEGRADED_OPERATION` |
| `SELF_TEST` | One or more critical faults | `FAULT` |
| `NORMAL_OPERATION` | One or more degraded faults | `DEGRADED_OPERATION` |
| `NORMAL_OPERATION` | One or more critical faults | `FAULT` |
| `DEGRADED_OPERATION` | All degraded faults recovered | `NORMAL_OPERATION` |
| `DEGRADED_OPERATION` | One or more critical faults | `FAULT` |
| `FAULT` | Processor reset | `INITIALIZATION` |

Fault recovery that requires a reset shall therefore pass through the complete initialization and self-test sequence.

---

# 8. INITIALIZATION State

## 8.1 Purpose

`INITIALIZATION` establishes the software and MCU environment required for subsequent AeroSense operation.

The state begins immediately after processor startup and completion of the minimum startup code required to execute the application.

---

## 8.2 Initialization Activities

Initialization includes, as applicable:

```text
STM32 HAL initialization
        |
        v
System clock configuration
        |
        v
GPIO initialization
        |
        v
ADC1 initialization
        |
        v
I2C2 initialization
        |
        v
SPI1 initialization
        |
        v
FDCAN1 initialization
        |
        v
USB initialization
        |
        v
AeroSense software services
        |
        v
Diagnostic Manager initialization
        |
        v
System Manager initialization
```

Device communication verification is primarily performed during `SELF_TEST`.

---

## 8.3 Initialization Behavior

During `INITIALIZATION`:

- application measurements shall initially be marked invalid;
- diagnostic data structures shall be initialized;
- the operational state shall be set to `INITIALIZATION`;
- application-level cyclic monitoring shall not yet be considered operational;
- required MCU peripherals and software services shall be prepared.

---

## 8.4 Exit Conditions

Normal exit:

```text
Required initialization completed
              |
              v
          SELF_TEST
```

If initialization encounters a critical failure that prevents meaningful execution of the self-test or application:

```text
Critical initialization failure
              |
              v
             FAULT
```

Non-critical device availability shall normally be evaluated during `SELF_TEST`.

---

# 9. SELF_TEST State

## 9.1 Purpose

`SELF_TEST` verifies that the hardware interfaces and devices required by AeroSense are operational.

---

## 9.2 Self-Test Activities

The startup self-test includes, where applicable:

| Function | Test |
|---|---|
| STTS22H | I2C communication and device identification |
| LPS22HH | I2C communication and device identification |
| LIS2DW12 | SPI communication and device identification |
| VIN monitor | Valid ADC acquisition |
| CAN | Controller initialization/start status |
| USB | Interface initialization status |
| Internal services | Software initialization status |

The self-test results shall be reported to the Diagnostic Manager.

---

## 9.3 Successful Self-Test

If no `DEGRADED` or `CRITICAL` diagnostic condition is active:

```text
SELF_TEST
    |
    | self-test complete
    | no relevant fault
    v
NORMAL_OPERATION
```

---

## 9.4 Partially Successful Self-Test

Failure of a non-critical function shall permit degraded operation where useful monitoring capability remains.

Example:

```text
STTS22H unavailable
        |
        v
TEMP_SENSOR_COMM_FAULT
        |
        v
DEGRADED_OPERATION
```

Other available measurements continue to operate.

---

## 9.5 Critical Self-Test Failure

If a critical diagnostic condition is active:

```text
SELF_TEST
    |
    | critical fault
    v
FAULT
```

---

# 10. NORMAL_OPERATION State

## 10.1 Purpose

`NORMAL_OPERATION` represents full AeroSense monitoring operation.

All required baseline monitoring functions are available and no `DEGRADED` or `CRITICAL` diagnostic condition is active.

---

## 10.2 Normal Operation Activities

During `NORMAL_OPERATION`, AeroSense performs:

- temperature acquisition;
- pressure acquisition;
- acceleration acquisition;
- input-voltage monitoring;
- measurement processing;
- runtime diagnostics;
- cyclic CAN transmission;
- supported USB communication;
- status indication;
- execution supervision;
- watchdog servicing when supervision conditions are satisfied.

Activities are executed according to `Scheduling_Design.md`.

---

## 10.3 Transition to Degraded Operation

If one or more `DEGRADED` diagnostic conditions become confirmed:

```text
NORMAL_OPERATION
       |
       | degraded fault
       v
DEGRADED_OPERATION
```

Example:

```text
Pressure sensor stops responding
            |
            v
PRESS_SENSOR_COMM_FAULT
            |
            v
DEGRADED_OPERATION
```

---

## 10.4 Transition to Fault

If one or more `CRITICAL` diagnostic conditions become active:

```text
NORMAL_OPERATION
       |
       | critical fault
       v
FAULT
```

---

# 11. DEGRADED_OPERATION State

## 11.1 Purpose

`DEGRADED_OPERATION` allows AeroSense to continue providing useful monitoring functionality when one or more non-critical functions are unavailable.

This state implements graceful degradation.

---

## 11.2 Degraded Operation Behavior

During `DEGRADED_OPERATION`:

- unaffected sensor acquisition shall continue;
- valid measurements shall continue to be processed;
- failed measurements shall remain invalid;
- CAN communication shall continue where available;
- diagnostic status shall be reported;
- failed recoverable devices may be periodically retried;
- execution supervision shall remain active;
- the current system state shall be externally reported.

---

## 11.3 Example

If the accelerometer fails:

```text
LIS2DW12 communication failure
             |
             v
ACCEL_SENSOR_COMM_FAULT
             |
             +---- Acceleration X INVALID
             +---- Acceleration Y INVALID
             +---- Acceleration Z INVALID
             |
             +---- Temperature continues
             +---- Pressure continues
             +---- VIN monitoring continues
             +---- CAN continues
             |
             v
DEGRADED_OPERATION
```

---

## 11.4 Recovery to Normal Operation

Recoverable degraded faults may be cleared after the diagnostic recovery criteria have been satisfied.

```text
DEGRADED_OPERATION
        |
        | failed device available again
        | recovery confirmed
        v
NORMAL_OPERATION
```

Transition to `NORMAL_OPERATION` shall occur only when no active `DEGRADED` or `CRITICAL` diagnostic condition remains.

---

## 11.5 Multiple Degraded Faults

Multiple degraded faults may exist simultaneously.

Example:

```text
TEMP_SENSOR_COMM_FAULT
PRESS_SENSOR_COMM_FAULT
```

Recovery of only one fault does not permit transition to `NORMAL_OPERATION`.

The system remains in `DEGRADED_OPERATION` until all degraded faults are cleared.

---

## 11.6 Escalation to Fault

A critical fault occurring during degraded operation causes:

```text
DEGRADED_OPERATION
        |
        | critical fault
        v
FAULT
```

---

# 12. FAULT State

## 12.1 Purpose

`FAULT` represents a system condition in which continued normal or degraded operation cannot be considered reliable because a critical diagnostic condition is active.

---

## 12.2 Fault State Entry

Examples of conditions capable of causing entry into `FAULT` include:

- confirmed critical supply undervoltage;
- execution-supervision failure;
- critical initialization failure;
- another diagnostic explicitly classified as `CRITICAL`.

The exact diagnostic classification is defined in `Diagnostics_Design.md`.

---

## 12.3 Fault State Behavior

During `FAULT`, AeroSense shall:

- stop treating normal monitoring operation as fully operational;
- preserve active diagnostic information where possible;
- provide fault indication where the available interfaces permit;
- avoid reporting invalid measurements as valid;
- maintain bounded software execution;
- allow the hardware watchdog to perform its intended safety/recovery function.

The exact behavior depends on the nature of the critical fault.

For example, communication cannot be guaranteed if the underlying supply voltage or MCU execution itself is compromised.

---

## 12.4 Exit from FAULT

The Rev A baseline does not automatically transition directly from `FAULT` to `NORMAL_OPERATION`.

Recovery is performed through a processor reset:

```text
FAULT
  |
  | processor reset
  v
INITIALIZATION
  |
  v
SELF_TEST
  |
  +---- healthy ----> NORMAL_OPERATION
  |
  +---- degraded ---> DEGRADED_OPERATION
  |
  +---- critical ---> FAULT
```

This ensures that the complete initialization and self-test sequence is performed following a critical fault.

---

# 13. Diagnostic Severity and State Relationship

The System Manager shall use diagnostic information as follows:

| Highest Active Severity | Operational Result |
|---|---|
| None / INFO only | `NORMAL_OPERATION` |
| DEGRADED | `DEGRADED_OPERATION` |
| CRITICAL | `FAULT` |

`INFO` diagnostics do not by themselves force a degraded state.

For example:

```text
USB_COMM_FAULT = INFO
```

may be active while AeroSense remains in:

```text
NORMAL_OPERATION
```

provided required monitoring functions remain available.

---

# 14. Measurement Validity by State

Operational state and individual measurement validity are separate concepts.

| State | Measurement Behavior |
|---|---|
| `INITIALIZATION` | Measurements initially invalid |
| `SELF_TEST` | Measurements used for test may be acquired; operational validity not yet fully established |
| `NORMAL_OPERATION` | Available measurements valid according to acquisition result |
| `DEGRADED_OPERATION` | Unaffected measurements valid; failed functions invalid |
| `FAULT` | Validity determined conservatively according to fault condition |

A degraded system does not imply that all measurements are invalid.

Example:

```text
State = DEGRADED_OPERATION

Temperature = INVALID
Pressure    = VALID
Acceleration= VALID
VIN         = VALID
```

---

# 15. CAN Behavior by State

The current system state shall be represented in `AEROSENSE_STATUS` using the encoding defined in `CAN_Interface.md`.

Where CAN communication remains available:

| State | CAN Behavior |
|---|---|
| `INITIALIZATION` | Status communication may be limited until CAN initialization completes |
| `SELF_TEST` | Status may report `SELF_TEST` |
| `NORMAL_OPERATION` | Normal cyclic transmission |
| `DEGRADED_OPERATION` | Cyclic transmission continues with validity and fault information |
| `FAULT` | Fault/status reporting where technically possible |

A failed sensor shall not stop transmission of unrelated valid measurement messages.

---

# 16. Status LED Behavior

The Status LED may provide a simple local indication of the operational state.

The Rev A baseline indication is:

| State | LED Behavior |
|---|---|
| `INITIALIZATION` | OFF |
| `SELF_TEST` | Slow blink |
| `NORMAL_OPERATION` | ON |
| `DEGRADED_OPERATION` | Slow blink |
| `FAULT` | Fast blink |

The exact blink timing is defined in `Scheduling_Design.md`.

The Status LED Driver only controls the GPIO. Interpretation of the system state remains the responsibility of the System Manager.

---

# 17. Recovery Concept

Recovery from a non-critical device fault is coordinated between the driver, Diagnostic Manager, and System Manager.

Example:

```text
Temperature Driver
       |
       | communication failure
       v
Diagnostic Manager
       |
       | fault confirmed
       v
TEMP_SENSOR_COMM_FAULT
       |
       v
System Manager
       |
       v
DEGRADED_OPERATION
       |
       | periodic device retry
       v
Temperature Driver
       |
       | communication restored
       v
Diagnostic Manager
       |
       | recovery confirmed
       v
Clear TEMP_SENSOR_COMM_FAULT
       |
       v
System Manager
       |
       | no remaining degraded/critical fault
       v
NORMAL_OPERATION
```

The state machine therefore does not implement device-specific recovery itself.

---

# 18. Fault Priority

If multiple transition conditions occur during the same application cycle, the highest severity shall determine the next operational state.

Priority:

```text
CRITICAL
   >
DEGRADED
   >
NORMAL
```

Example:

```text
TEMP_SENSOR_COMM_FAULT   = DEGRADED
VIN_UNDERVOLTAGE         = CRITICAL
```

Result:

```text
FAULT
```

The presence of a degraded fault shall never override a simultaneously active critical fault.

---

# 19. State Evaluation

The System Manager may evaluate the state using logic conceptually equivalent to:

```c
if (DIAG_HasCriticalFault())
{
    next_state = SYSTEM_STATE_FAULT;
}
else if (DIAG_HasDegradedFault())
{
    next_state = SYSTEM_STATE_DEGRADED;
}
else
{
    next_state = SYSTEM_STATE_NORMAL;
}
```

This evaluation applies after initialization and self-test have completed.

`INITIALIZATION` and `SELF_TEST` use their own completion conditions.

---

# 20. Proposed System Manager Interface

The System Manager may expose:

```c
void SYSTEM_Init(void);

void SYSTEM_Update(void);

SystemState_t SYSTEM_GetState(void);
```

Internally, state-specific functions may be used:

```c
static void SYSTEM_HandleInitialization(void);

static void SYSTEM_HandleSelfTest(void);

static void SYSTEM_HandleNormalOperation(void);

static void SYSTEM_HandleDegradedOperation(void);

static void SYSTEM_HandleFault(void);
```

The exact function organization may be refined during implementation while preserving the behavior defined in this document.

---

# 21. State Entry Handling

Where state-specific initialization is required, the System Manager should distinguish between:

```text
State Entry
```

and:

```text
Repeated State Execution
```

Conceptually:

```c
if (new_state != current_state)
{
    SYSTEM_ExitState(current_state);

    current_state = new_state;

    SYSTEM_EnterState(current_state);
}

SYSTEM_ExecuteState(current_state);
```

This prevents state-entry actions from being executed repeatedly during every scheduler cycle.

---

# 22. State Transition Logging

For development and verification, state transitions should be observable through available debugging or communication interfaces where practical.

A transition may conceptually be represented as:

```text
Previous State
      |
      v
Transition Reason
      |
      v
New State
```

Example:

```text
NORMAL_OPERATION
        |
        | TEMP_SENSOR_COMM_FAULT
        v
DEGRADED_OPERATION
```

This supports integration testing and fault-injection verification.

---

# 23. Watchdog Interaction

The state machine shall not unconditionally refresh the hardware watchdog.

Watchdog refresh is controlled through Execution Supervision.

In normal and degraded operation:

```text
Required cyclic functions executed correctly
                  |
                  v
       Execution supervision valid
                  |
                  v
          Watchdog refresh
```

If execution supervision fails, watchdog servicing may be withheld so that the MCU can reset.

The detailed timing is defined in `Scheduling_Design.md`.

---

# 24. State Machine Timing

The System Manager shall be evaluated cyclically.

State transitions do not require a dedicated RTOS task.

The bare-metal scheduler invokes the System Manager according to the timing defined in `Scheduling_Design.md`.

The state evaluation period shall be sufficiently short to satisfy required diagnostic response times.

---

# 25. State Machine and Interrupts

Operational state transitions shall not normally be executed directly inside interrupt service routines.

Interrupts may report events or update low-level status.

The System Manager evaluates those conditions during normal cyclic execution.

Conceptually:

```text
Interrupt
   |
   v
Set event / status
   |
   v
Return from interrupt
   |
   v
Scheduler
   |
   v
System Manager
   |
   v
State evaluation
```

This keeps system-level behavior deterministic and centralized.

---

# 26. State Machine Verification

State-machine verification shall include transition testing.

Minimum transition tests include:

| Initial Condition | Injected Condition | Expected State |
|---|---|---|
| Reset | Normal startup | `INITIALIZATION` |
| `INITIALIZATION` | Initialization complete | `SELF_TEST` |
| `SELF_TEST` | All required functions healthy | `NORMAL_OPERATION` |
| `SELF_TEST` | One sensor unavailable | `DEGRADED_OPERATION` |
| `SELF_TEST` | Critical fault | `FAULT` |
| `NORMAL_OPERATION` | Temperature sensor failure | `DEGRADED_OPERATION` |
| `NORMAL_OPERATION` | Pressure sensor failure | `DEGRADED_OPERATION` |
| `NORMAL_OPERATION` | Accelerometer failure | `DEGRADED_OPERATION` |
| `NORMAL_OPERATION` | Critical undervoltage | `FAULT` |
| `DEGRADED_OPERATION` | Fault successfully recovered | `NORMAL_OPERATION` |
| `DEGRADED_OPERATION` | Critical fault occurs | `FAULT` |
| `FAULT` | Processor reset and healthy restart | `INITIALIZATION` |

Detailed test procedures shall be maintained under `06_Verification_Validation`.

---

# 27. Requirement Traceability

The state-machine design primarily supports:

| Requirement Group | State Machine Contribution |
|---|---|
| SW-INIT-* | Startup and initialization behavior |
| SW-ACQ-* | Activation of cyclic acquisition during operation |
| SW-DIAG-* | Diagnostic-driven operational behavior |
| SW-CTRL-* | Definition of operational states and transitions |
| SW-WDG-* | Interaction with execution supervision |
| SW-CAN-* | External state reporting |
| SW-ARCH-* | Centralized system-level control |
| SW-TEST-* | Defined and testable state transitions |

Detailed bidirectional traceability shall be maintained in the project traceability matrix.

---

# 28. Rev A State Machine Baseline

The AeroSense Rev A operational behavior is summarized as:

```text
RESET
  |
  v
INITIALIZATION
  |
  v
SELF_TEST
  |
  +---- Healthy ------------------> NORMAL_OPERATION
  |                                      |
  |                                      | degraded fault
  |                                      v
  |                               DEGRADED_OPERATION
  |                                      |
  |                                      | recovery
  |                                      v
  |                               NORMAL_OPERATION
  |
  +---- Degraded fault ----------> DEGRADED_OPERATION
  |
  +---- Critical fault ----------> FAULT

NORMAL_OPERATION
       |
       +---- critical fault -----> FAULT

DEGRADED_OPERATION
       |
       +---- critical fault -----> FAULT

FAULT
  |
  +---- processor reset ---------> INITIALIZATION
```

This baseline provides deterministic startup, graceful degradation, fault containment, and controlled recovery for AeroSense Rev A.

---

# 29. Detailed Design Status

This document establishes the AeroSense Rev A operational state machine.

The remaining detailed timing aspects are defined in:

`Scheduling_Design.md`

This includes:

- cyclic execution periods;
- sensor acquisition periods;
- CAN transmission periods;
- diagnostic evaluation timing;
- state-machine evaluation timing;
- LED timing;
- recovery retry timing;
- execution supervision;
- watchdog timing.

After completion of `Scheduling_Design.md`, the AeroSense Rev A Detailed Software Design baseline is complete and firmware implementation can begin.

---

## 30. Revision History

| Revision | Version | Description | Status |
|---|---|---|---|
| A | 0.1 | Initial AeroSense Rev A state-machine design | Draft |
