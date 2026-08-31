# AeroSense – Stakeholder Needs

**Project:** AeroSense – Embedded Condition Monitoring & Sensor Node  
**Document:** Stakeholder Needs  
**Version:** 1.0  
**Status:** Draft

## 1. Purpose

This document defines the stakeholder needs for the AeroSense embedded condition monitoring system.

AeroSense is intended as a development and engineering platform for acquiring environmental, mechanical and electrical condition data and providing measurement and diagnostic information to external systems.

The stakeholder needs defined in this document provide the basis for the derivation of the system requirements.

---

## 2. User / Operator Needs

| ID | Stakeholder Need | Rationale |
|---|---|---|
| UN-001 | The user needs to monitor environmental conditions. | Temperature and ambient pressure shall be observable. |
| UN-002 | The user needs to monitor mechanical motion and vibration. | Acceleration and vibration information is required for condition monitoring. |
| UN-003 | The user needs to monitor the electrical supply condition of the device. | The user shall be able to identify abnormal supply conditions. |
| UN-004 | The user needs to receive measurement data on an external system. | Measurement data shall be available outside the device. |
| UN-005 | The user needs to identify the operational status and detected faults of the device. | The user shall be able to determine whether AeroSense is operating correctly. |
| UN-006 | The user needs to configure the device without opening the enclosure. | Configuration shall be possible while the device is assembled. |

---

## 3. System Integrator Needs

| ID | Stakeholder Need | Rationale |
|---|---|---|
| SI-001 | The system integrator needs a standardized communication interface for exchanging measurement and diagnostic data. | AeroSense must be capable of integration with external systems. |
| SI-002 | The system integrator needs clearly defined communication messages and data formats. | External systems must be able to interpret AeroSense data correctly. |
| SI-003 | The system integrator needs clearly defined electrical and physical interfaces. | Power and communication interfaces must be unambiguous. |
| SI-004 | The system integrator needs predictable and periodic transmission of measurement data. | External systems require predictable data availability. |
| SI-005 | The system integrator needs to determine whether received measurement data is valid. | Invalid or faulty sensor data must be identifiable. |

---

## 4. Developer / Engineering Needs

| ID | Stakeholder Need | Rationale |
|---|---|---|
| DEV-001 | The developer needs access to the embedded system for programming and debugging. | Firmware development and troubleshooting require access to the embedded controller. |
| DEV-002 | The developer needs a modular hardware and software architecture. | Individual functions shall be independently developed, modified and tested. |
| DEV-003 | The developer needs diagnostic information for troubleshooting hardware and software failures. | Failures must be identifiable during development and integration. |
| DEV-004 | The developer needs the system design to support incremental hardware/software integration. | Components shall be integrated and tested step by step. |
| DEV-005 | The developer needs traceability between stakeholder needs, system requirements, implementation and verification. | Development decisions and verification evidence shall remain traceable. |

---

## 5. Test / Verification / Maintenance Needs

| ID | Stakeholder Need | Rationale |
|---|---|---|
| TV-001 | The test engineer needs to verify the correct operation of each major system function. | System requirements must be objectively verifiable. |
| TV-002 | The test engineer needs access to measurement and diagnostic information during testing. | Test results require observable system information. |
| TV-003 | The test engineer needs to reproduce defined fault conditions. | Fault detection and system reactions must be testable. |
| TV-004 | The maintenance technician needs to identify sensor, communication and power-related failures. | Fault localization shall be possible without extensive disassembly. |
| TV-005 | The maintenance technician needs to determine the firmware and hardware configuration of the device. | Hardware and software versions must be traceable. |
| TV-006 | The test engineer needs repeatable test procedures and documented acceptance criteria. | Verification results shall be reproducible and objectively evaluated. |

---

## 6. Manufacturing / Product Needs

| ID | Stakeholder Need | Rationale |
|---|---|---|
| MFG-001 | The manufacturer needs the device to be reproducibly manufacturable and assemblable. | Multiple devices shall be manufacturable with consistent configuration. |
| MFG-002 | The manufacturer needs complete manufacturing and assembly documentation. | Production requires clearly defined manufacturing information. |
| MFG-003 | The manufacturer needs a defined method for programming the firmware during production. | Firmware installation shall be repeatable for every manufactured device. |
| MFG-004 | The manufacturer needs a method to verify each assembled device before release. | Manufacturing defects shall be detected before the device is released. |
| MFG-005 | The manufacturer needs each device to be uniquely identifiable. | Individual devices and their configurations shall be traceable. |
| MFG-006 | The manufacturer needs the electronics to be mechanically protected during normal handling and operation. | The electronic components require protection against normal handling and environmental influences. |

---

## 7. Traceability

The stakeholder needs defined in this document will be used as the source for the derivation of system requirements.

Traceability will follow the development chain:

Stakeholder Need → System Requirement → HW/SW Requirement → Design → Implementation → Verification
