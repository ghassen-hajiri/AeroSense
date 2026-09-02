# AeroSense – Hardware Component Selection

**Project:** AeroSense – Embedded Condition Monitoring & Sensor Node  
**Document ID:** ARS-HCS-001  
**Version:** 0.1  
**Status:** Draft  

---

## 1. Scope

This document defines the selection of the main electronic hardware components for the AeroSense Rev A design.

The component selection is derived from the system requirements, hardware requirements, system architecture, and interface definition.

The selected components form the basis for the detailed electrical design, schematic capture, PCB design, firmware driver implementation, and hardware verification.

---

## 2. Applicable Documents

- `01_Requirements/System_Requirements.md`
- `01_Requirements/Hardware_Requirements.md`
- `01_Requirements/Software_Requirements.md`
- `01_Requirements/Traceability_Matrix.md`
- `02_System_Architecture/System_Architecture.md`
- `02_System_Architecture/Interfaces/Interface_Definition.md`

---

## 3. Selection Criteria

Components are evaluated against the following criteria:

| Criterion | Description |
|---|---|
| Requirements Compliance | Compliance with allocated system and hardware requirements |
| Functional Capability | Ability to implement the allocated hardware functions |
| Performance | Capability to meet required acquisition and communication performance |
| Interface Compatibility | Compatibility with required digital and analog interfaces |
| Electrical Compatibility | Compatibility with the AeroSense supply and logic levels |
| Integration | Suitability for integration into the AeroSense PCB architecture |
| Package | Suitability for PCB assembly and prototype manufacturing |
| Development Support | Availability of development tools, drivers, examples, and documentation |
| Diagnostics | Availability of self-test, monitoring, or diagnostic capabilities where applicable |
| Availability / Lifecycle | Product status and suitability for continued development |

Component cost may be considered during procurement but is not a primary technical selection criterion in this document.

---

# 4. Processing Unit

## 4.1 Allocated Requirements

The processing unit shall support:

- temperature acquisition;
- pressure acquisition;
- three-axis acceleration acquisition;
- supply-voltage measurement;
- CAN communication;
- USB communication;
- programming and debugging;
- watchdog-based recovery;
- diagnostic processing;
- system-state management.

Applicable requirements include:

- `HW-PROC-001`
- `HW-PROC-002`
- `HW-PROC-003`
- `HW-PROC-004`
- `HW-PROC-005`
- `HW-CAN-001`
- `HW-USB-001`
- `HW-DBG-001`

---

## 4.2 Candidate Comparison

Three STM32 microcontrollers were considered.

| Parameter | STM32F072CBT6 | STM32G0B1CBT6 | STM32G431CBT6 |
|---|---|---|---|
| CPU | Arm Cortex-M0 | Arm Cortex-M0+ | Arm Cortex-M4 |
| Maximum CPU Frequency | 48 MHz | 64 MHz | 170 MHz |
| Flash | 128 KB | 128 KB | 128 KB |
| SRAM | 16 KB | up to 144 KB family capability | 32 KB |
| CAN | CAN 2.0 | 2 × FDCAN | 1 × FDCAN |
| USB | USB 2.0 Full Speed | USB Full Speed | USB 2.0 Full Speed |
| ADC | 12-bit | 12-bit | 2 × 12-bit |
| I²C | 2 | 3 | 3 |
| SPI | 2 | 3 | 3 |
| Hardware Watchdog | Yes | Yes | Yes |
| SWD | Yes | Yes | Yes |
| FPU | No | No | Yes |
| DSP Instructions | No | No | Yes |
| Supply | 2.0–3.6 V | 1.7–3.6 V | 1.71–3.6 V |
| Package considered | LQFP-48 | LQFP-48 | LQFP-48 |
| Product Status | Active | Active | Active |

---

## 4.3 Selected Processing Unit

**Selected Component:** STM32G431CBT6  
**Manufacturer:** STMicroelectronics  
**Package:** LQFP-48  

### Selection Rationale

The STM32G431CBT6 is selected as the AeroSense Rev A processing unit.

The device integrates the required FDCAN controller, USB Full-Speed interface, ADC capability, I²C and SPI interfaces, hardware watchdogs, and SWD/JTAG development support.

The Cortex-M4 core additionally provides floating-point and DSP capabilities. These capabilities provide processing margin for future signal-processing functions associated with acceleration and vibration monitoring.

The LQFP-48 package is preferred for Rev A because it provides a practical package for prototype PCB assembly and hardware debugging.

The STM32F072CBT6 satisfies the basic communication requirements but provides significantly lower processing capability.

The STM32G0B1CBT6 provides modern communication peripherals and substantial memory capability, but the STM32G431CBT6 provides additional DSP and floating-point capability relevant to future condition-monitoring functions.

---

# 5. Temperature Sensor

## 5.1 Allocated Requirements

Applicable requirements:

- `HW-SENS-001`
- `HW-SENS-002`
- `SYS-FUN-001`
- `SYS-PERF-001`

The temperature sensor shall provide digital ambient-temperature measurement at an update rate of at least 10 Hz.

---

## 5.2 Candidate Comparison

| Parameter | STTS22H | MCP9808 | TMP117 |
|---|---|---|---|
| Manufacturer | STMicroelectronics | Microchip | Texas Instruments |
| Interface | I²C / SMBus | I²C / SMBus | I²C / SMBus |
| Sensor Type | Local digital temperature | Local digital temperature | Precision digital temperature |
| Accuracy | ±0.5°C class | ±0.5°C max. | up to ±0.1°C |
| Digital Output | Yes | Yes | Yes |
| Factory Calibrated | Yes | Yes | Yes |
| Alert Capability | Yes | Yes | Yes |
| 3.3 V System Compatibility | Yes | Yes | Yes |
| Package | UDFN-6 | DFN-8 / MSOP-8 | WSON-6 / DSBGA |
| Product Status | Active | In Production | Active |

---

## 5.3 Selected Temperature Sensor

**Selected Component:** STTS22H  
**Manufacturer:** STMicroelectronics  

### Selection Rationale

The STTS22H is selected for ambient-temperature measurement.

The sensor provides a calibrated digital temperature measurement through an I²C/SMBus interface and is compatible with the AeroSense 3.3 V digital architecture.

Its accuracy is sufficient for the intended condition-monitoring function, while its digital interface simplifies integration with the selected MCU.

The TMP117 provides higher temperature accuracy but exceeds the current functional requirement.

The MCP9808 is also suitable but provides no significant architectural advantage for AeroSense Rev A.

---

# 6. Pressure Sensor

## 6.1 Allocated Requirements

Applicable requirements:

- `HW-SENS-003`
- `HW-SENS-004`
- `SYS-FUN-002`
- `SYS-PERF-002`

The pressure sensor shall measure ambient atmospheric pressure with an update rate of at least 10 Hz.

---

## 6.2 Candidate Comparison

| Parameter | LPS22HH | BMP390 | BMP581 |
|---|---|---|---|
| Manufacturer | STMicroelectronics | Bosch Sensortec | Bosch Sensortec |
| Measurement Type | Absolute pressure | Absolute pressure | Absolute pressure |
| Pressure Range | 260–1260 hPa | 300–1250 hPa | 300–1250 hPa |
| Maximum Output / Sampling Rate | 200 Hz | 200 Hz | 480 Hz |
| Digital Resolution | 24-bit pressure output | 24-bit | 24-bit |
| I²C | Yes | Yes | Yes |
| SPI | Yes | Yes | Yes |
| I3C | Yes | No | Yes |
| FIFO | Yes | Device dependent | Device dependent |
| 3.3 V Compatibility | Yes | Yes | Yes |
| Package | HLGA | LGA | LGA |
| Product Status | Active | Current product | Current product |

---

## 6.3 Selected Pressure Sensor

**Selected Component:** LPS22HH  
**Manufacturer:** STMicroelectronics  

### Selection Rationale

The LPS22HH is selected for AeroSense Rev A.

Its 260–1260 hPa measurement range covers normal atmospheric-pressure monitoring and its output data rate of up to 200 Hz significantly exceeds the required 10 Hz acquisition rate.

The sensor supports both I²C and SPI and operates from 1.7 V to 3.6 V, allowing direct integration into the 3.3 V AeroSense architecture.

The BMP390 and BMP581 are technically suitable alternatives. The additional sampling capability of the BMP581 is not required for the current AeroSense pressure-monitoring function.

---

# 7. Three-Axis Accelerometer

## 7.1 Allocated Requirements

Applicable requirements:

- `HW-SENS-005`
- `HW-SENS-006`
- `SYS-FUN-003`
- `SYS-PERF-003`

The selected device shall measure acceleration along three orthogonal axes at an acquisition rate of at least 50 Hz.

---

## 7.2 Candidate Comparison

| Parameter | LIS2DW12 | LIS3DH | ADXL345 |
|---|---|---|---|
| Manufacturer | STMicroelectronics | STMicroelectronics | Analog Devices |
| Axes | 3 | 3 | 3 |
| Measurement Ranges | ±2 / ±4 / ±8 / ±16 g | ±2 / ±4 / ±8 / ±16 g | ±2 / ±4 / ±8 / ±16 g |
| Maximum Output Data Rate | 1600 Hz | 5.3 kHz | Suitable above required 50 Hz |
| I²C | Yes | Yes | Yes |
| SPI | Yes | Yes | Yes |
| FIFO | 32-level | 32-level | 32-level |
| Self-Test | Yes | Yes | Supported |
| Digital Output | 16-bit | 16-bit | Up to 13-bit effective resolution |
| Supply Compatibility | 3.3 V compatible | 3.3 V compatible | 3.3 V compatible |
| Package | LGA-12 | LGA-16 | LGA-14 |
| Product Status | Active | Active | Production |

---

## 7.3 Selected Accelerometer

**Selected Component:** LIS2DW12  
**Manufacturer:** STMicroelectronics  

### Selection Rationale

The LIS2DW12 is selected as the AeroSense Rev A three-axis accelerometer.

The device supports all required acceleration axes and provides selectable ±2 g, ±4 g, ±8 g, and ±16 g measurement ranges.

Its maximum output data rate of 1600 Hz significantly exceeds the current 50 Hz system requirement and provides margin for future vibration-monitoring development.

The integrated FIFO and self-test functionality support data acquisition and verification activities.

The LIS3DH provides a higher maximum output data rate and remains a suitable alternative where higher-bandwidth vibration acquisition becomes a system requirement.

The ADXL345 is a mature and well-supported alternative but is not selected for the baseline Rev A architecture.

---

# 8. CAN Transceiver

## 8.1 Allocated Requirements

Applicable requirements:

- `HW-CAN-001`
- `HW-CAN-002`
- `HW-CAN-003`
- `HW-CAN-004`
- `SYS-IF-001`
- `SYS-IF-002`

The CAN physical layer shall support the defined nominal CAN bit rate of 500 kbit/s.

---

## 8.2 Candidate Comparison

| Parameter | TCAN332 | MCP2562FD | TJA1051T/3 |
|---|---|---|---|
| Manufacturer | Texas Instruments | Microchip | NXP |
| Protocol | CAN / CAN FD | CAN / CAN FD | High-speed CAN |
| 500 kbit/s Support | Yes | Yes | Yes |
| MCU Logic Compatibility | 3.3 V | VIO supports logic-level adaptation | 3.3 V compatible I/O variant |
| Bus Interface | CANH / CANL | CANH / CANL | CANH / CANL |
| Differential Physical Layer | Yes | Yes | Yes |
| Package Options | SOIC-8 / SOT-23 | 8-pin packages | SO8 |
| Product Status | Active | In Production | Active |

---

## 8.3 Selected CAN Transceiver

**Selected Component:** TCAN332  
**Manufacturer:** Texas Instruments  

### Selection Rationale

The TCAN332 is selected as the AeroSense Rev A CAN physical-layer transceiver.

The device operates from a single 3.3 V supply and provides direct logic compatibility with the selected 3.3 V MCU architecture.

It supports ISO 11898-2 CAN communication and provides sufficient signaling performance for the required 500 kbit/s CAN network.

The direct 3.3 V implementation reduces the need for an additional 5 V supply rail solely for the CAN transceiver.

The MCP2562FD and TJA1051T/3 remain suitable alternatives where a 5 V CAN transceiver supply architecture is preferred.

---

# 9. Power Architecture

## 9.1 Allocated Requirements

Applicable requirements:

- `HW-PWR-001`
- `HW-PWR-002`
- `HW-PWR-003`
- `HW-PWR-004`
- `SYS-IF-006`

The power subsystem shall generate the internal supply voltage required by the AeroSense electronics from the external DC supply.

---

## 9.2 Architecture Decision

The primary AeroSense digital electronics shall operate from a common **3.3 V supply rail**.

The selected MCU, temperature sensor, pressure sensor, accelerometer, and CAN transceiver are compatible with a 3.3 V architecture.

This minimizes the number of required internal supply rails.

---

## 9.3 Regulator Candidate Comparison

| Parameter | TPS62162 | LMR51430 | L7987 |
|---|---|---|---|
| Manufacturer | Texas Instruments | Texas Instruments | STMicroelectronics |
| Topology | Synchronous Buck | Synchronous Buck | Step-Down Converter |
| Input Capability | Up to 17 V | Up to 36 V | Wide-input capability |
| 3.3 V Output | Supported / fixed variant | Adjustable | Adjustable |
| Output Capability | 1 A | 3 A | Above AeroSense requirement |
| Integrated Protection | Yes | Yes | Yes |
| Primary Advantage | Compact low-power rail | Wide input range and design margin | Industrial power capability |
| Primary Limitation | 17 V maximum input | Higher capability than required | Higher complexity than required |

---

## 9.4 Selected Power Regulator

**Selected Component:** LMR51430  
**Manufacturer:** Texas Instruments  
**Topology:** Synchronous Buck  

### Selection Rationale

The LMR51430 is selected as the baseline AeroSense Rev A power converter.

Its 4.5 V to 36 V input range provides substantial flexibility for the external DC power interface and allows the system to be developed around a 3.3 V internal supply rail.

The device integrates synchronous rectification, internal compensation, current limiting, short-circuit protection, and thermal shutdown.

The available input-voltage margin also permits later refinement of the external AeroSense supply requirement without requiring immediate replacement of the regulator.

The final external DC operating range shall be defined during detailed power-interface design and shall remain within the limits of all selected input-protection components.

---

# 10. Supply-Voltage Monitoring

## 10.1 Architecture

The external supply voltage shall be monitored using an MCU ADC input.

The external voltage shall not be connected directly to the MCU ADC input.

A resistor-divider network shall scale the monitored voltage into the permitted ADC input range.

Additional filtering and input protection shall be defined during schematic design.

---

## 10.2 Selected Implementation

**Measurement Device:** STM32G431 internal ADC  
**Signal Conditioning:** Resistor divider and RC filtering  
**Detailed Component Values:** To be determined during schematic design  

### Selection Rationale

The selected MCU already provides high-performance ADC capability.

A separate external ADC is therefore not required for the current supply-voltage monitoring requirement.

This implementation minimizes unnecessary hardware while maintaining direct software access to the monitored supply voltage.

---

# 11. USB Interface

## 11.1 Selected Implementation

**USB Controller:** STM32G431 integrated USB Full-Speed peripheral  
**External USB Bridge:** Not required  
**Connector:** USB Type-C receptacle, USB 2.0 device configuration  
**USB Power Delivery:** Not required for Rev A  

### Selection Rationale

The selected MCU provides an integrated USB 2.0 Full-Speed device interface.

Using the integrated peripheral avoids an additional USB-to-UART or USB interface controller.

The USB interface shall be used for configuration and development access as allocated by the system architecture.

Detailed USB connector circuitry, ESD protection, and CC resistor configuration shall be defined during schematic design.

---

# 12. Programming and Debug Interface

## 12.1 Candidate Interfaces

| Interface | Capability | Evaluation |
|---|---|---|
| SWD | Programming and debugging | Selected |
| JTAG | Extended debugging | Supported by MCU but not required |
| USB Bootloader | Firmware loading | Secondary development option |

---

## 12.2 Selected Interface

**Primary Interface:** SWD  

The PCB shall provide access to at least:

- SWDIO;
- SWCLK;
- GND;
- target reference voltage;
- NRST where required.

### Selection Rationale

SWD provides programming and debugging functionality with a reduced pin count compared with full JTAG.

It is directly supported by the STM32G431 and the STM32 development ecosystem.

---

# 13. Sensor Communication Architecture

The baseline sensor communication allocation is:

| Device | Primary Interface | Alternative |
|---|---|---|
| STTS22H Temperature Sensor | I²C | — |
| LPS22HH Pressure Sensor | I²C | SPI |
| LIS2DW12 Accelerometer | SPI | I²C |
| Supply Voltage Monitoring | ADC | — |

The temperature and pressure sensors shall share an I²C bus where electrical and address compatibility permits.

The accelerometer shall use SPI in the baseline design.

### Rationale

Separating the accelerometer onto SPI provides an independent communication path for the higher-rate motion-sensing function.

The lower-rate environmental sensors can share the I²C bus without affecting the required acquisition rates.

The final MCU pin allocation shall be defined during schematic design.

---

# 14. CAN Bus Implementation

The baseline CAN interface consists of:

- STM32G431 FDCAN controller;
- TCAN332 physical-layer transceiver;
- CANH;
- CANL;
- bus reference connection as defined during detailed interface design;
- optional termination;
- CAN bus protection.

The requirement for onboard 120 Ω termination shall be configurable rather than permanently fixed where practical.

Detailed termination and protection circuitry shall be defined during schematic design.

---

# 15. Protection Functions

The following protection functions shall be evaluated and implemented during detailed schematic design:

| Interface | Protection Function |
|---|---|
| External DC Input | Reverse polarity, transient suppression, overcurrent protection |
| CAN | ESD / transient protection |
| USB | ESD protection |
| ADC Supply Monitor | Input limiting / filtering |
| MCU Supply | Local decoupling |
| Sensors | Local decoupling |

Specific protection-device part numbers are not selected at this stage because their ratings depend on the final external connector and power-interface definitions.

---

# 16. External Connectors

Connector selection is deferred until the PCB mechanical constraints and enclosure interface are defined.

The following external interfaces require connectors:

- DC power input;
- CAN;
- USB.

USB Type-C is selected as the baseline USB connector type.

The power and CAN connector families shall be selected during detailed mechanical and schematic design.

---

# 17. Selected Component Summary

| Function | Manufacturer | Selected Component | Primary Interface | Status |
|---|---|---|---|---|
| Processing Unit | STMicroelectronics | STM32G431CBT6 | — | Selected |
| Temperature Sensor | STMicroelectronics | STTS22H | I²C | Selected |
| Pressure Sensor | STMicroelectronics | LPS22HH | I²C | Selected |
| Accelerometer | STMicroelectronics | LIS2DW12 | SPI | Selected |
| CAN Transceiver | Texas Instruments | TCAN332 | FDCAN / CAN | Selected |
| Power Regulator | Texas Instruments | LMR51430 | Power | Selected |
| Supply Monitoring | STMicroelectronics | STM32G431 ADC | ADC | Selected |
| USB Controller | STMicroelectronics | STM32G431 integrated USB FS | USB | Selected |
| Debug Interface | STMicroelectronics | STM32G431 SWD | SWD | Selected |
| USB Connector | TBD | USB Type-C receptacle | USB | Architecture Selected |
| CAN Connector | TBD | TBD | CANH / CANL | Open |
| Power Connector | TBD | TBD | DC | Open |
| Protection Devices | TBD | TBD | — | Open |

---

# 18. Resulting Hardware Architecture

The selected baseline hardware architecture is:

                     External DC Power
                            │
                            ▼
                    Protection Circuit
                            │
                            ▼
                       LMR51430
                            │
                          3.3 V
                            │
              ┌─────────────┼─────────────┐
              │             │             │
              ▼             ▼             ▼
          STM32G431      Sensors       TCAN332
              │
      ┌───────┼───────────┐
      │       │           │
     I²C     SPI         ADC
      │       │           │
      │       │           └── Supply Voltage Monitor
      │       │
      │       └────────────── LIS2DW12
      │
      ├────────────────────── STTS22H
      │
      └────────────────────── LPS22HH

          STM32G431
          │    │    │
          │    │    └──────── SWD Debug Interface
          │    │
          │    └───────────── USB Type-C
          │
          └──── FDCAN ── TCAN332 ── CAN Bus

---

# 19. Open Detailed Design Decisions

The following items remain open for detailed hardware design:

- external DC nominal operating range;
- power-input protection topology;
- power connector;
- CAN connector;
- exact USB Type-C receptacle;
- CAN termination implementation;
- CAN protection devices;
- USB ESD protection;
- power-input TVS device;
- fuse / resettable fuse implementation;
- supply-monitor resistor-divider values;
- ADC input filter;
- regulator inductor and capacitor values;
- MCU clock configuration;
- MCU decoupling network;
- sensor decoupling;
- sensor interrupt allocation;
- MCU pin allocation;
- programming/debug connector or test-point implementation;
- PCB dimensions;
- PCB layer stack;
- enclosure.

These items shall be resolved during schematic and PCB development.

---

# 20. Traceability

The component-selection process maintains the following development chain:

**Stakeholder Need → System Requirement → Hardware Requirement → Component Selection → Schematic → PCB Design → Hardware Verification**

Selected components may be revised if detailed electrical analysis, PCB integration, manufacturing constraints, or verification results demonstrate that the selected component does not satisfy its allocated requirements.

---

## Revision History

| Version | Date | Description |
|---|---|---|
| 0.1 | 2026-09-02 | Initial Rev A hardware component selection |
