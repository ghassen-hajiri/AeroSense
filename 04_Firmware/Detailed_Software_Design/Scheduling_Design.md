# AeroSense – Scheduling Design

**Project:** AeroSense – Embedded Condition Monitoring & Sensor Node  
**Document ID:** ARS-SDD-SCH-001  
**Revision:** A  
**Version:** 0.1  
**Status:** Draft  

---

## 1. Purpose

This document defines the scheduling and timing design of the AeroSense Rev A firmware.

AeroSense Rev A uses a cooperative bare-metal cyclic execution model without an RTOS.

The scheduling design defines:

- scheduler time base;
- cyclic function periods;
- sensor acquisition timing;
- CAN transmission timing;
- diagnostic processing;
- system-state evaluation;
- recovery processing;
- status LED timing;
- execution supervision;
- watchdog interaction.

The objective is deterministic and understandable firmware execution while satisfying the AeroSense acquisition and communication requirements.

---

## 2. Applicable Documents

The scheduling design is based on:

- `01_Requirements/Software_Requirements.md`
- `04_Firmware/Detailed_Software_Design/Software_Architecture.md`
- `04_Firmware/Detailed_Software_Design/Driver_Design.md`
- `04_Firmware/Detailed_Software_Design/Diagnostics_Design.md`
- `04_Firmware/Detailed_Software_Design/CAN_Interface.md`
- `04_Firmware/Detailed_Software_Design/State_Machine.md`

---

# 3. Scheduling Architecture

AeroSense Rev A uses a cooperative time-triggered scheduler executed from the main application loop.

No RTOS task scheduler is required.

The conceptual execution model is:

```text
MCU Reset
   |
   v
Initialization
   |
   v
Self-Test
   |
   v
+----------------------------------+
|         Main Superloop           |
|                                  |
| Read system time                 |
|                                  |
| Execute due cyclic functions     |
|                                  |
| Process diagnostics              |
|                                  |
| Evaluate system state            |
|                                  |
| Perform execution supervision    |
|                                  |
| Refresh watchdog if permitted    |
+----------------+-----------------+
                 |
                 +---- repeat
```

The scheduler shall not intentionally wait for the next execution period.

The main loop continues executing and invokes functions when their corresponding period has elapsed.

---

# 4. System Time Base

The STM32 HAL system tick provides the baseline software time reference.

The scheduler uses a millisecond time base.

Conceptually:

```c
uint32_t now_ms = HAL_GetTick();
```

The scheduling implementation shall use unsigned time differences so that normal 32-bit tick counter wraparound is handled correctly.

Example:

```c
if ((uint32_t)(now_ms - last_run_ms) >= period_ms)
{
    last_run_ms = now_ms;
    /* execute function */
}
```

The application shall not depend on equality with an exact tick value.

---

# 5. Scheduling Principles

The Rev A scheduler shall follow these principles:

1. cyclic functions execute only when due;
2. no function shall intentionally block indefinitely;
3. hardware communication shall use finite timeouts;
4. interrupt service routines shall remain short;
5. application processing shall normally execute outside interrupt context;
6. sensor acquisition and CAN transmission shall remain logically separated;
7. critical diagnostic and system-state processing shall execute frequently;
8. watchdog refresh shall depend on successful execution supervision;
9. execution timing shall be verified on the target hardware;
10. scheduling periods shall be traceable to software requirements or design decisions.

---

# 6. Baseline Scheduling Table

The AeroSense Rev A baseline schedule is:

| Function | Period | Rate |
|---|---:|---:|
| System Manager evaluation | 10 ms | 100 Hz |
| Diagnostic Manager update | 10 ms | 100 Hz |
| LIS2DW12 acceleration acquisition | 20 ms | 50 Hz |
| Acceleration CAN transmission | 20 ms | 50 Hz |
| STTS22H temperature acquisition | 100 ms | 10 Hz |
| LPS22HH pressure acquisition | 100 ms | 10 Hz |
| VIN acquisition | 100 ms | 10 Hz |
| Environmental CAN transmission | 100 ms | 10 Hz |
| Power CAN transmission | 100 ms | 10 Hz |
| Status CAN transmission | 1000 ms | 1 Hz |
| Recovery processing | 1000 ms | 1 Hz |
| LED update | 100 ms | 10 Hz |
| Execution supervision | 10 ms | 100 Hz |

These periods define the Rev A software baseline.

---

# 7. Sensor Acquisition Scheduling

## 7.1 Temperature

The STTS22H temperature measurement shall be acquired every:

```text
100 ms
```

Equivalent rate:

```text
10 Hz
```

Execution:

```text
100 ms due
    |
    v
STTS22H_ReadTemperature()
    |
    +---- success --> update temperature
    |
    +---- failure --> update diagnostic evidence
```

---

## 7.2 Pressure

The LPS22HH pressure measurement shall be acquired every:

```text
100 ms
```

Equivalent rate:

```text
10 Hz
```

The pressure acquisition shall be independent of temperature acquisition failure.

---

## 7.3 Acceleration

The LIS2DW12 shall be acquired every:

```text
20 ms
```

Equivalent rate:

```text
50 Hz
```

All three acceleration axes shall be acquired as part of the same measurement cycle.

---

## 7.4 Input Voltage

VIN shall be acquired every:

```text
100 ms
```

Equivalent rate:

```text
10 Hz
```

The VIN Monitor Driver converts the ADC result into volts before the measurement is provided to the Measurement Manager.

---

# 8. CAN Transmission Scheduling

CAN transmission uses the latest available data maintained by the Measurement Manager.

The CAN Message Manager shall not directly trigger sensor acquisition.

The baseline CAN schedule is:

| Message | CAN ID | Period |
|---|---:|---:|
| `AEROSENSE_ENVIRONMENT` | `0x100` | 100 ms |
| `AEROSENSE_ACCELERATION` | `0x101` | 20 ms |
| `AEROSENSE_POWER` | `0x102` | 100 ms |
| `AEROSENSE_STATUS` | `0x103` | 1000 ms |

This is consistent with `CAN_Interface.md`.

---

# 9. Acquisition Before Transmission

Where practical, measurement acquisition shall occur before transmission of the corresponding CAN message.

Example:

```text
Temperature acquisition
Pressure acquisition
        |
        v
Measurement Manager updated
        |
        v
Environmental CAN message
```

Similarly:

```text
Acceleration acquisition
        |
        v
Measurement Manager updated
        |
        v
Acceleration CAN message
```

This ensures that CAN transmission normally contains the most recent available measurement.

---

# 10. Execution Phasing

Functions with the same nominal period should not all be started at exactly the same scheduler instant where practical.

A conceptual 100 ms schedule may use:

```text
0 ms     Temperature acquisition

10 ms    Pressure acquisition

20 ms    VIN acquisition

30 ms    Environmental CAN transmission

40 ms    Power CAN transmission
```

The exact implementation may use execution offsets or ordered function calls.

The purpose is to distribute processing and communication load rather than creating unnecessary execution bursts.

---

# 11. 20 ms Acceleration Cycle

The acceleration processing path is:

```text
Every 20 ms
     |
     v
Read LIS2DW12
     |
     v
Update Measurement Manager
     |
     v
Update acceleration diagnostic evidence
     |
     v
Construct CAN 0x101
     |
     v
Request CAN transmission
```

The complete processing chain shall finish before the next required 20 ms acceleration cycle under normal operating conditions.

---

# 12. Diagnostic Scheduling

The Diagnostic Manager shall be evaluated every:

```text
10 ms
```

This allows confirmed diagnostic information to be propagated rapidly to the System Manager.

Device-specific fault evidence may be generated at the rate of the corresponding device operation.

Example:

```text
Temperature acquisition every 100 ms
               |
               v
       communication result
               |
               v
       diagnostic evidence
               |
               v
Diagnostic Manager evaluation
```

A diagnostic cannot be confirmed faster than the underlying function is observed unless a separate asynchronous hardware indication exists.

---

# 13. Diagnostic Confirmation Timing

Diagnostics using consecutive failure counters shall have a known detection time.

For a cyclic function with period:

```text
T
```

and a confirmation requirement of:

```text
N failures
```

the approximate fault confirmation time is:

```text
T_detection ≈ N × T
```

Example:

```text
Temperature acquisition = 100 ms
Failure confirmation     = 3 consecutive failures

Approximate confirmation time = 300 ms
```

For the Rev A baseline, recoverable sensor communication faults should use:

```text
3 consecutive failed acquisitions
```

before being considered confirmed.

This prevents a single transient communication error from immediately forcing degraded operation.

---

# 14. Diagnostic Recovery Timing

Recoverable sensor faults shall require:

```text
3 consecutive successful acquisitions
```

before the corresponding fault is cleared.

Example:

```text
Temperature sensor recovered
        |
        v
Successful acquisition #1
        |
        v
Successful acquisition #2
        |
        v
Successful acquisition #3
        |
        v
Clear TEMP_SENSOR_COMM_FAULT
```

For a 100 ms sensor period, recovery confirmation therefore requires approximately:

```text
300 ms
```

after successful communication has resumed.

---

# 15. Device Recovery Retry

If a sensor is unavailable and normal acquisition cannot continue, the firmware shall periodically retry communication.

Baseline retry period:

```text
1000 ms
```

The recovery task shall attempt to re-establish communication with recoverable failed devices.

A successful retry does not immediately clear a confirmed fault.

Normal recovery confirmation rules shall still apply.

---

# 16. System Manager Scheduling

The System Manager shall evaluate the operational state every:

```text
10 ms
```

Conceptually:

```text
Diagnostic Manager
       |
       v
Current diagnostic severity
       |
       v
System Manager
       |
       v
State evaluation
```

The System Manager shall use the transition rules defined in `State_Machine.md`.

---

# 17. State Transition Response

Following confirmation of a diagnostic condition, the System Manager shall evaluate the resulting state during the next scheduled System Manager execution.

With a 10 ms System Manager period, additional state-transition latency after diagnostic confirmation is bounded approximately by one System Manager period, excluding execution time.

---

# 18. LED Scheduling

The status LED is updated every:

```text
100 ms
```

The baseline state indication is:

| State | LED Behavior |
|---|---|
| `INITIALIZATION` | OFF |
| `SELF_TEST` | Slow blink |
| `NORMAL_OPERATION` | ON |
| `DEGRADED_OPERATION` | Slow blink |
| `FAULT` | Fast blink |

The baseline blink timing is:

```text
Slow blink:
500 ms ON
500 ms OFF

Fast blink:
100 ms ON
100 ms OFF
```

The LED implementation shall use elapsed time rather than blocking delays.

For example, LED blinking shall not use:

```c
HAL_Delay(500);
```

inside normal cyclic application processing.

---

# 19. Non-Blocking LED Example

Conceptually:

```c
if ((now_ms - last_led_toggle_ms) >= led_period_ms)
{
    last_led_toggle_ms = now_ms;
    STATUS_LED_Toggle();
}
```

This allows sensor acquisition, CAN communication, diagnostics, and state processing to continue while the LED is blinking.

---

# 20. Communication Timeout Policy

Driver operations using blocking STM32 HAL functions shall use finite timeouts.

Timeout values shall be significantly shorter than the corresponding cyclic execution period.

For example:

```text
Acceleration period = 20 ms
```

An SPI operation shall not be allowed to block for a comparable or longer period under normal operation.

The initial implementation shall use conservative finite timeouts and verify actual transaction duration during hardware bring-up.

If blocking operations threaten required timing, the affected interface shall be migrated to interrupt- or DMA-based operation.

---

# 21. Interrupt Scheduling Policy

Interrupts shall be used primarily for hardware events requiring timely response.

Interrupt handlers shall remain short.

Typical interrupt behavior:

```text
Hardware Interrupt
       |
       v
Capture event / data
       |
       v
Set software flag
       |
       v
Return
       |
       v
Main Scheduler
       |
       v
Process event
```

Complex diagnostic evaluation, physical-unit conversion, state transitions, and CAN payload construction shall normally remain outside interrupt context.

---

# 22. Main Superloop Design

The application execution may conceptually follow:

```c
while (1)
{
    uint32_t now_ms = HAL_GetTick();

    Scheduler_Update(now_ms);

    ExecutionSupervision_Update(now_ms);
}
```

A conceptual scheduler may contain:

```c
if (task_due_10ms)
{
    DIAG_Update();
    SYSTEM_Update();
}

if (task_due_20ms)
{
    AcquireAcceleration();
    CANMSG_TransmitAcceleration();
}

if (task_due_100ms)
{
    AcquireTemperature();
    AcquirePressure();
    AcquireVIN();

    CANMSG_TransmitEnvironment();
    CANMSG_TransmitPower();

    StatusLED_Update();
}

if (task_due_1000ms)
{
    Recovery_Update();
    CANMSG_TransmitStatus();
}
```

The final implementation may phase individual 100 ms activities to distribute execution load.

---

# 23. Scheduler Data Representation

A simple task representation may be used:

```c
typedef struct
{
    uint32_t period_ms;
    uint32_t last_run_ms;
} SchedulerTask_t;
```

Alternatively, explicit timestamps may be maintained for each cyclic function.

The Rev A scheduler does not require dynamic task creation or task priorities.

---

# 24. Scheduler Overrun

A scheduler overrun occurs when a required cyclic function cannot complete in time for its next required execution.

The firmware shall not intentionally queue an unlimited number of missed executions.

If a task is late, the scheduler should execute the required function once and continue from a controlled time reference.

Persistent overruns shall be detectable through Execution Supervision.

---

# 25. Execution Supervision

Execution Supervision verifies that required cyclic activities are being executed.

The supervision concept uses execution flags.

Example:

```text
Acceleration task executed  ----+
                                |
100 ms tasks executed ---------+|
                               ||
Diagnostics executed ----------+|
                               ||
System Manager executed -------+|
                               ||
                               vv
                     Execution Supervision
                               |
                    all required functions?
                         /             \
                       yes             no
                        |               |
                        v               v
                 Watchdog allowed   Watchdog not
                  to refresh         refreshed
```

The exact flag implementation may be refined during firmware implementation.

---

# 26. Watchdog Strategy

The independent hardware watchdog provides recovery from loss of normal firmware execution.

The watchdog shall not be refreshed unconditionally from the main loop.

Instead:

```text
Required scheduler functions completed
              |
              v
Execution supervision successful
              |
              v
WATCHDOG_Refresh()
```

If required execution does not occur:

```text
Execution supervision failure
              |
              v
Do not refresh watchdog
              |
              v
Watchdog timeout
              |
              v
MCU Reset
```

---

# 27. Watchdog Timeout

The Rev A baseline watchdog timeout shall be selected significantly longer than the longest normal supervised execution window while remaining short enough to recover from a stalled application.

Initial design target:

```text
Watchdog timeout ≈ 2 seconds
```

The exact achievable timeout depends on the STM32 IWDG configuration and oscillator characteristics and shall be calculated and verified during implementation.

The final configured value shall be documented after target-hardware verification.

---

# 28. Watchdog Supervision Window

Execution supervision shall confirm required cyclic execution within a shorter interval than the watchdog timeout.

A baseline supervision window of:

```text
500 ms
```

is used for Rev A.

Within each supervision window, all required cyclic categories shall demonstrate successful scheduler execution.

If the supervision criteria are satisfied, watchdog refresh is permitted.

---

# 29. Startup and Watchdog

The watchdog shall be enabled only after the firmware has reached the point where the normal supervision mechanism can reliably service it, unless hardware or configuration constraints require earlier activation.

Startup execution shall therefore be considered separately from normal cyclic supervision.

Once enabled, watchdog servicing shall follow the execution-supervision rules.

---

# 30. CPU Load Considerations

The Rev A schedule is intentionally lightweight.

The highest application-level periodic rate is:

```text
50 Hz
```

for acceleration acquisition and transmission.

The design therefore does not require an RTOS for timing reasons.

Actual execution times shall nevertheless be measured on target hardware.

The implementation shall verify that the worst-case cyclic execution remains comfortably below the shortest required 20 ms period.

---

# 31. Timing Measurement

During integration, execution time may be measured using:

- debugger timing;
- GPIO instrumentation;
- MCU cycle counter where available;
- timestamp logging.

A GPIO timing method may conceptually use:

```text
GPIO HIGH
   |
   v
Execute function
   |
   v
GPIO LOW
```

An oscilloscope or logic analyzer can then measure actual execution duration.

---

# 32. Scheduling and Fault Isolation

Failure of one device shall not intentionally block unrelated cyclic functions.

Example:

```text
STTS22H unavailable
        |
        v
I2C operation times out
        |
        v
Temperature fault evidence
        |
        +---- Pressure scheduling continues
        +---- Acceleration scheduling continues
        +---- VIN scheduling continues
        +---- CAN scheduling continues
```

This requires finite communication timeouts.

---

# 33. Timing Summary

The final Rev A baseline timing is:

```text
10 ms
├── Diagnostics
├── System Manager
└── Execution Supervision

20 ms
├── Acceleration Acquisition
└── CAN 0x101 Acceleration

100 ms
├── Temperature Acquisition
├── Pressure Acquisition
├── VIN Acquisition
├── CAN 0x100 Environment
├── CAN 0x102 Power
└── LED Update

1000 ms
├── Recovery Processing
└── CAN 0x103 Status

500 ms
└── Execution Supervision Window

~2000 ms
└── Watchdog Timeout Target
```

---

# 34. Scheduling Verification

The scheduling implementation shall be verified on the target MCU.

Minimum verification includes:

| Verification | Expected Result |
|---|---|
| Temperature acquisition | ≥ 10 Hz |
| Pressure acquisition | ≥ 10 Hz |
| Acceleration acquisition | ≥ 50 Hz |
| VIN acquisition | ≥ 10 Hz |
| CAN `0x100` | 100 ms nominal period |
| CAN `0x101` | 20 ms nominal period |
| CAN `0x102` | 100 ms nominal period |
| CAN `0x103` | 1000 ms nominal period |
| State evaluation | 10 ms |
| Diagnostic evaluation | 10 ms |
| Slow LED blink | 500 ms ON / 500 ms OFF |
| Fast LED blink | 100 ms ON / 100 ms OFF |
| Failed communication | Does not indefinitely block scheduler |
| Watchdog supervision | Reset occurs when refresh is intentionally suppressed |

Detailed test procedures shall be defined under `06_Verification_Validation`.

---

# 35. Requirement Traceability

The scheduling design primarily supports:

| Requirement Group | Scheduling Contribution |
|---|---|
| SW-INIT-* | Controlled startup execution |
| SW-ACQ-* | Defined sensor acquisition rates |
| SW-CAN-* | Defined cyclic CAN transmission |
| SW-DIAG-* | Periodic fault detection and recovery |
| SW-CTRL-* | Periodic state evaluation |
| SW-WDG-* | Execution supervision and watchdog timing |
| SW-ARCH-* | Cooperative bare-metal execution |
| SW-TEST-* | Measurable timing behavior |

Detailed bidirectional traceability shall be maintained in the project traceability matrix.

---

# 36. Rev A Scheduling Baseline

AeroSense Rev A uses:

```text
Bare-metal cooperative scheduling
Millisecond system time
No RTOS
Finite driver timeouts
Time-triggered cyclic execution
Central execution supervision
Hardware watchdog recovery
```

The design satisfies the baseline acquisition and communication rates while maintaining a simple implementation suitable for incremental hardware bring-up and verification.

---

# 37. Detailed Software Design Completion

With this document, the baseline AeroSense Rev A Detailed Software Design consists of:

```text
Software_Architecture.md
Driver_Design.md
Diagnostics_Design.md
CAN_Interface.md
State_Machine.md
Scheduling_Design.md
```

These documents define the software structure, hardware-driver interfaces, diagnostic behavior, CAN protocol, operational states, and execution timing required for implementation.

The next development activity is:

```text
Firmware Implementation
```

Implementation shall proceed incrementally, beginning with the common firmware structure and basic MCU/status LED execution before integrating individual hardware drivers.

---

## 38. Revision History

| Revision | Version | Description | Status |
|---|---|---|---|
| A | 0.1 | Initial AeroSense Rev A scheduling design | Draft |
