# AeroSense – Driver Design

**Project:** AeroSense – Embedded Condition Monitoring & Sensor Node  
**Document ID:** ARS-SDD-DRV-001  
**Revision:** A  
**Version:** 0.1  
**Status:** Draft  

---

## 1. Purpose

This document defines the detailed software design of the hardware-facing drivers used by the AeroSense Rev A firmware.

The driver layer isolates device-specific and MCU-peripheral-specific behavior from the higher-level AeroSense application and service components.

The document defines:

- driver responsibilities;
- hardware interfaces;
- driver initialization behavior;
- proposed C interfaces;
- data conversion responsibilities;
- communication-error handling;
- dependencies on STM32 HAL peripherals;
- interaction with higher software layers.

Register-level configuration values may be refined during implementation and hardware bring-up where required by the selected component datasheets.

---

## 2. Applicable Documents

The driver design is based on:

- `01_Requirements/Software_Requirements.md`
- `02_System_Architecture/System_Architecture.md`
- `02_System_Architecture/Interfaces/Interface_Definition.md`
- `03_Hardware/Component_Selection/Component_Selection.md`
- `03_Hardware/Detailed_Hardware_Design/Schematics/Schematic_Design.md`
- `04_Firmware/Detailed_Software_Design/Software_Architecture.md`

Related detailed software design documents:

- `Diagnostics_Design.md`
- `CAN_Interface.md`
- `State_Machine.md`
- `Scheduling_Design.md`

---

## 3. Driver Layer Overview

The AeroSense Rev A driver layer contains the following hardware-facing software components:

| Driver | Hardware | MCU Interface |
|---|---|---|
| STTS22H Driver | STTS22H temperature sensor | I2C2 |
| LPS22HH Driver | LPS22HH pressure sensor | I2C2 |
| LIS2DW12 Driver | LIS2DW12 accelerometer | SPI1 + GPIO CS |
| VIN Monitor Driver | Input-voltage sensing circuit | ADC1 |
| CAN Driver | FDCAN1 + TCAN332 | FDCAN1 |
| USB Interface | USB host connection | USB FS |
| Status LED Driver | Status LED | GPIO PA1 |
| Watchdog Interface | MCU independent watchdog | IWDG |

The sensor drivers provide physical measurement values to the Measurement Manager.

Hardware communication failures are reported to the higher software layers and are subsequently handled by the Diagnostic Manager.

---

# 4. General Driver Design Rules

All AeroSense device drivers shall follow the following design principles:

1. Hardware-specific communication shall remain inside the responsible driver.
2. Application components shall not directly access sensor registers.
3. Drivers shall expose small and deterministic public interfaces.
4. Drivers shall report success or failure explicitly.
5. Drivers shall validate pointer arguments where applicable.
6. Drivers shall not intentionally contain indefinite blocking loops.
7. Communication timeouts shall be finite.
8. Drivers shall not directly determine the global AeroSense operational state.
9. Drivers shall not directly encode application-level CAN messages.
10. Drivers shall use STM32 HAL interfaces for MCU peripheral access unless a justified implementation reason requires otherwise.

---

# 5. Common Driver Status Concept

Device drivers shall use explicit return status information.

A common conceptual status representation is:

```c
typedef enum
{
    AEROSENSE_DRV_OK = 0,
    AEROSENSE_DRV_ERROR,
    AEROSENSE_DRV_TIMEOUT,
    AEROSENSE_DRV_INVALID_ARG,
    AEROSENSE_DRV_NOT_READY,
    AEROSENSE_DRV_DEVICE_ERROR
} AeroSense_DrvStatus_t;
```

Individual drivers may use the common status type or a driver-specific type where additional information is required.

Detailed diagnostic classification is the responsibility of the Diagnostic Manager rather than the device driver.

---

# 6. STTS22H Temperature Driver

## 6.1 Purpose

The STTS22H driver provides software access to the AeroSense temperature sensor.

The driver is responsible for sensor initialization, communication verification, temperature acquisition, raw-data conversion, and communication-error reporting.

---

## 6.2 Hardware Interface

| Parameter | Design |
|---|---|
| Device | STTS22H |
| MCU peripheral | I2C2 |
| SDA | PA8 |
| SCL | PA9 |
| I2C address | 0x3F (7-bit) |
| HAL address representation | 0x7E |
| Supply | 3.3 V |

The STTS22H shares the I2C2 bus with the LPS22HH pressure sensor.

---

## 6.3 Public Interface

The proposed public interface is:

```c
AeroSense_DrvStatus_t STTS22H_Init(void);

AeroSense_DrvStatus_t STTS22H_CheckDevice(void);

AeroSense_DrvStatus_t STTS22H_ReadTemperature(
    float *temperature_c);
```

The implementation may additionally contain private register read/write functions.

Example internal functions:

```c
static AeroSense_DrvStatus_t STTS22H_ReadRegister(
    uint8_t reg,
    uint8_t *data);

static AeroSense_DrvStatus_t STTS22H_WriteRegister(
    uint8_t reg,
    uint8_t data);
```

These functions shall not be exposed to higher software layers.

---

## 6.4 Initialization

`STTS22H_Init()` shall:

1. verify communication with the device;
2. verify the expected device identity where supported;
3. configure the sensor for the required AeroSense operating mode;
4. configure an acquisition rate sufficient to satisfy the software acquisition requirement;
5. return an explicit initialization status.

Failure to initialize the STTS22H shall not intentionally prevent initialization of unrelated sensors.

---

## 6.5 Temperature Acquisition

`STTS22H_ReadTemperature()` shall:

1. verify the output pointer;
2. acquire the required temperature registers;
3. combine the raw measurement data;
4. convert the raw value into degrees Celsius;
5. store the converted value in the supplied output variable;
6. return the driver status.

The higher-level Measurement Manager determines the validity and age of the application-level measurement.

---

# 7. LPS22HH Pressure Driver

## 7.1 Purpose

The LPS22HH driver provides access to the AeroSense atmospheric pressure sensor.

---

## 7.2 Hardware Interface

| Parameter | Design |
|---|---|
| Device | LPS22HH |
| MCU peripheral | I2C2 |
| SDA | PA8 |
| SCL | PA9 |
| Supply | 3.3 V |

The final I2C device address used by the firmware shall correspond to the hardware address-selection configuration defined by the Rev A schematic.

---

## 7.3 Public Interface

```c
AeroSense_DrvStatus_t LPS22HH_Init(void);

AeroSense_DrvStatus_t LPS22HH_CheckDevice(void);

AeroSense_DrvStatus_t LPS22HH_ReadPressure(
    float *pressure_hpa);
```

Private register-access functions shall remain internal to the driver.

---

## 7.4 Initialization

`LPS22HH_Init()` shall:

1. verify communication with the sensor;
2. verify device identity;
3. configure the required measurement mode;
4. configure a measurement rate sufficient to satisfy AeroSense acquisition requirements;
5. return an explicit initialization status.

---

## 7.5 Pressure Acquisition

`LPS22HH_ReadPressure()` shall:

1. validate the output pointer;
2. acquire the pressure measurement registers;
3. reconstruct the raw pressure value;
4. convert the raw value into the selected engineering unit;
5. return the converted pressure and driver status.

The baseline application representation shall use hectopascals (`hPa`) unless subsequently changed consistently across the software interface definition and CAN interface.

---

# 8. LIS2DW12 Accelerometer Driver

## 8.1 Purpose

The LIS2DW12 driver provides access to the three-axis acceleration sensor.

---

## 8.2 Hardware Interface

| Parameter | Design |
|---|---|
| Device | LIS2DW12 |
| MCU peripheral | SPI1 |
| SCK | PB3 |
| MISO | PB4 |
| MOSI | PB5 |
| Chip Select | PB6 |
| SPI mode | Mode 0 |
| Bit order | MSB first |
| Supply | 3.3 V |

PB6 shall remain HIGH while the device is not selected.

---

## 8.3 Acceleration Data Type

A driver-level acceleration representation is defined conceptually as:

```c
typedef struct
{
    float x_g;
    float y_g;
    float z_g;
} LIS2DW12_Acceleration_t;
```

The use of `g` as the driver engineering unit allows the application layer to receive directly interpretable acceleration values.

---

## 8.4 Public Interface

```c
AeroSense_DrvStatus_t LIS2DW12_Init(void);

AeroSense_DrvStatus_t LIS2DW12_CheckDevice(void);

AeroSense_DrvStatus_t LIS2DW12_ReadAcceleration(
    LIS2DW12_Acceleration_t *acceleration);
```

Private SPI register access may use interfaces such as:

```c
static AeroSense_DrvStatus_t LIS2DW12_ReadRegister(
    uint8_t reg,
    uint8_t *data);

static AeroSense_DrvStatus_t LIS2DW12_WriteRegister(
    uint8_t reg,
    uint8_t data);
```

---

## 8.5 SPI Transaction Behavior

For each SPI transaction the driver shall:

```text
PB6 / CS LOW
      |
      v
SPI transfer
      |
      v
PB6 / CS HIGH
```

Chip Select shall be returned to the inactive HIGH state after each completed or aborted transaction.

---

## 8.6 Initialization

`LIS2DW12_Init()` shall:

1. place the chip-select signal in its inactive state;
2. verify SPI communication;
3. verify the expected device identity;
4. configure the selected acceleration full-scale range;
5. configure an output data rate sufficient to satisfy the required AeroSense acceleration acquisition rate;
6. configure the required operating mode;
7. return initialization status.

---

## 8.7 Acceleration Acquisition

`LIS2DW12_ReadAcceleration()` shall:

1. validate the output pointer;
2. acquire X, Y, and Z measurement data;
3. reconstruct signed raw axis values;
4. apply the sensitivity associated with the configured measurement range;
5. convert the values to the selected engineering unit;
6. return all three axis values together.

All three axes shall belong to the same acquisition cycle.

---

# 9. VIN Monitor Driver

## 9.1 Purpose

The VIN Monitor Driver converts the MCU ADC measurement into the AeroSense external supply voltage.

---

## 9.2 Hardware Interface

| Parameter | Design |
|---|---|
| MCU peripheral | ADC1 |
| ADC input | PA0 |
| Signal | VIN_SENSE |
| Divider high-side resistor | 110 kΩ |
| Divider low-side resistor | 10 kΩ |
| Filter capacitor | 100 nF |
| Nominal divider ratio | 1:12 |
| ADC resolution | 12 bit |

The nominal analog relationship is:

```text
VIN_SENSE = VIN × 10 kΩ / (110 kΩ + 10 kΩ)

VIN_SENSE = VIN / 12
```

Therefore:

```text
VIN = VIN_SENSE × 12
```

---

## 9.3 Voltage Calculation

For a 12-bit ADC using a nominal 3.3 V reference:

```text
VIN = (ADC_RAW / 4095) × 3.3 V × 12
```

The implementation may later use MCU reference-voltage calibration to improve measurement accuracy without changing the external driver interface.

---

## 9.4 Public Interface

```c
AeroSense_DrvStatus_t VINMON_Init(void);

AeroSense_DrvStatus_t VINMON_ReadVoltage(
    float *voltage_v);
```

---

## 9.5 Acquisition

`VINMON_ReadVoltage()` shall:

1. validate the output pointer;
2. start or obtain an ADC conversion;
3. wait for conversion completion using a finite timeout;
4. obtain the raw ADC value;
5. convert the value into volts;
6. return the calculated voltage;
7. report ADC acquisition failure through the driver status.

Supply undervoltage classification is not performed by the driver.

The Diagnostic Manager is responsible for evaluating the measured voltage against diagnostic thresholds.

---

# 10. CAN Driver

## 10.1 Purpose

The CAN Driver provides low-level CAN frame transmission and reception services.

Application-level message encoding is handled by the CAN Message Manager.

---

## 10.2 Hardware Interface

| Parameter | Design |
|---|---|
| MCU peripheral | FDCAN1 |
| CAN RX | PB8 |
| CAN TX | PB9 |
| External transceiver | TCAN332 |
| Nominal bit rate | 500 kbit/s |
| Frame format | Classical CAN |

---

## 10.3 CAN Frame Representation

A generic software representation may use:

```c
typedef struct
{
    uint32_t id;
    uint8_t dlc;
    uint8_t data[8];
} AeroSense_CAN_Frame_t;
```

---

## 10.4 Public Interface

```c
AeroSense_DrvStatus_t CAN_Init(void);

AeroSense_DrvStatus_t CAN_Start(void);

AeroSense_DrvStatus_t CAN_Transmit(
    const AeroSense_CAN_Frame_t *frame);

AeroSense_DrvStatus_t CAN_Receive(
    AeroSense_CAN_Frame_t *frame);
```

Additional status or callback functions may be introduced if required by the final receive architecture.

---

## 10.5 Responsibilities

The CAN Driver shall:

- initialize and start FDCAN operation;
- transmit requested Classical CAN frames;
- obtain received frames;
- report transmission or controller errors;
- provide received data to the higher communication layer.

The CAN Driver shall not:

- assign AeroSense application message IDs;
- convert physical sensor values into CAN payloads;
- determine system operational state.

Those responsibilities belong to higher software layers.

---

# 11. USB Interface

## 11.1 Purpose

The USB Interface provides the low-level communication path between AeroSense and an external host.

The USB connection is intended for configuration, identification, development, and diagnostic access.

---

## 11.2 Hardware Interface

| Parameter | Design |
|---|---|
| MCU USB D- | PA11 |
| MCU USB D+ | PA12 |
| Connector | USB-C |
| USB operation | USB 2.0 Full Speed |
| Device power | Self-powered AeroSense node |

USB VBUS shall not be treated as the primary AeroSense 3.3 V supply.

---

## 11.3 Driver Responsibilities

The USB Interface shall:

- initialize the selected USB device stack;
- receive host data;
- provide received application data to the Configuration Manager;
- transmit requested responses;
- report relevant USB communication status.

USB packet handling shall remain separated from configuration-command interpretation.

The selected USB device class and application protocol shall be documented before final implementation of the configuration interface.

---

# 12. Status LED Driver

## 12.1 Hardware Interface

| Parameter | Design |
|---|---|
| Function | STATUS_LED |
| MCU pin | PA1 |
| Interface | GPIO |

---

## 12.2 Public Interface

A minimal interface is:

```c
void STATUS_LED_Init(void);

void STATUS_LED_On(void);

void STATUS_LED_Off(void);

void STATUS_LED_Toggle(void);
```

The driver only controls the hardware output.

The meaning of LED patterns belongs to application or state-management logic.

---

# 13. Watchdog Interface

## 13.1 Purpose

The Watchdog Interface provides controlled access to the STM32 hardware watchdog.

---

## 13.2 Public Interface

```c
AeroSense_DrvStatus_t WATCHDOG_Init(void);

void WATCHDOG_Refresh(void);
```

---

## 13.3 Design Rules

The watchdog driver shall not independently decide when the watchdog is refreshed.

Refresh authorization belongs to Execution Supervision.

This prevents a failed application cycle from being hidden by unconditional watchdog servicing.

The final watchdog timeout shall be coordinated with the scheduling and worst-case execution behavior defined in `Scheduling_Design.md`.

---

# 14. Error Handling

Drivers shall return errors to their caller rather than directly changing the AeroSense global state.

The intended error propagation is:

```text
Hardware / Communication Failure
             |
             v
        Device Driver
             |
             v
      Driver Status Error
             |
             v
     Measurement / Service
             |
             v
      Diagnostic Manager
             |
             v
        System Manager
             |
             v
NORMAL / DEGRADED / FAULT
```

This separation prevents device-specific software from controlling system-level behavior.

---

# 15. Blocking and Timeout Policy

Hardware communication operations may use STM32 HAL blocking functions during the initial Rev A implementation where the execution time remains bounded and compatible with the scheduling requirements.

All such operations shall use finite timeout values.

Indefinite waits such as the following shall not be used:

```c
while (communication_not_finished)
{
    /* wait forever */
}
```

Timeout values shall be selected in coordination with the final scheduling design.

If later timing measurements show that blocking access prevents required acquisition or communication timing, the affected interface may be migrated to interrupt- or DMA-based operation without changing the higher-level driver API where practical.

---

# 16. Interrupt Policy

Interrupt Service Routines shall remain short.

Interrupt handlers may:

- capture hardware events;
- store received data;
- update minimal synchronization state;
- set processing flags.

Complex application processing, physical-unit conversion, diagnostic evaluation, and CAN application-message construction should execute outside interrupt context.

This supports predictable execution and simplifies verification.

---

# 17. Data Ownership

Each driver owns its device-specific configuration and communication implementation.

Drivers shall not maintain duplicate application-level measurement databases.

The Measurement Manager owns the application-level current measurement representation.

Conceptually:

```text
Sensor
  |
  v
Driver
  |
  | new measurement
  v
Measurement Manager
  |
  +---- CAN Message Manager
  |
  +---- Diagnostic Manager
  |
  +---- Application
```

---

# 18. STM32CubeMX Integration

STM32CubeMX remains responsible for generating the baseline MCU peripheral initialization code.

Generated initialization functions may include:

```c
MX_GPIO_Init();
MX_ADC1_Init();
MX_I2C2_Init();
MX_SPI1_Init();
MX_FDCAN1_Init();
MX_USB_PCD_Init();
```

The exact generated function set depends on the final CubeMX configuration.

AeroSense-specific device logic shall not be unnecessarily implemented inside generated peripheral initialization functions.

Custom code added to CubeMX-managed files shall be placed inside supported `USER CODE` sections where necessary so that code regeneration does not remove manually written logic.

Where practical, AeroSense drivers shall be implemented in dedicated `.c` and `.h` files outside generated source files.

---

# 19. Proposed Implementation Structure

The driver implementation is expected to use a structure similar to:

```text
AeroSense/
└── DeviceDrivers/
    ├── stts22h.c
    ├── stts22h.h
    ├── lps22hh.c
    ├── lps22hh.h
    ├── lis2dw12.c
    ├── lis2dw12.h
    ├── vin_monitor.c
    ├── vin_monitor.h
    ├── can_driver.c
    ├── can_driver.h
    ├── usb_interface.c
    ├── usb_interface.h
    ├── status_led.c
    ├── status_led.h
    ├── watchdog.c
    └── watchdog.h
```

A common driver status definition may be placed in:

```text
aerosense_driver.h
```

The final physical directory structure may be adapted to the STM32Cube-generated CMake project while preserving the logical separation defined by the software architecture.

---

# 20. Driver Integration Sequence

Drivers should be integrated incrementally rather than enabling all hardware functions simultaneously.

The planned Rev A bring-up order is:

```text
1. Status LED / basic MCU execution
          |
          v
2. STTS22H temperature sensor
          |
          v
3. LPS22HH pressure sensor
          |
          v
4. LIS2DW12 accelerometer
          |
          v
5. VIN ADC monitoring
          |
          v
6. CAN communication
          |
          v
7. USB communication
          |
          v
8. Watchdog / execution supervision
```

Each driver shall be verified individually before relying on it as part of the integrated application.

---

# 21. Driver Verification Strategy

Each driver shall support independent verification.

Minimum verification shall include:

| Driver | Principal Verification |
|---|---|
| STTS22H | Device identification and plausible temperature measurement |
| LPS22HH | Device identification and plausible atmospheric pressure measurement |
| LIS2DW12 | Device identification and axis response to physical orientation |
| VIN Monitor | ADC result compared with externally measured VIN |
| CAN | Known frame transmission and reception |
| USB | Host enumeration and bidirectional communication |
| Status LED | Observable GPIO-controlled LED behavior |
| Watchdog | Controlled test demonstrating reset following missing refresh |

Detailed verification procedures and acceptance criteria shall be defined under `06_Verification_Validation`.

---

# 22. Requirement Traceability

The driver design primarily supports the following software requirement groups:

| Requirement Group | Driver Design Contribution |
|---|---|
| SW-INIT-* | Hardware and device initialization |
| SW-ACQ-* | Sensor and VIN acquisition |
| SW-PROC-* | Conversion of raw hardware data |
| SW-CAN-* | Low-level CAN access |
| SW-CFG-* | USB communication interface |
| SW-DIAG-* | Detection and reporting of hardware-access failures |
| SW-WDG-* | Hardware watchdog access |
| SW-ARCH-* | Separation of device-specific software from application logic |
| SW-TEST-* | Independently testable hardware interfaces |

Detailed bidirectional requirement traceability shall be maintained in the project traceability matrix as implementation and verification artifacts are completed.

---

# 23. Design Decisions

The following Rev A driver design decisions are established:

| Decision | Rev A Baseline |
|---|---|
| MCU abstraction | STM32 HAL |
| Temperature interface | I2C2 |
| Pressure interface | I2C2 |
| Accelerometer interface | SPI1 |
| VIN acquisition | ADC1 |
| CAN peripheral | FDCAN1 |
| CAN physical interface | TCAN332 |
| CAN bit rate | 500 kbit/s |
| Driver execution | Primarily synchronous/bounded |
| RTOS dependency | None |
| Error handling | Explicit status return |
| Application measurement ownership | Measurement Manager |
| Global fault decision | Diagnostic Manager / System Manager |

---

# 24. Detailed Design Status

This document defines the baseline AeroSense Rev A driver architecture and software interfaces.

The following details are refined during implementation or in their dedicated design documents:

- exact sensor register configuration;
- communication timeout values;
- detailed USB protocol;
- CAN application message definitions;
- diagnostic fault identifiers and thresholds;
- watchdog timeout;
- final scheduler timing.

The next detailed software design activity is the definition of the AeroSense diagnostic concept in `Diagnostics_Design.md`.

---

## 25. Revision History

| Revision | Version | Description | Status |
|---|---|---|---|
| A | 0.1 | Initial AeroSense Rev A driver design | Draft |
