# AeroSense – System Architecture

**Project:** AeroSense – Embedded Condition Monitoring & Sensor Node  
**Document:** System Architecture Description  
**Version:** 0.1  
**Status:** Draft  

---

## 1. Purpose

This document defines the system architecture of the AeroSense Embedded Condition Monitoring & Sensor Node.

The architecture is derived from the system requirements defined in `01_Requirements/System_Requirements.md`.

The purpose of the system architecture is to:

- define the AeroSense system boundary;
- identify the external systems and environments interacting with AeroSense;
- define the external system interfaces;
- decompose AeroSense into major functional elements;
- define the logical flow of information through the system;
- allocate system functions to hardware and software;
- provide the basis for deriving hardware and software requirements.

---

## 2. System Overview

AeroSense is an embedded condition monitoring and sensor node intended to acquire environmental, mechanical, and electrical condition data.

The system provides the following primary monitoring functions:

- ambient temperature measurement;
- ambient atmospheric pressure measurement;
- three-axis acceleration measurement;
- supply-voltage monitoring.

AeroSense processes the acquired measurement data and provides measurement and diagnostic information to external systems.

The system provides a CAN communication interface for measurement and diagnostic data exchange.

AeroSense also provides an interface for configuration and development access as well as an interface for firmware programming and debugging.

---

## 3. System Boundary

The AeroSense system includes the functions required for:

- sensing;
- measurement acquisition;
- measurement processing;
- diagnostics;
- system control;
- external communication;
- configuration;
- programming and debugging;
- internal power management.

The following elements are considered external to the AeroSense system:

- external DC power source;
- physical environment;
- monitored mechanical environment;
- external CAN network;
- host computer;
- programming and debugging equipment.

---

## 4. System Context

### 4.1 External Elements

AeroSense interacts with the following external elements:

| External Element | Interaction with AeroSense |
|---|---|
| Physical Environment | Provides ambient temperature and atmospheric pressure to be measured |
| Mechanical Environment | Provides mechanical motion and vibration to be measured |
| DC Power Source | Provides electrical power to AeroSense |
| CAN Network | Receives measurement and diagnostic data from AeroSense |
| Host Computer | Provides configuration and development access |
| Programming / Debugging Equipment | Provides firmware programming and debugging access |

### 4.2 System Context Diagram

```text
                    Physical Environment
                  Temperature / Pressure
                           |
                           v
                +-----------------------+
                |                       |
Mechanical ---->|                       |-----> External CAN Network
Environment     |       AeroSense       |
Motion /        |                       |
Vibration       |                       |-----> Host Computer
                |                       |
                +----------+------------+
                           ^
                           |
                    External DC Power

                           ^
                           |
                   Programming /
                   Debugging Equipment
```

---

## 5. External Interfaces

The following external interfaces are identified at system level.

| Interface ID | External Element | Purpose |
|---|---|---|
| IF-EXT-001 | Physical Environment | Acquisition of ambient temperature and atmospheric pressure |
| IF-EXT-002 | Mechanical Environment | Acquisition of acceleration and vibration-related information |
| IF-EXT-003 | DC Power Source | Electrical power supply |
| IF-EXT-004 | CAN Network | Exchange of measurement and diagnostic information |
| IF-EXT-005 | Host Computer | Configuration and development communication |
| IF-EXT-006 | Programming / Debugging Equipment | Firmware programming and debugging |

Detailed electrical characteristics, protocols, connector definitions, and signal assignments will be defined during hardware and software requirement derivation and detailed design.

---

## 6. Functional Architecture

AeroSense is decomposed into the following major functional elements:

1. Environmental Sensing
2. Motion Sensing
3. Supply Monitoring
4. Measurement Acquisition
5. Measurement Processing
6. Diagnostics
7. System Control
8. CAN Communication
9. Configuration Communication
10. Programming and Debugging
11. Power Management

### 6.1 Functional Architecture Diagram

```text
              Physical Environment
             Temperature / Pressure
                       |
                       v
            +-----------------------+
            | Environmental Sensing |
            +-----------+-----------+
                        |
                        |
Mechanical              |              Supply
Environment             |              Voltage
     |                  |                 |
     v                  v                 v
+------------+   +-------------+   +-------------------+
| Motion     |-->| Measurement |<--| Supply Monitoring |
| Sensing    |   | Acquisition |   +-------------------+
+------------+   +------+------+
                       |
                       v
                +------+------+
                | Measurement |
                | Processing  |
                +------+------+
                       |
             +---------+---------+
             |                   |
             v                   v
      +-------------+      +-------------+
      | Diagnostics |<---->|   System    |
      |             |      |   Control   |
      +------+------+      +------+------+
             |                    |
             +---------+----------+
                       |
             +---------v----------+
             |                    |
             |   Communication    |
             |                    |
             +-----+---------+----+
                   |         |
                  CAN       USB
                   |         |
                   v         v
             CAN Network   Host PC
```

---

## 7. Functional Elements

### 7.1 Environmental Sensing

The Environmental Sensing function provides environmental measurement information to the AeroSense system.

Responsibilities:

- sense ambient temperature;
- sense ambient atmospheric pressure;
- provide environmental measurement information to the Measurement Acquisition function.

Related system requirements:

- SYS-FUN-001
- SYS-FUN-002
- SYS-PERF-001
- SYS-PERF-002

---

### 7.2 Motion Sensing

The Motion Sensing function provides mechanical motion information to the AeroSense system.

Responsibilities:

- sense acceleration along three orthogonal axes;
- provide acceleration information to the Measurement Acquisition function.

Related system requirements:

- SYS-FUN-003
- SYS-PERF-003

---

### 7.3 Supply Monitoring

The Supply Monitoring function monitors the electrical supply condition of AeroSense.

Responsibilities:

- measure the system supply voltage;
- provide supply-voltage measurement information;
- provide information required for detection of abnormal supply conditions.

Related system requirements:

- SYS-FUN-004
- SYS-PERF-004
- SYS-DIAG-003

---

### 7.4 Measurement Acquisition

The Measurement Acquisition function coordinates the acquisition of measurement information from the sensing functions.

Responsibilities:

- acquire temperature measurement data;
- acquire atmospheric pressure measurement data;
- acquire three-axis acceleration measurement data;
- acquire supply-voltage measurement data;
- provide acquired data to the Measurement Processing function.

Related system requirements:

- SYS-FUN-001
- SYS-FUN-002
- SYS-FUN-003
- SYS-FUN-004
- SYS-PERF-001
- SYS-PERF-002
- SYS-PERF-003
- SYS-PERF-004

---

### 7.5 Measurement Processing

The Measurement Processing function processes acquired measurement data for internal use and external communication.

Responsibilities:

- process acquired measurement data;
- determine measurement validity;
- prepare measurement information for communication;
- provide processed measurement information to other system functions.

Related system requirements:

- SYS-FUN-005
- SYS-FUN-009
- SYS-PERF-005

---

### 7.6 Diagnostics

The Diagnostics function monitors the health and operational condition of AeroSense.

Responsibilities:

- detect loss of communication with digital sensors;
- detect abnormal supply conditions;
- identify invalid measurement data;
- generate diagnostic information;
- support initialization checks;
- provide detected fault information;
- support degraded operation following individual sensor failures.

Related system requirements:

- SYS-FUN-006
- SYS-FUN-007
- SYS-FUN-009
- SYS-DIAG-001
- SYS-DIAG-002
- SYS-DIAG-003
- SYS-DIAG-004
- SYS-DIAG-005
- SYS-DIAG-007

---

### 7.7 System Control

The System Control function coordinates the overall operation of AeroSense.

Responsibilities:

- coordinate system initialization;
- coordinate system self-test;
- manage normal system operation;
- coordinate degraded operation following detected failures;
- manage system recovery mechanisms;
- supervise software execution;
- coordinate system state transitions.

Related system requirements:

- SYS-FUN-006
- SYS-DIAG-005
- SYS-DIAG-006
- SYS-DIAG-007

---

### 7.8 CAN Communication

The CAN Communication function provides communication between AeroSense and an external CAN network.

Responsibilities:

- transmit measurement information;
- transmit diagnostic information;
- implement the defined CAN message structure;
- provide periodic transmission of defined CAN messages.

Related system requirements:

- SYS-FUN-005
- SYS-PERF-005
- SYS-IF-001
- SYS-IF-002
- SYS-IF-004
- SYS-DIAG-004

---

### 7.9 Configuration Communication

The Configuration Communication function provides communication between AeroSense and an external host computer.

Responsibilities:

- provide access to system configuration;
- provide development communication;
- provide system identification information.

Related system requirements:

- SYS-FUN-008
- SYS-FUN-010
- SYS-IF-003

---

### 7.10 Programming and Debugging

The Programming and Debugging function supports firmware development, firmware installation, and troubleshooting.

Responsibilities:

- provide firmware programming access;
- provide debugging access;
- support firmware installation during development;
- support firmware programming after PCB assembly.

Related system requirements:

- SYS-DEV-001
- SYS-MFG-002

---

### 7.11 Power Management

The Power Management function provides the electrical power required by the internal AeroSense functions.

Responsibilities:

- receive external DC power;
- provide the required internal electrical supply;
- distribute power to internal system elements;
- support supply-voltage monitoring.

Related system requirements:

- SYS-FUN-004
- SYS-IF-006
- SYS-DIAG-003

---

## 8. Logical Data Flow

### 8.1 Measurement Data Flow

The primary measurement data flow is:

```text
Physical Quantity
       |
       v
     Sensing
       |
       v
Measurement Acquisition
       |
       v
Measurement Processing
       |
       v
Communication
       |
       v
External System
```

---

### 8.2 Diagnostic Data Flow

The diagnostic data flow is:

```text
Sensor / System Status
          |
          v
      Diagnostics
          |
          v
     System Control
          |
          v
     Communication
          |
          v
    External System
```

---

### 8.3 Configuration Data Flow

The configuration data flow is:

```text
Host Computer
      |
      v
Configuration Communication
      |
      v
System Control / Configuration
```

---

## 9. Hardware / Software Partitioning

The system functions are preliminarily allocated to hardware and software.

This allocation provides the basis for the later derivation of Hardware Requirements and Software Requirements.

| System Function | Hardware Responsibility | Software Responsibility | Allocation |
|---|---|---|---|
| Environmental Sensing | Physical sensing and electrical interface | Sensor control and data acquisition | HW + SW |
| Motion Sensing | Physical acceleration sensing and electrical interface | Sensor control and data acquisition | HW + SW |
| Supply Monitoring | Voltage measurement circuitry | Measurement evaluation and fault detection | HW + SW |
| Measurement Acquisition | Processing hardware and peripheral interfaces | Acquisition timing and sensor drivers | HW + SW |
| Measurement Processing | Processing resources | Data processing and validity handling | Primarily SW |
| Diagnostics | Hardware monitoring support where required | Fault detection, status management and diagnostic logic | HW + SW |
| System Control | Processing hardware and watchdog support | State management, initialization and execution control | HW + SW |
| CAN Communication | CAN-capable controller and physical communication interface | CAN driver, message handling and communication control | HW + SW |
| Configuration Communication | External communication hardware | Configuration communication and command handling | HW + SW |
| Programming / Debugging | Physical programming/debug interface | Firmware and development support | HW + SW |
| Power Management | Power conversion, distribution and protection circuitry | Supply monitoring support where required | Primarily HW |

The allocation is preliminary and may be refined during hardware and software requirement derivation.

---

## 10. Operational States

The preliminary AeroSense operational states are:

- POWER_OFF
- INITIALIZATION
- SELF_TEST
- NORMAL_OPERATION
- DEGRADED_OPERATION
- FAULT

### 10.1 POWER_OFF

AeroSense is not electrically powered.

---

### 10.2 INITIALIZATION

AeroSense initializes the required hardware, software, sensing functions, and communication interfaces.

---

### 10.3 SELF_TEST

AeroSense evaluates the availability and operational status of required system functions.

---

### 10.4 NORMAL_OPERATION

All required monitoring, processing, diagnostic, and communication functions are operating normally.

---

### 10.5 DEGRADED_OPERATION

One or more non-critical functions are unavailable while unaffected monitoring and communication functions remain operational.

---

### 10.6 FAULT

A failure prevents AeroSense from providing its intended minimum functionality.

---

### 10.7 State Transition Concept

```text
                     Power Applied
                          |
                          v
                    INITIALIZATION
                          |
                          v
                      SELF_TEST
                       /     \
                      /       \
                   PASS      Fault
                    |          |
                    v          v
             NORMAL_OPERATION  DEGRADED_OPERATION
                    |          |
                    |          |
                    +----+-----+
                         |
                  Critical Fault
                         |
                         v
                       FAULT
```

Detailed transition conditions and fault reactions will be defined during software requirement derivation.

---

## 11. Preliminary Physical Architecture

The preliminary physical architecture identifies the main physical elements required to implement the system functions.

```text
+-----------------------------------------------------------+
|                         AeroSense                         |
|                                                           |
|  +------------------+                                     |
|  | Environmental    |                                     |
|  | Sensing          |---------+                           |
|  +------------------+         |                           |
|                               |                           |
|  +------------------+         |     +------------------+  |
|  | Motion Sensing   |---------+---->|                  |  |
|  +------------------+         |     |    Processing    |  |
|                               |     |       Unit       |  |
|  +------------------+         |     |                  |  |
|  | Supply           |---------+---->|                  |  |
|  | Monitoring       |               +----+--------+----+  |
|  +------------------+                    |        |       |
|                                          |        |       |
|                             +------------v--+  +--v-----+ |
|                             | CAN Interface |  | USB /  | |
|                             |               |  | Config | |
|                             +-------+-------+  +----+----+ |
|                                     |               |      |
|  +------------------+               |               |      |
|  | Power Management |               |               |      |
|  +------------------+               |               |      |
|                                                           |
+-------------------------------------|---------------|-----+
                                      |               |
                                      v               v
                              External CAN         Host PC
                                 Network
```

The preliminary architecture does not prescribe specific implementation components.

At this stage, no specific:

- microcontroller;
- temperature sensor;
- pressure sensor;
- acceleration sensor;
- CAN transceiver;
- voltage regulator;
- connector;
- PCB technology

is selected.

Component selection will be performed after the hardware and software requirements have been derived.

---

## 12. Architecture-to-Requirements Traceability

The following table identifies the main relationship between the architecture elements and the existing system requirements.

| Architecture Element | Related System Requirements |
|---|---|
| Environmental Sensing | SYS-FUN-001, SYS-FUN-002, SYS-PERF-001, SYS-PERF-002 |
| Motion Sensing | SYS-FUN-003, SYS-PERF-003 |
| Supply Monitoring | SYS-FUN-004, SYS-PERF-004, SYS-DIAG-003 |
| Measurement Acquisition | SYS-FUN-001, SYS-FUN-002, SYS-FUN-003, SYS-FUN-004, SYS-PERF-001, SYS-PERF-002, SYS-PERF-003, SYS-PERF-004 |
| Measurement Processing | SYS-FUN-005, SYS-FUN-009, SYS-PERF-005 |
| Diagnostics | SYS-FUN-006, SYS-FUN-007, SYS-FUN-009, SYS-DIAG-001, SYS-DIAG-002, SYS-DIAG-003, SYS-DIAG-004, SYS-DIAG-005, SYS-DIAG-007 |
| System Control | SYS-FUN-006, SYS-DIAG-005, SYS-DIAG-006, SYS-DIAG-007 |
| CAN Communication | SYS-FUN-005, SYS-PERF-005, SYS-IF-001, SYS-IF-002, SYS-IF-004, SYS-DIAG-004 |
| Configuration Communication | SYS-FUN-008, SYS-FUN-010, SYS-IF-003 |
| Programming / Debugging | SYS-DEV-001, SYS-MFG-002 |
| Power Management | SYS-FUN-004, SYS-IF-006, SYS-DIAG-003 |

---

## 13. Architecture Constraints and Open Decisions

The following implementation decisions remain open at this development stage:

- microcontroller selection;
- environmental sensor selection;
- acceleration sensor selection;
- CAN transceiver selection;
- power-supply architecture;
- input-voltage range;
- connector selection;
- detailed USB implementation;
- PCB dimensions;
- enclosure dimensions;
- detailed CAN message definition;
- detailed diagnostic behavior.

These decisions will be made based on the derived hardware and software requirements.

---

## 14. Architecture Status

This document defines the preliminary system architecture for AeroSense Rev A.

The architecture provides the basis for the next development activities:

1. derivation of hardware requirements;
2. derivation of software requirements;
3. detailed interface definition;
4. component selection;
5. detailed hardware design;
6. detailed software design;
7. implementation and integration;
8. system verification.

Changes to the architecture that affect existing requirements will be reviewed and reflected in the requirements traceability matrix.
