# AeroSense – Software Architecture

**Project:** AeroSense – Embedded Condition Monitoring & Sensor Node  
**Document ID:** ARS-SWA-001  
**Revision:** A  
**Version:** 0.1  
**Status:** Draft  

---

## 1. Purpose

This document defines the software architecture of the AeroSense embedded firmware.

The architecture decomposes the software requirements into software components with defined responsibilities, dependencies, and data flows.

The architecture provides the basis for:

- detailed driver design;
- diagnostic design;
- CAN communication design;
- operational state-machine design;
- scheduling design;
- firmware implementation;
- software integration and verification.

This document defines the structural organization of the firmware. Detailed register-level device behavior, CAN payload definitions, diagnostic fault codes, state-transition conditions, and scheduling periods are defined in the corresponding detailed software design documents.

---

## 2. Applicable Documents

The software architecture is derived from the following AeroSense development artifacts:

- `01_Requirements/System_Requirements.md`
- `01_Requirements/Hardware_Requirements.md`
- `01_Requirements/Software_Requirements.md`
- `01_Requirements/Traceability_Matrix.md`
- `02_System_Architecture/System_Architecture.md`
- `02_System_Architecture/Interfaces/Interface_Definition.md`
- `03_Hardware/Component_Selection/Component_Selection.md`
- `03_Hardware/Detailed_Hardware_Design/Schematics/Schematic_Design.md`

Related detailed software design documents:

- `Driver_Design.md`
- `Diagnostics_Design.md`
- `CAN_Interface.md`
- `State_Machine.md`
- `Scheduling_Design.md`

---

## 3. Architectural Objectives

The AeroSense firmware architecture shall support the following objectives:

1. separation of application logic from hardware-specific implementation;
2. independent integration of sensor functions;
3. centralized measurement handling;
4. centralized diagnostic handling;
5. separation of communication functions from sensor drivers;
6. deterministic periodic execution;
7. continued operation of unaffected functions following an individual sensor failure;
8. testability of individual software components;
9. maintainability and extension of the firmware;
10. traceability from software requirements to implementation and verification.

The architecture is designed for AeroSense Rev A and shall remain sufficiently modular to support later extension of the platform.

---

## 4. Architectural Approach

AeroSense Rev A uses a **bare-metal layered firmware architecture** based on the STM32 Hardware Abstraction Layer (HAL).

No real-time operating system is required for the Rev A baseline.

Periodic software functions are executed using a cooperative time-triggered scheduling concept. Software components shall not intentionally perform indefinite blocking operations.

The architecture is divided into four principal layers:

1. Application Layer
2. Service Layer
3. Device Driver Layer
4. Hardware Abstraction Layer

---

## 5. Software Architecture Overview

```text
+-------------------------------------------------------+
|                  APPLICATION LAYER                    |
|                                                       |
|        AeroSense Application / System Manager         |
|                 Operational State Machine             |
+--------------------------+----------------------------+
                           |
                           v
+-------------------------------------------------------+
|                    SERVICE LAYER                      |
|                                                       |
|  Measurement Manager        Diagnostic Manager        |
|  CAN Message Manager        Configuration Manager     |
|  Identification Service     Execution Supervision     |
+--------------------------+----------------------------+
                           |
                           v
+-------------------------------------------------------+
|                 DEVICE DRIVER LAYER                   |
|                                                       |
|  STTS22H Driver          LPS22HH Driver               |
|  LIS2DW12 Driver         VIN Monitor Driver           |
|  CAN Driver              USB Interface                |
|  Status LED Driver       Watchdog Interface           |
+--------------------------+----------------------------+
                           |
                           v
+-------------------------------------------------------+
|              HARDWARE ABSTRACTION LAYER               |
|                                                       |
|     STM32 HAL / MCU Peripheral Configuration          |
|                                                       |
| I2C2 | SPI1 | ADC1 | FDCAN1 | USB | GPIO | IWDG/SWD |
+--------------------------+----------------------------+
                           |
                           v
+-------------------------------------------------------+
|                    PHYSICAL HARDWARE                  |
|                                                       |
| STM32G431CBT6 | Sensors | TCAN332 | USB | VIN Sense  |
+-------------------------------------------------------+
```

Dependencies shall primarily point from higher architectural layers toward lower layers.

Device drivers shall not depend on application-level components.

---

# 6. Application Layer

## 6.1 AeroSense Application

The AeroSense Application represents the highest-level software control function.

It coordinates system startup, periodic application execution, system-state evaluation, and interaction between service-layer components.

The application shall not directly access sensor registers or MCU peripheral registers.

### Responsibilities

The AeroSense Application is responsible for:

- starting application-level initialization;
- coordinating startup self-test activities;
- executing the main application cycle;
- invoking scheduled software functions;
- evaluating the operational state;
- coordinating degraded operation;
- coordinating fault behavior.

---

## 6.2 System Manager

The System Manager coordinates the global operational behavior of AeroSense.

It uses diagnostic and initialization information to determine the current operational state.

The System Manager shall support:

```text
INITIALIZATION
SELF_TEST
NORMAL_OPERATION
DEGRADED_OPERATION
FAULT
```

Detailed transition conditions and state-specific behavior are defined in `State_Machine.md`.

`POWER_OFF` is a system-level condition and is not implemented as an executing firmware state.

---

# 7. Service Layer

## 7.1 Measurement Manager

The Measurement Manager provides a common application-level representation of acquired measurement data.

### Responsibilities

The Measurement Manager shall:

- receive measurements from the sensor and supply-monitoring functions;
- maintain the latest measurement values;
- maintain measurement validity information;
- provide measurement data to communication functions;
- prevent invalid measurements from being treated as valid current data.

The Measurement Manager shall maintain at least:

```text
Temperature
Pressure
Acceleration X
Acceleration Y
Acceleration Z
Supply Voltage
```

A conceptual measurement representation is:

```text
Measurement
|
+-- value
+-- validity
+-- timestamp / age information
```

The exact C data structures shall be defined during implementation.

---

## 7.2 Diagnostic Manager

The Diagnostic Manager provides centralized handling of detected system faults.

### Responsibilities

The Diagnostic Manager shall:

- receive fault indications from software components;
- maintain active diagnostic conditions;
- provide diagnostic status to the System Manager;
- provide diagnostic information to communication functions;
- distinguish faults that permit degraded operation from faults requiring FAULT operation.

Examples of diagnostic sources include:

- temperature sensor communication failure;
- pressure sensor communication failure;
- accelerometer communication failure;
- supply undervoltage;
- CAN peripheral error;
- startup initialization failure;
- execution supervision failure.

Detailed diagnostic identifiers, fault classification, detection criteria, and recovery behavior are defined in `Diagnostics_Design.md`.

---

## 7.3 CAN Message Manager

The CAN Message Manager is responsible for the application-level representation of CAN communication.

### Responsibilities

The CAN Message Manager shall:

- obtain measurement data from the Measurement Manager;
- obtain diagnostic information from the Diagnostic Manager;
- encode defined CAN messages;
- request CAN transmission through the CAN driver;
- decode supported received CAN messages;
- reject unsupported application-level CAN messages.

The CAN Message Manager shall not directly access FDCAN peripheral registers.

CAN identifiers, payload definitions, scaling, byte order, validity encoding, and transmission periods are defined in `CAN_Interface.md`.

---

## 7.4 Configuration Manager

The Configuration Manager provides application-level handling of configuration communication with an external host.

### Responsibilities

The Configuration Manager shall:

- receive supported configuration requests through the USB communication function;
- validate received configuration values;
- reject invalid or unsupported commands;
- provide requested system information;
- apply supported configuration changes.

The configuration protocol and configurable parameter set shall be defined during detailed USB/configuration design.

---

## 7.5 Identification Service

The Identification Service provides device and firmware identification information.

The service shall provide available information including:

- firmware version;
- hardware revision;
- device identifier.

Identification information may be accessed by communication services.

---

## 7.6 Execution Supervision

The Execution Supervision function monitors correct cyclic execution of required firmware functions.

It coordinates servicing of the hardware watchdog.

The watchdog shall only be serviced when the required execution-supervision conditions are satisfied.

Detailed watchdog timing and supervision criteria are defined in `Scheduling_Design.md` and `Diagnostics_Design.md`.

---

# 8. Device Driver Layer

The Device Driver Layer isolates hardware-device-specific communication and control from application and service logic.

Each hardware device shall be represented by a dedicated driver or hardware service.

---

## 8.1 STTS22H Temperature Driver

The STTS22H driver provides access to the temperature sensing hardware.

### Responsibilities

The driver shall provide functions for:

- sensor initialization;
- device communication verification;
- temperature acquisition;
- raw-data conversion;
- communication-error reporting.

The driver communicates with the sensor through the configured I2C hardware interface.

---

## 8.2 LPS22HH Pressure Driver

The LPS22HH driver provides access to the atmospheric pressure sensing hardware.

### Responsibilities

The driver shall provide functions for:

- sensor initialization;
- device communication verification;
- pressure acquisition;
- raw-data conversion;
- communication-error reporting.

The driver communicates with the sensor through the configured I2C hardware interface.

---

## 8.3 LIS2DW12 Acceleration Driver

The LIS2DW12 driver provides access to the three-axis acceleration sensing hardware.

### Responsibilities

The driver shall provide functions for:

- sensor initialization;
- device communication verification;
- three-axis acceleration acquisition;
- raw-data conversion;
- communication-error reporting.

The driver communicates with the sensor through the configured SPI hardware interface.

---

## 8.4 VIN Monitor Driver

The VIN Monitor Driver provides access to the analog supply-voltage monitoring function.

### Responsibilities

The driver shall:

- trigger or obtain ADC conversion data;
- convert ADC data into the corresponding monitored input voltage;
- provide the calculated voltage to the Measurement Manager;
- support detection of invalid ADC acquisition.

The detailed electrical scaling is defined by the hardware design and shall be reflected in the driver implementation.

---

## 8.5 CAN Driver

The CAN Driver provides the software interface to the MCU CAN controller.

### Responsibilities

The CAN Driver shall:

- initialize CAN operation;
- transmit CAN frames;
- receive CAN frames;
- report controller errors;
- provide received frames to the CAN Message Manager.

The CAN Driver shall not contain AeroSense measurement-payload encoding logic.

---

## 8.6 USB Interface

The USB Interface provides access to the MCU USB peripheral and associated communication stack.

### Responsibilities

The USB Interface shall:

- initialize USB communication;
- receive host data;
- transmit host data;
- provide received configuration data to the Configuration Manager;
- report relevant communication status.

Application-specific configuration processing shall remain outside the low-level USB interface.

---

## 8.7 Status LED Driver

The Status LED Driver controls the visual status indicator.

The driver shall provide an abstraction for controlling the status LED without exposing MCU GPIO details to application-level software.

Status indication behavior may depend on the current operational state.

---

## 8.8 Watchdog Interface

The Watchdog Interface provides controlled access to the MCU hardware watchdog.

The interface shall provide operations for:

- watchdog initialization;
- watchdog servicing.

The decision to service the watchdog belongs to the Execution Supervision function and shall not be made independently by the watchdog driver.

---

# 9. Hardware Abstraction Layer

The Hardware Abstraction Layer consists primarily of STM32Cube-generated peripheral initialization and STM32 HAL functionality.

The principal MCU resources used by AeroSense Rev A are:

| Function | MCU Resource |
|---|---|
| Temperature sensor communication | I2C2 |
| Pressure sensor communication | I2C2 |
| Accelerometer communication | SPI1 |
| Supply-voltage acquisition | ADC1 |
| CAN communication | FDCAN1 |
| USB communication | USB peripheral |
| Status indication | GPIO |
| Debug / programming | SWD |
| Execution supervision | Hardware watchdog |

The detailed MCU pin and signal assignments are defined in the AeroSense Interface Definition and hardware design.

Application components shall not directly manipulate MCU peripheral registers unless explicitly justified by the detailed design.

---

# 10. Software Data Flow

The principal measurement-data flow is:

```text
Physical Quantity
      |
      v
Sensor Hardware
      |
      v
Device Driver
      |
      v
Measurement Manager
      |
      +--------------------+
      |                    |
      v                    v
CAN Message Manager   Diagnostic Manager
      |                    |
      v                    v
CAN Driver           System Manager
      |                    |
      v                    v
External CAN       Operational State
Network
```

Supply-voltage monitoring follows the corresponding path:

```text
VIN
 |
 v
Voltage Divider
 |
 v
ADC1
 |
 v
VIN Monitor Driver
 |
 v
Measurement Manager
 |
 +-------> Diagnostic Manager
 |
 +-------> CAN Message Manager
```

Diagnostic information follows:

```text
Driver / Service Fault
        |
        v
Diagnostic Manager
        |
        +------------------+
        |                  |
        v                  v
System Manager       CAN Message Manager
        |                  |
        v                  v
Operational State     External Reporting
```

---

# 11. Component Dependency Rules

To maintain architectural separation, the following dependency rules apply:

1. Application components may access service-layer interfaces.
2. Service components may access device-driver interfaces.
3. Device drivers may access STM32 HAL functions and assigned MCU peripherals.
4. Device drivers shall not access application-state logic.
5. Sensor drivers shall not directly encode CAN application messages.
6. CAN communication shall not directly control sensor hardware.
7. Diagnostic decisions shall be centralized in the Diagnostic Manager where practical.
8. Hardware-specific constants shall remain within the appropriate driver or hardware-interface implementation where practical.
9. Automatically generated STM32 initialization code shall be separated from AeroSense application logic where supported by the toolchain.

---

# 12. Execution Model

AeroSense Rev A uses a cooperative cyclic execution model.

After MCU startup, the firmware performs initialization and self-test activities before entering cyclic operation.

Conceptually:

```text
Reset
  |
  v
MCU / HAL Initialization
  |
  v
AeroSense Initialization
  |
  v
Self Test
  |
  v
+-----------------------------+
|       Main Superloop        |
|                             |
|  Update scheduler           |
|  Acquire due measurements   |
|  Process measurements       |
|  Update diagnostics         |
|  Process communication      |
|  Update system state        |
|  Update status indication   |
|  Supervise execution        |
+-------------+---------------+
              |
              +---- repeat
```

Periodic activities shall be triggered according to their defined scheduling periods.

Long-running or indefinite blocking operations shall be avoided.

Detailed task periods, execution ordering, timing behavior, and timeout values are defined in `Scheduling_Design.md`.

---

# 13. Initialization Sequence

The preliminary software initialization sequence is:

```text
Processor Reset
      |
      v
HAL Initialization
      |
      v
System Clock Configuration
      |
      v
MCU Peripheral Initialization
      |
      v
AeroSense Software Initialization
      |
      v
Sensor Initialization
      |
      v
Communication Initialization
      |
      v
Startup Self-Test
      |
      v
Operational State Evaluation
      |
      +------ success ------> NORMAL_OPERATION
      |
      +------ partial ------> DEGRADED_OPERATION
      |
      +------ critical -----> FAULT
```

The exact state-transition criteria are defined in `State_Machine.md`.

---

# 14. Failure Containment

The software architecture shall support continued operation of unaffected monitoring functions following failure of an individual sensor where technically possible.

Example:

```text
Temperature Sensor Failure
          |
          v
Temperature Driver
detects communication failure
          |
          v
Diagnostic Manager
          |
          +---- Temperature data INVALID
          |
          +---- Diagnostic fault ACTIVE
          |
          v
System Manager
          |
          v
DEGRADED_OPERATION

Pressure acquisition       continues
Acceleration acquisition   continues
VIN monitoring             continues
CAN diagnostics            continues
```

A single recoverable sensor communication failure shall therefore not intentionally terminate unrelated measurement functions.

---

# 15. Proposed Firmware Module Structure

The detailed implementation is expected to follow a modular structure similar to:

```text
AeroSense_Firmware/
|
+-- Core/
|   +-- Inc/
|   +-- Src/
|
+-- Drivers/
|   +-- STM32 HAL
|
+-- AeroSense/
    |
    +-- App/
    |   +-- aerosense_app
    |   +-- system_manager
    |
    +-- Services/
    |   +-- measurement_manager
    |   +-- diagnostic_manager
    |   +-- can_message_manager
    |   +-- configuration_manager
    |   +-- identification
    |   +-- execution_supervision
    |
    +-- DeviceDrivers/
        +-- stts22h
        +-- lps22hh
        +-- lis2dw12
        +-- vin_monitor
        +-- can_driver
        +-- usb_interface
        +-- status_led
        +-- watchdog
```

The exact directory and file structure may be adapted to the generated STM32Cube/CMake project provided that the architectural separation defined in this document is preserved.

---

# 16. Requirement Allocation

The following table provides the high-level allocation of existing software requirements to architectural components.

| Requirement Group | Primary Architectural Component |
|---|---|
| SW-INIT-* | AeroSense Application / System Manager |
| SW-ACQ-* | Sensor Drivers / VIN Monitor / Measurement Manager |
| SW-PROC-* | Measurement Manager |
| SW-CAN-* | CAN Message Manager / CAN Driver |
| SW-CFG-* | Configuration Manager / USB Interface |
| SW-DIAG-* | Diagnostic Manager |
| SW-CTRL-* | System Manager / State Machine |
| SW-WDG-* | Execution Supervision / Watchdog Interface |
| SW-ID-* | Identification Service |
| SW-ARCH-* | Overall Software Architecture |
| SW-TEST-* | All relevant components |

Detailed requirement-to-design and requirement-to-test traceability shall be maintained as the software design and verification artifacts are completed.

---

# 17. Detailed Design Allocation

The software architecture defined in this document is refined by the following detailed design artifacts:

| Design Artifact | Content |
|---|---|
| `Driver_Design.md` | Driver APIs, device initialization, sensor access and hardware interaction |
| `Diagnostics_Design.md` | Fault detection, fault classification, diagnostic status and recovery |
| `CAN_Interface.md` | CAN IDs, payloads, scaling, validity and transmission behavior |
| `State_Machine.md` | States, transitions, entry conditions and state behavior |
| `Scheduling_Design.md` | Execution periods, scheduling, timeouts and watchdog supervision |

These documents shall define implementation-level behavior without changing the architectural responsibilities established in this document.

---

# 18. Architectural Constraints

The AeroSense Rev A software architecture is subject to the following constraints:

- target MCU: STM32G431CBT6;
- firmware language: C;
- STM32 HAL is used for MCU hardware abstraction;
- firmware shall be buildable using the project CMake-based build environment;
- sensor acquisition shall support the rates defined by the Software Requirements Specification;
- CAN nominal bit rate is 500 kbit/s;
- SWD is the primary programming and debugging interface;
- the baseline architecture does not require an RTOS;
- software shall support incremental integration and hardware bring-up.

---

# 19. Architecture Verification

The software architecture shall be verified primarily by inspection and subsequently confirmed through implementation and integration testing.

Architecture verification shall confirm that:

- required software functions are allocated to defined components;
- hardware-specific access is separated from application logic;
- sensor drivers are independently integrable;
- diagnostic handling is centralized;
- CAN application-message handling is separated from low-level CAN access;
- operational state management is separated from device drivers;
- individual sensor failure can be contained where required;
- the architecture supports the acquisition and communication timing requirements.

---

# 20. Architecture Status

This document defines the AeroSense Rev A baseline software architecture.

The following details are intentionally defined in subsequent detailed design documents:

- device-driver APIs;
- sensor register configuration;
- sensor communication timeout values;
- diagnostic fault identifiers;
- diagnostic recovery strategy;
- CAN message identifiers and payloads;
- USB configuration protocol;
- detailed state-transition conditions;
- scheduler periods and execution ordering;
- watchdog timeout and supervision criteria.

The next detailed software design activity is the definition of the device-driver design in `Driver_Design.md`.

---

## 21. Revision History

| Revision | Version | Description | Status |
|---|---|---|---|
| A | 0.1 | Initial AeroSense Rev A software architecture | Draft |
