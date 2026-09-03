# AeroSense – Power Supply Design

Project: AeroSense – Embedded Condition Monitoring & Sensor Node  
Hardware Revision: Rev A  
Document ID: ARS-PWR-001  
Document Version: 1.0  
Status: Design Baseline

## 1. Power Requirements

The AeroSense Rev A hardware shall be powered from an external DC supply.

- Nominal input voltage: 12 V DC
- Operating input voltage range: 9 V to 16 V DC
- Main regulated rail: 3.3 V
- 3.3 V design current: 250 mA
- Maximum design output power: 0.825 W
- USB shall not provide the primary system power.
- The external DC input shall include overcurrent, reverse-polarity,
  and transient-voltage protection.

## 2. Power Budget

| Load | Design Current |
|---|---:|
| STM32G431CBT6 and MCU peripherals | 60 mA |
| TCAN332 CAN transceiver | 60 mA |
| STTS22H temperature sensor | 1 mA |
| LPS22HH pressure sensor | 1 mA |
| LIS2DW12 accelerometer | 1 mA |
| LEDs, pull-ups and auxiliary circuitry | 20 mA |
| Engineering reserve | 107 mA |
| **Total design current** | **250 mA** |

The design current intentionally includes significant engineering
margin above the expected nominal operating current.

At 3.3 V and 250 mA:

P_OUT = 3.3 V × 0.25 A = 0.825 W

Using a conservative assumed converter efficiency of 85 %, the
maximum estimated input current at the minimum input voltage is:

I_IN = 0.825 W / (9 V × 0.85) = 108 mA

The normal power-input current is therefore expected to remain
well below 500 mA.

## 3. Power Architecture

External DC Input
→ Input PTC Protection
→ Reverse-Polarity Protection
→ Transient Protection
→ Buck Converter
→ 3.3 V Rail

## 4. Input Connector

Reference: J1

Electrical interface:

- Pin 1: VIN_RAW
- Pin 2: GND
- Nominal voltage: 12 V DC
- Allowed operating voltage: 9 V to 16 V DC

J1 uses a two-pin schematic interface. The final PCB connector shall
use a two-position wire-to-board connector suitable for at least
24 V DC and 0.5 A.

## 5. Input Overcurrent Protection

Reference: F1

Part:
Littelfuse 1206L050/24

Type:
Resettable polymer PTC

Nominal hold-current class:
0.50 A

Voltage rating:
24 V

Package:
1206 SMD

Rationale:

The expected maximum normal input current is approximately 108 mA
at 9 V input under the 250 mA 3.3 V design-load condition.

A 500 mA PTC therefore provides substantial operating margin while
providing protection against sustained input overcurrent and
downstream short-circuit faults.

## 6. Reverse-Polarity Protection

Reference: D1

Part:
SS14

Type:
Schottky rectifier

Ratings:

- Maximum repetitive reverse voltage: 40 V
- Average forward-current rating: 1 A
- Package: SMA / DO-214AC

D1 shall be connected in series with the positive input supply.

The Schottky topology was selected instead of a MOSFET-based
reverse-polarity circuit to minimize component count and design
complexity. The expected AeroSense input current is sufficiently low
that the associated forward-voltage power loss is acceptable.

## 7. Transient Protection

Reference: D2

Part:
Littelfuse SMBJ18A

Type:
Unidirectional TVS diode

Electrical ratings:

- Reverse standoff voltage: 18 V
- Minimum breakdown voltage: 20 V
- Maximum clamping voltage: 29.2 V
- Peak pulse power: 600 W

D2 shall be connected from VIN_PROT to GND with its cathode connected
to VIN_PROT and its anode connected to GND.

The TVS diode remains inactive throughout the specified 9 V to 16 V
normal operating range and clamps positive input transients below the
36 V maximum input rating of the selected buck converter under the
specified TVS test conditions.

## 8. Buck Converter

Reference: U1

Manufacturer:
Texas Instruments

Part:
LMR51430XDDCR

Topology:
Synchronous buck converter

Operating mode:
PFM

Switching frequency:
500 kHz

Input operating range of device:
4.5 V to 36 V

AeroSense input range:
9 V to 16 V

Output:
3.3 V

Maximum device capability:
3 A

AeroSense design load:
250 mA

## 9. Buck Inductor

Reference: L1

Manufacturer:
Coilcraft

Part:
XAL5050-562MEC

Inductance:
5.6 µH

Saturation current:
6.3 A

Package:
XAL5050

The 5.6 µH value follows the LMR51430 500 kHz / 3.3 V design
recommendation.

## 10. Feedback Network

Reference: R1
Value: 100 kΩ

Reference: R2
Value: 22.1 kΩ

These values configure the adjustable LMR51430 output for 3.3 V.

Recommended resistor tolerance:
1 %

## 11. Input Capacitors

C1:
4.7 µF, 50 V, X7R ceramic

C2:
4.7 µF, 50 V, X7R ceramic

C3:
100 nF, 50 V, X7R ceramic

C1 and C2 provide input-energy decoupling.
C3 provides local high-frequency input decoupling and shall be placed
immediately adjacent to the VIN and GND pins of U1.

## 12. Output Capacitors

C4:
22 µF, 25 V, X7R ceramic

C5:
22 µF, 25 V, X7R ceramic

The total nominal output capacitance is 44 µF.

The capacitors shall be placed close to the power-stage current path.

## 13. Bootstrap Capacitor

Reference: C6

Value:
100 nF

Voltage rating:
16 V minimum

Dielectric:
X7R

C6 shall be connected between the CB and SW pins of U1.

## 14. Power Nets

VIN_RAW:
Unprotected external DC input directly after J1.

VIN_FUSED:
Input supply after F1.

VIN_PROT:
Protected supply after reverse-polarity protection.

3V3:
Main regulated 3.3 V system rail.

GND:
Common system reference.

## 15. Final Power-Stage Component Baseline

| Ref. | Function | Selected Component | Value / Rating |
|---|---|---|---|
| J1 | DC input | 2-pin connector | 9–16 V DC |
| F1 | Overcurrent protection | Littelfuse 1206L050/24 | 0.50 A PTC |
| D1 | Reverse-polarity protection | SS14 | 1 A, 40 V |
| D2 | Input transient protection | Littelfuse SMBJ18A | 18 V TVS |
| U1 | Buck converter | TI LMR51430XDDCR | 500 kHz |
| L1 | Buck inductor | Coilcraft XAL5050-562MEC | 5.6 µH |
| R1 | Feedback upper resistor | Standard 1 % resistor | 100 kΩ |
| R2 | Feedback lower resistor | Standard 1 % resistor | 22.1 kΩ |
| C1 | Input capacitor | X7R ceramic | 4.7 µF / 50 V |
| C2 | Input capacitor | X7R ceramic | 4.7 µF / 50 V |
| C3 | HF input bypass | X7R ceramic | 100 nF / 50 V |
| C4 | Output capacitor | X7R ceramic | 22 µF / 25 V |
| C5 | Output capacitor | X7R ceramic | 22 µF / 25 V |
| C6 | Bootstrap capacitor | X7R ceramic | 100 nF / ≥16 V |

## 16. Schematic Implementation

The power schematic shall implement the following electrical path:

J1.1 VIN_RAW
→ F1
→ D1
→ VIN_PROT
→ U1 LMR51430XDDCR
→ L1
→ 3V3

J1.2 shall connect directly to GND.

D2 SMBJ18A shall connect between VIN_PROT and GND.

The LMR51430 feedback network shall use 100 kΩ and 22.1 kΩ
resistors.

The converter shall use 5.6 µH output inductance, 44 µF nominal
output capacitance, and the specified input and bootstrap
capacitors.
