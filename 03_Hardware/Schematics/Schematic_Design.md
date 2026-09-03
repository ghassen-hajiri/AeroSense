# AeroSense – Schematic Design Specification

**Project:** AeroSense – Embedded Condition Monitoring & Sensor Node  
**Document ID:** ARS-HDS-001  
**Hardware Revision:** Rev A  
**Document Version:** 0.1  
**Status:** Draft  

---

## 1. Scope

This document defines the detailed electrical architecture and schematic design of the AeroSense Rev A hardware.

It translates the hardware requirements, interface definitions, and component-selection decisions into electrical circuit blocks that shall be implemented in the AeroSense schematic.

The schematic design includes:

- external power input;
- input protection;
- 3.3 V power generation;
- STM32 processing unit;
- temperature sensing;
- pressure sensing;
- acceleration sensing;
- supply-voltage monitoring;
- CAN physical-layer interface;
- USB interface;
- programming and debugging interface;
- status indication;
- decoupling and filtering;
- external connectors;
- test points.

Component values not yet justified by component datasheets or electrical analysis remain open until the corresponding circuit is designed and reviewed.

---

## 2. Applicable Documents

- `01_Requirements/System_Requirements.md`
- `01_Requirements/Hardware_Requirements.md`
- `01_Requirements/Software_Requirements.md`
- `01_Requirements/Traceability_Matrix.md`
- `02_System_Architecture/System_Architecture.md`
- `02_System_Architecture/Interfaces/Interface_Definition.md`
- `03_Hardware/Component_Selection/Component_Selection.md`

Manufacturer datasheets and application documentation for the selected components shall be treated as design inputs.

---

## 3. Selected Hardware Baseline

The Rev A schematic shall be based on the following selected components.

| Function | Component |
|---|---|
| Processing Unit | STM32G431CBT6 |
| Temperature Sensor | STTS22H |
| Pressure Sensor | LPS22HH |
| Accelerometer | LIS2DW12 |
| CAN Transceiver | TCAN332 |
| Main Power Converter | LMR51430 |
| Supply Monitoring | STM32G431 ADC |
| USB Controller | STM32G431 USB FS |
| Debug Interface | SWD |

The primary digital supply voltage is:

**VDD = 3.3 V**

---

## 4. Electrical Architecture

The Rev A electrical architecture is divided into the following functional blocks:

1. External Power Input
2. Input Protection
3. 3.3 V Power Supply
4. Processing Unit
5. Environmental Sensors
6. Motion Sensor
7. Supply-Voltage Monitoring
8. CAN Interface
9. USB Interface
10. Programming and Debug Interface
11. Status Indication
12. External Connectors
13. Test and Measurement Points

---

## 5. Top-Level Power and Signal Architecture

```text
                         AeroSense Rev A

                        External DC Input
                               │
                               ▼
                    ┌─────────────────────┐
                    │   Input Protection  │
                    └──────────┬──────────┘
                               │
                             VIN_PROT
                               │
                    ┌──────────▼──────────┐
                    │      LMR51430       │
                    │   DC/DC Converter   │
                    └──────────┬──────────┘
                               │
                              3V3
                               │
             ┌─────────────────┼──────────────────┐
             │                 │                  │
             ▼                 ▼                  ▼
      ┌─────────────┐    ┌───────────┐      ┌───────────┐
      │ STM32G431   │    │ Sensors   │      │ TCAN332   │
      └──────┬──────┘    └─────┬─────┘      └─────┬─────┘
             │                 │                  │
       ┌─────┼─────┐      I2C / SPI             CAN
       │     │     │                             │
      USB   SWD   ADC                         CANH/CANL
                   │
                   ▼
           Supply Monitoring
```

---

## 6. Power Input

### 6.1 Function

The external power interface shall provide DC power to the AeroSense electronics.

The external supply shall enter the PCB through a dedicated power connector.

### 6.2 Signals

| Signal | Description |
|---|---|
| VIN_RAW | External unprotected DC input |
| GND | System ground |
| VIN_PROT | Protected DC supply after input protection |
| 3V3 | Regulated internal 3.3 V rail |

### 6.3 Input Protection

The power-input stage shall provide protection against relevant electrical faults.

The baseline protection architecture shall include provisions for:

- reverse-polarity protection;
- transient-voltage suppression;
- overcurrent protection;
- input filtering.

Baseline signal path:

```text
POWER CONNECTOR
      │
    VIN_RAW
      │
      ▼
Overcurrent Protection
      │
      ▼
Reverse-Polarity Protection
      │
      ▼
Transient Protection
      │
      ▼
Input Filtering
      │
      ▼
   VIN_PROT
```

Exact protection components and ratings shall be selected after the external DC operating range has been finalized.

---

## 7. 3.3 V Power Supply

### 7.1 Main Converter

The LMR51430 shall generate the primary 3.3 V rail.

```text
VIN_PROT
   │
   ▼
┌─────────────┐
│  LMR51430   │
└──────┬──────┘
       │
      3V3
       │
 ┌─────┼──────────┬──────────────┐
 │     │          │              │
 ▼     ▼          ▼              ▼
MCU   Sensors   CAN PHY      Indicators
```

### 7.2 Converter Circuit

The converter circuit shall contain the components required by the manufacturer reference design, including as applicable:

- input capacitor;
- bootstrap capacitor;
- power inductor;
- feedback network;
- output capacitors;
- enable configuration;
- local grounding.

The final component values shall be calculated for:

- final input-voltage range;
- 3.3 V output voltage;
- expected load current;
- switching-frequency configuration;
- output ripple;
- transient response.

These values shall be documented before schematic release.

### 7.3 Power-Rail Distribution

The 3.3 V rail shall supply:

- STM32G431;
- STTS22H;
- LPS22HH;
- LIS2DW12;
- TCAN332;
- pull-up networks;
- status indicators where applicable.

Each IC shall receive local supply decoupling.

---

## 8. Processing Unit

### 8.1 MCU

The primary processing unit shall be:

**STM32G431CBT6**

Package:

**LQFP-48**

### 8.2 MCU Functional Connections

The MCU shall provide interfaces for:

| Function | MCU Peripheral |
|---|---|
| Temperature Sensor | I²C |
| Pressure Sensor | I²C |
| Accelerometer | SPI |
| Supply Monitoring | ADC |
| CAN | FDCAN |
| Host Communication | USB FS |
| Programming / Debug | SWD |
| Status Indication | GPIO |
| Watchdog | Internal IWDG / WWDG |

Exact peripheral instances and GPIO pin assignments shall be finalized during schematic capture.

---

## 9. MCU Power Circuit

All required STM32 supply and ground pins shall be connected according to the STM32G431 datasheet and hardware-design recommendations.

The design shall include:

- digital supply connections;
- analog supply connections;
- ground connections;
- local decoupling;
- analog-supply filtering where required;
- reference-voltage connection;
- reset circuit;
- boot configuration.

Decoupling capacitors shall be located electrically and physically close to the corresponding MCU supply pins.

The exact capacitor network shall follow the MCU manufacturer recommendations.

---

## 10. MCU Reset

The MCU reset signal shall be available as:

**NRST**

The reset network shall support:

- normal power-up reset;
- external debugging;
- manual reset where implemented;
- programming and bring-up.

NRST shall also be accessible through the debug interface or a dedicated test point.

---

## 11. MCU Boot Configuration

The MCU boot configuration shall ensure deterministic startup into the AeroSense application during normal operation.

Boot-related pins shall not be left electrically undefined.

Where bootloader access is required during development, the design shall provide an appropriate method of controlling the boot configuration.

---

## 12. MCU Clocking

The baseline design shall evaluate operation using the STM32 internal clock sources.

An external crystal shall only be added where required by:

- USB clock accuracy;
- communication timing;
- system-performance requirements;
- verification results.

The final clock architecture shall be documented during schematic review.

---

## 13. Temperature Sensor Circuit

### 13.1 Device

**STTS22H**

Primary interface:

**I²C**

### 13.2 Connections

```text
STM32G431                STTS22H

I2C_SCL  ─────────────── SCL
I2C_SDA  ─────────────── SDA
3V3      ─────────────── VDD
GND      ─────────────── GND
GPIO     ─────────────── ALERT/INT   (optional)
                         ADDR
```

### 13.3 I²C Address

The STTS22H address-selection input shall be configured to provide a fixed and documented I²C address.

The selected address shall not conflict with any other device connected to the same I²C bus.

### 13.4 Decoupling

Local supply decoupling shall be implemented according to the STTS22H datasheet.

---

## 14. Pressure Sensor Circuit

### 14.1 Device

**LPS22HH**

Primary interface:

**I²C**

### 14.2 Connections

```text
STM32G431                LPS22HH

I2C_SCL  ─────────────── SCL
I2C_SDA  ─────────────── SDA
3V3      ─────────────── VDD
GND      ─────────────── GND
GPIO     ─────────────── INT_DRDY    (optional)
```

The device shall be configured for I²C operation.

Required mode-selection pins shall be connected according to the manufacturer datasheet.

### 14.3 Pressure Exposure

The pressure-sensor package shall remain exposed to ambient atmospheric pressure.

PCB placement and the later enclosure design shall not block the sensor pressure port.

This constraint shall be transferred to the PCB and mechanical design.

---

## 15. Environmental Sensor I²C Bus

The STTS22H and LPS22HH shall share the environmental-sensor I²C bus.

```text
                     3V3
                      │
                ┌─────┴─────┐
                │           │
              RPU_SCL     RPU_SDA
                │           │
                ▼           ▼
STM32 ───────── SCL         SDA ───────── STM32
                │           │
        ┌───────┴────┐ ┌────┴───────┐
        │ STTS22H    │ │ LPS22HH    │
        └────────────┘ └────────────┘
```

The bus shall contain pull-up resistors to 3.3 V.

The final pull-up resistance shall be selected based on:

- total bus capacitance;
- selected I²C operating frequency;
- device electrical specifications;
- rise-time requirements.

---

## 16. Accelerometer Circuit

### 16.1 Device

**LIS2DW12**

Primary interface:

**SPI**

### 16.2 Connections

```text
STM32G431                 LIS2DW12

SPI_SCK   ─────────────── SCL/SPC
SPI_MOSI  ─────────────── SDA/SDI/SDO
SPI_MISO  ─────────────── SDO/SA0
GPIO_CS   ─────────────── CS
GPIO_INT1 ─────────────── INT1
GPIO_INT2 ─────────────── INT2    (optional)
3V3       ─────────────── VDD
3V3       ─────────────── VDD_IO
GND       ─────────────── GND
```

### 16.3 Sensor Interrupt

At least one accelerometer interrupt output should be routed to an MCU GPIO.

This permits future implementation of:

- data-ready interrupts;
- FIFO events;
- motion events;
- diagnostic functions.

### 16.4 Decoupling

The VDD and VDD_IO supplies shall be decoupled according to the LIS2DW12 manufacturer recommendations.

### 16.5 PCB Placement Constraint

The accelerometer shall be placed in a mechanically appropriate region of the PCB.

The PCB shall provide clear identification of the sensor coordinate axes:

**+X, +Y, +Z**

The accelerometer orientation shall be documented and shall remain consistent with the firmware coordinate definition.

---

## 17. Supply-Voltage Monitoring

### 17.1 Architecture

The protected external supply voltage shall be measured by an STM32 ADC channel.

```text
VIN_PROT
   │
   R_TOP
   │
   ├──────────── ADC_VIN_MON ───── STM32 ADC
   │
 R_BOTTOM
   │
  GND
```

An optional capacitor may be connected from `ADC_VIN_MON` to ground to provide low-pass filtering.

### 17.2 Divider Design

The resistor divider shall satisfy:

```text
V_ADC = V_IN × R_BOTTOM / (R_TOP + R_BOTTOM)
```

At the maximum permitted external input voltage:

```text
V_ADC_MAX < MCU ADC maximum input voltage
```

with appropriate electrical margin.

The final resistor values shall be calculated after the external DC operating range is defined.

---

## 18. CAN Interface

### 18.1 Architecture

```text
STM32G431
   │
   │ FDCAN_TX
   ▼
┌──────────────┐
│   TCAN332    │
└──────────────┘
   ▲        │
   │        │
FDCAN_RX    ├──────── CANH
            │
            └──────── CANL
```

### 18.2 MCU Interface

The following signals shall connect the MCU and transceiver:

| MCU Signal | TCAN332 Signal |
|---|---|
| FDCAN_TX | TXD |
| FDCAN_RX | RXD |
| 3V3 | VCC |
| GND | GND |

### 18.3 CAN External Interface

The external CAN connector shall provide:

- CANH;
- CANL;
- GND/reference where required.

### 18.4 CAN Termination

The PCB shall provide provision for a **120 Ω CAN termination resistor**.

The termination shall be configurable so that AeroSense may operate as:

- a terminated end node; or
- a non-terminated intermediate node.

Possible implementation:

```text
CANH ─────┬──────────── CAN Connector
          │
        120 Ω
          │
CANL ─────┴──────────── CAN Connector
```

The resistor may be enabled using a jumper, solder bridge, or assembly option.

### 18.5 CAN Protection

CANH and CANL shall include provision for external-interface transient and ESD protection.

Protection components shall be located near the external CAN connector.

The final protection device shall be selected according to:

- CAN common-mode requirements;
- expected transient environment;
- parasitic capacitance;
- clamping voltage;
- PCB layout requirements.

---

## 19. USB Interface

### 19.1 Architecture

The STM32 integrated USB Full-Speed peripheral shall connect directly to a USB Type-C receptacle configured as a USB 2.0 device.

```text
STM32G431                       USB-C

USB_DM  ─────────────────────── D-
USB_DP  ─────────────────────── D+

                                  CC1
                                   │
                              Configuration
                               Resistor
                                   │
                                  GND

                                  CC2
                                   │
                              Configuration
                               Resistor
                                   │
                                  GND
```

### 19.2 USB Signals

The USB circuit shall include:

- D+;
- D−;
- CC1;
- CC2;
- connector ground;
- shield connection;
- ESD protection.

VBUS sensing or power use shall be implemented only where required by the final USB architecture.

### 19.3 USB-C Configuration

The USB Type-C connector shall be configured as a USB device / sink for the USB 2.0 communication function.

The required CC resistors shall be implemented according to the applicable USB Type-C configuration.

The exact resistor values shall be confirmed during schematic implementation.

USB Power Delivery is outside the scope of Rev A.

### 19.4 USB Protection

Low-capacitance ESD protection shall be provided for externally accessible USB data signals.

Protection devices shall be located close to the USB connector.

---

## 20. Programming and Debug Interface

### 20.1 Interface

The primary programming/debug interface shall be:

**SWD**

### 20.2 Signals

The debug connection shall provide:

| Signal | Function |
|---|---|
| SWDIO | Serial Wire Data |
| SWCLK | Serial Wire Clock |
| NRST | Target Reset |
| 3V3_REF | Target Voltage Reference |
| GND | Ground |

Optional:

| Signal | Function |
|---|---|
| SWO | Serial Wire Output |

### 20.3 Physical Implementation

The SWD interface may be implemented using:

- compact pin header;
- Tag-Connect-compatible footprint;
- test pads.

The final implementation shall support both development and PCB bring-up.

---

## 21. Status Indication

The Rev A PCB should provide visual indication for basic system states.

Baseline indicators:

| Indicator | Function |
|---|---|
| LED_PWR | 3.3 V power present |
| LED_STATUS | Firmware-controlled system status |

An optional communication LED may be provided for CAN activity.

GPIO-controlled LEDs shall include appropriate current-limiting resistors.

---

## 22. Test Points

The PCB shall provide accessible test points for critical signals.

Minimum test-point set:

| Test Point | Signal |
|---|---|
| TP_GND | Ground |
| TP_VIN | Protected input voltage |
| TP_3V3 | 3.3 V rail |
| TP_NRST | MCU reset |
| TP_SWDIO | SWD data |
| TP_SWCLK | SWD clock |
| TP_CANH | CAN High |
| TP_CANL | CAN Low |
| TP_I2C_SCL | Environmental sensor clock |
| TP_I2C_SDA | Environmental sensor data |
| TP_ADC_MON | Supply-monitor ADC signal |

Additional test points may be added during schematic review.

---

## 23. Ground Architecture

The PCB shall use a common system ground reference.

High-current switching paths associated with the DC/DC converter shall be kept electrically and physically compact.

Sensitive sensor and ADC signal paths shall be routed away from high-current switching nodes.

Ground return paths shall be considered during PCB layout.

Artificial ground splitting shall not be introduced without a documented electrical reason.

---

## 24. Decoupling Strategy

Every active integrated circuit shall receive local supply decoupling according to its manufacturer recommendations.

General design rules:

- high-frequency decoupling shall be located close to IC supply pins;
- capacitor ground paths shall be short;
- bulk capacitance shall be provided where required;
- power-converter capacitors shall follow regulator layout recommendations;
- sensor decoupling shall follow sensor datasheets.

Exact capacitor values shall be recorded in the released schematic and BOM.

---

## 25. Net Naming Convention

The schematic shall use descriptive net names.

| Net | Function |
|---|---|
| VIN_RAW | External DC input |
| VIN_PROT | Protected DC input |
| 3V3 | Main regulated supply |
| GND | System ground |
| I2C_SCL | Environmental sensor I²C clock |
| I2C_SDA | Environmental sensor I²C data |
| IMU_SCK | Accelerometer SPI clock |
| IMU_MOSI | Accelerometer SPI data input |
| IMU_MISO | Accelerometer SPI data output |
| IMU_CS | Accelerometer chip select |
| IMU_INT1 | Accelerometer interrupt |
| ADC_VIN_MON | Supply-monitor ADC signal |
| CAN_TX | MCU-to-transceiver CAN data |
| CAN_RX | Transceiver-to-MCU CAN data |
| CANH | CAN bus high |
| CANL | CAN bus low |
| USB_DP | USB D+ |
| USB_DM | USB D- |
| SWDIO | SWD data |
| SWCLK | SWD clock |
| NRST | MCU reset |

---

## 26. Schematic Sheet Structure

The KiCad schematic should be organized into hierarchical sheets.

Recommended structure:

```text
AeroSense_RevA.kicad_sch
│
├── 01_Power.kicad_sch
├── 02_MCU.kicad_sch
├── 03_Sensors.kicad_sch
├── 04_CAN.kicad_sch
├── 05_USB.kicad_sch
└── 06_Debug_IO.kicad_sch
```

### 26.1 Sheet 01 – Power

Contains:

- power connector;
- protection;
- LMR51430;
- power filtering;
- 3.3 V generation;
- voltage-monitor divider.

### 26.2 Sheet 02 – MCU

Contains:

- STM32G431;
- supply connections;
- decoupling;
- reset;
- boot configuration;
- clock circuit;
- MCU net labels.

### 26.3 Sheet 03 – Sensors

Contains:

- STTS22H;
- LPS22HH;
- LIS2DW12;
- I²C pull-ups;
- sensor decoupling;
- interrupt connections.

### 26.4 Sheet 04 – CAN

Contains:

- TCAN332;
- CAN termination;
- CAN protection;
- CAN connector.

### 26.5 Sheet 05 – USB

Contains:

- USB Type-C connector;
- CC configuration;
- USB ESD protection;
- D+/D− interface.

### 26.6 Sheet 06 – Debug / I/O

Contains:

- SWD interface;
- status LEDs;
- test points;
- optional user/debug signals.

---

## 27. Schematic Design Rules

The following rules shall be applied during schematic capture:

1. No required IC input shall be left unintentionally floating.
2. All IC power pins shall be explicitly represented or verified.
3. All active ICs shall include required decoupling.
4. External interfaces shall include appropriate protection where required.
5. Connector pins shall have explicit signal names.
6. Test points shall use identifiable reference designators.
7. Component reference designators shall be unique.
8. Unused MCU pins shall be documented.
9. Sensor mode-selection pins shall have defined states.
10. Boot and reset signals shall have defined states.
11. Net names shall be consistent across schematic sheets.
12. All selected footprints shall correspond to the exact ordered package.
13. Manufacturer datasheet recommendations shall be reviewed before circuit release.

---

## 28. Design Verification Before PCB Layout

The schematic shall not be released to PCB layout until the following checks have been completed.

| Check | Status |
|---|---|
| MCU power pins verified | OPEN |
| MCU decoupling verified | OPEN |
| MCU boot configuration verified | OPEN |
| MCU reset circuit verified | OPEN |
| MCU clock architecture verified | OPEN |
| MCU pin allocation verified | OPEN |
| STTS22H circuit verified | OPEN |
| LPS22HH circuit verified | OPEN |
| LIS2DW12 circuit verified | OPEN |
| I²C addresses verified | OPEN |
| SPI configuration verified | OPEN |
| CAN transceiver circuit verified | OPEN |
| CAN termination verified | OPEN |
| CAN protection verified | OPEN |
| USB-C circuit verified | OPEN |
| USB ESD protection verified | OPEN |
| Power-converter calculations completed | OPEN |
| Power protection verified | OPEN |
| ADC divider calculated | OPEN |
| SWD interface verified | OPEN |
| Test points verified | OPEN |
| Connector pinouts verified | OPEN |
| ERC completed without unexplained errors | OPEN |
| Footprints verified against datasheets | OPEN |

---

## 29. Open Detailed Design Items

The following parameters remain to be resolved during schematic capture and electrical analysis:

- final external DC operating range;
- input fuse or resettable-fuse selection;
- reverse-polarity circuit;
- input TVS device;
- LMR51430 switching-frequency configuration;
- power-inductor value and current rating;
- regulator input capacitance;
- regulator output capacitance;
- regulator feedback values;
- MCU decoupling network;
- MCU clock configuration;
- I²C pull-up resistance;
- sensor interrupt allocation;
- STTS22H address configuration;
- LPS22HH mode configuration;
- LIS2DW12 SPI configuration;
- ADC divider values;
- ADC filter values;
- CAN TVS device;
- CAN termination implementation;
- USB Type-C receptacle part number;
- USB CC resistor implementation;
- USB ESD device;
- power connector;
- CAN connector;
- SWD physical connector;
- status LED values;
- exact MCU pin allocation.

These items shall be closed before the Rev A schematic is released for PCB layout.

---

## 30. Design Output

Completion of the schematic-design phase shall produce:

- `AeroSense_RevA.kicad_sch`;
- hierarchical schematic sheets;
- reviewed component reference designators;
- assigned PCB footprints;
- updated component-selection information where required;
- preliminary BOM;
- completed electrical-rule check;
- schematic PDF for design review.

---

## 31. Traceability

The detailed hardware design maintains the following development chain:

**Stakeholder Need → System Requirement → Hardware Requirement → Component Selection → Schematic Design → PCB Design → Hardware Verification**

The schematic shall implement the selected hardware architecture without introducing functionality that conflicts with the allocated system and hardware requirements.

---

## 32. Release Criteria

The AeroSense Rev A schematic may proceed to PCB layout when:

- all critical open electrical parameters are resolved;
- component packages and footprints are verified;
- power architecture is electrically verified;
- communication interfaces are electrically verified;
- sensor interfaces are electrically verified;
- programming/debug access is verified;
- schematic ERC has been completed;
- remaining warnings are reviewed and documented;
- schematic design review is completed.

---

## Revision History

| Version | Date | Description |
|---|---|---|
| 0.1 | 2026-09-03 | Initial AeroSense Rev A schematic design specification |
