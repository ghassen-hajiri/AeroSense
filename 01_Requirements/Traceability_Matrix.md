# AeroSense – Requirements Traceability Matrix

**Project:** AeroSense – Embedded Condition Monitoring & Sensor Node  
**Document:** Requirements Traceability Matrix  
**Version:** 0.1  
**Status:** Draft  

## 1. Purpose

This document provides traceability between the stakeholder needs and
system requirements of the AeroSense Embedded Condition Monitoring &
Sensor Node.

The purpose of the traceability matrix is to ensure that:

- each applicable stakeholder need is addressed by one or more system
  requirements;
- each system requirement has a traceable source;
- missing or unjustified requirements can be identified;
- future hardware, software, design, and verification artifacts can be
  traced to their originating requirements.

The traceability matrix will be extended during the development lifecycle
as hardware requirements, software requirements, design elements, and
verification cases are defined.

---

## 2. Traceability Model

The AeroSense development traceability chain is:

**Stakeholder Need → System Requirement → HW/SW Requirement → Design / Implementation → Verification**

At the current development stage, this matrix covers:

**Stakeholder Need → System Requirement**

---

## 3. User / Operator Needs Traceability

| Stakeholder Need | System Requirement | Relationship |
|---|---|---|
| UN-001 | SYS-FUN-001 | Derived |
| UN-001 | SYS-FUN-002 | Derived |
| UN-001 | SYS-PERF-001 | Derived |
| UN-001 | SYS-PERF-002 | Derived |
| UN-002 | SYS-FUN-003 | Derived |
| UN-002 | SYS-PERF-003 | Derived |
| UN-003 | SYS-FUN-004 | Derived |
| UN-003 | SYS-PERF-004 | Derived |
| UN-003 | SYS-DIAG-003 | Derived |
| UN-004 | SYS-FUN-005 | Derived |
| UN-004 | SYS-IF-001 | Derived |
| UN-005 | SYS-FUN-006 | Derived |
| UN-005 | SYS-FUN-007 | Derived |
| UN-005 | SYS-DIAG-001 | Derived |
| UN-005 | SYS-DIAG-005 | Derived |
| UN-005 | SYS-DIAG-006 | Derived |
| UN-005 | SYS-DIAG-007 | Derived |
| UN-005 | SYS-MECH-004 | Derived |
| UN-006 | SYS-FUN-008 | Derived |
| UN-006 | SYS-IF-003 | Derived |
| UN-006 | SYS-MECH-002 | Derived |

---

## 4. System Integrator Needs Traceability

| Stakeholder Need | System Requirement | Relationship |
|---|---|---|
| SI-001 | SYS-IF-001 | Derived |
| SI-001 | SYS-IF-002 | Derived |
| SI-002 | SYS-IF-004 | Derived |
| SI-003 | SYS-IF-005 | Derived |
| SI-003 | SYS-IF-006 | Derived |
| SI-003 | SYS-MECH-002 | Derived |
| SI-004 | SYS-PERF-001 | Derived |
| SI-004 | SYS-PERF-002 | Derived |
| SI-004 | SYS-PERF-003 | Derived |
| SI-004 | SYS-PERF-005 | Derived |
| SI-005 | SYS-FUN-009 | Derived |
| SI-005 | SYS-DIAG-002 | Derived |

---

## 5. Developer / Engineering Needs Traceability

| Stakeholder Need | System Requirement | Relationship |
|---|---|---|
| DEV-001 | SYS-IF-003 | Derived |
| DEV-001 | SYS-DEV-001 | Derived |
| DEV-002 | SYS-DEV-002 | Derived |
| DEV-003 | SYS-FUN-007 | Derived |
| DEV-003 | SYS-DIAG-004 | Derived |
| DEV-004 | SYS-DEV-003 | Derived |
| DEV-005 | SYS-DEV-004 | Derived |
| DEV-005 | SYS-DEV-005 | Derived |

---

## 6. Test / Verification / Maintenance Needs Traceability

| Stakeholder Need | System Requirement | Relationship |
|---|---|---|
| TV-001 | SYS-DEV-005 | Derived |
| TV-002 | SYS-DIAG-004 | Derived |
| TV-003 | SYS-DIAG-006 | Derived |
| TV-003 | SYS-DIAG-007 | Derived |
| TV-004 | SYS-FUN-007 | Derived |
| TV-004 | SYS-DIAG-001 | Derived |
| TV-004 | SYS-DIAG-002 | Derived |
| TV-004 | SYS-DIAG-003 | Derived |
| TV-005 | SYS-FUN-010 | Derived |
| TV-005 | SYS-MFG-005 | Derived |
| TV-006 | SYS-DEV-005 | Derived |

---

## 7. Manufacturing / Product Needs Traceability

| Stakeholder Need | System Requirement | Relationship |
|---|---|---|
| MFG-001 | SYS-MECH-003 | Derived |
| MFG-001 | SYS-MFG-001 | Derived |
| MFG-002 | SYS-MFG-001 | Derived |
| MFG-003 | SYS-MFG-002 | Derived |
| MFG-004 | SYS-MFG-003 | Derived |
| MFG-005 | SYS-MFG-004 | Derived |
| MFG-005 | SYS-MFG-005 | Derived |
| MFG-006 | SYS-MECH-001 | Derived |
| MFG-006 | SYS-MECH-003 | Derived |

---

## 8. System Requirement Source Traceability

This section provides reverse traceability from each system requirement
to its originating stakeholder need or needs.

| System Requirement | Source Stakeholder Need(s) |
|---|---|
| SYS-FUN-001 | UN-001 |
| SYS-FUN-002 | UN-001 |
| SYS-FUN-003 | UN-002 |
| SYS-FUN-004 | UN-003 |
| SYS-FUN-005 | UN-004 |
| SYS-FUN-006 | UN-005 |
| SYS-FUN-007 | UN-005, DEV-003, TV-004 |
| SYS-FUN-008 | UN-006 |
| SYS-FUN-009 | SI-005 |
| SYS-FUN-010 | TV-005 |
| SYS-PERF-001 | UN-001, SI-004 |
| SYS-PERF-002 | UN-001, SI-004 |
| SYS-PERF-003 | UN-002, SI-004 |
| SYS-PERF-004 | UN-003 |
| SYS-PERF-005 | SI-004 |
| SYS-IF-001 | SI-001, UN-004 |
| SYS-IF-002 | SI-001 |
| SYS-IF-003 | UN-006, DEV-001 |
| SYS-IF-004 | SI-002 |
| SYS-IF-005 | SI-003 |
| SYS-IF-006 | SI-003 |
| SYS-DIAG-001 | UN-005, TV-004 |
| SYS-DIAG-002 | SI-005, TV-004 |
| SYS-DIAG-003 | UN-003, TV-004 |
| SYS-DIAG-004 | UN-005, DEV-003, TV-002 |
| SYS-DIAG-005 | UN-005 |
| SYS-DIAG-006 | UN-005, TV-003 |
| SYS-DIAG-007 | UN-005, TV-003 |
| SYS-MECH-001 | MFG-006 |
| SYS-MECH-002 | UN-006, SI-003 |
| SYS-MECH-003 | MFG-001, MFG-006 |
| SYS-MECH-004 | UN-005 |
| SYS-DEV-001 | DEV-001 |
| SYS-DEV-002 | DEV-002 |
| SYS-DEV-003 | DEV-004 |
| SYS-DEV-004 | DEV-005 |
| SYS-DEV-005 | DEV-005, TV-001, TV-006 |
| SYS-MFG-001 | MFG-001, MFG-002 |
| SYS-MFG-002 | MFG-003 |
| SYS-MFG-003 | MFG-004 |
| SYS-MFG-004 | MFG-005 |
| SYS-MFG-005 | MFG-005, TV-005 |

---

## 9. Coverage Review

### Stakeholder Need Coverage

All currently defined stakeholder needs are linked to at least one
system requirement.

**Status: Covered**

### System Requirement Source Coverage

All currently defined system requirements are linked to at least one
stakeholder need.

**Status: Covered**

---

## 10. Future Traceability Extension

The matrix will be extended after hardware and software requirements
have been derived.

The future structure will include:

| Stakeholder Need | System Requirement | HW/SW Requirement | Design / Implementation | Verification Case | Result |
|---|---|---|---|---|---|
| TBD | TBD | TBD | TBD | TBD | TBD |

Verification results will be recorded as:

- PASS
- FAIL
- NOT TESTED
- NOT APPLICABLE

No hardware requirement, software requirement, implementation element,
or verification case is assigned at this stage because these artifacts
have not yet been formally defined.
