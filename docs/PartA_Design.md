# PART A – DESIGN

**Student:** Mohammed Harsath N (192511097)

## TASK A1 – DESIGN THE POLYMORPHIC BILL HIERARCHY

### A1.1 Common Interface

Every bill must provide:
- `double total() const` – compute the amount due
- `std::string type() const` – return a short type identifier ("OP", "IP", ...)
- `void print() const` – display the bill (can be non-virtual, calling virtuals)
- Common data: patient name / ID

### A1.2 Hierarchy (UML – textual)

```
          <<abstract>>
             Bill
    + patient : string
    + Bill(patient)
    + ~Bill()  [virtual]
    + total() : double   [pure virtual]
    + type() : string    [virtual]
    + print() : void
           ^
           |
   +-------+--------+----------+------------+
   |                |          |            |
OutPatientBill  InPatientBill DayCareBill EmergencyBill
- consultation  - days        - package   - base
- tests         - perDay      - price     - hourlyRate
                - surgery                 - hours
                - medicines
+ total() ov    + total() ov  + total() ov + total() ov
+ type() ov     + type() ov   + type() ov  + type() ov
```

All `total()` and `type()` are overridden. Destructor is virtual.

### A1.3 Process All Bills (Pseudocode – SIMATS style)

```
ALGORITHM ComputeDayTotal
INPUT: array of Bill pointers bills[0..n-1]
OUTPUT: dayTotal

BEGIN
  dayTotal ← 0
  FOR i ← 0 TO n-1 DO
    dayTotal ← dayTotal + bills[i]->total()   // runtime polymorphic call
  END FOR
  RETURN dayTotal
END
```

### Alternative Concept (switch + type field)

One `struct Bill { enum Type t; ... fields; }` with `total()` containing a switch.

- Time for n bills: still O(n)
- Space: no vptr, but larger struct if union of all fields, or wasted space
- Adding a new type: must edit the enum, the switch in total(), and every other switch in the program → violates Open-Closed Principle.

Virtual solution requires only adding a new derived class.

## TASK A2 – UNDERSTAND RUNTIME BINDING

### A2.1 Early vs Late Binding

- Early (compile-time): non-virtual call resolved by static type of the pointer/reference.
- Late (run-time): virtual call resolved by dynamic type via vtable.

`Bill* b = new OPBill; b->total();` uses late binding → OPBill::total().

### A2.2 Vtables (simplified)

```
Bill vtable:
  ~Bill
  total   → pure (or null)
  type    → Bill::type

OPBill vtable:
  ~OPBill
  total   → OPBill::total
  type    → OPBill::type

InPatientBill vtable:
  ~InPatientBill
  total   → InPatientBill::total
  type    → InPatientBill::type
```

Call `b->total()`: load vptr from object → index into vtable → jump to function.

### A2.3 Virtual Destructor

Required so that `delete b;` (where b is Bill*) calls the most-derived destructor, releasing all resources correctly. Without it, only ~Bill runs → undefined behaviour / leaks.

### Predict the Output (written before running)

```
500 ~OPBill ~Bill 
```

Actual matches. `show()` is non-virtual but calls virtual `total()`, so OPBill::total is reached via the vtable of the dynamic type.

Removing `virtual` from ~Bill: only ~Bill runs (or sanitizer detects incomplete destruction).

## TASK A3 – DESIGN WITH ABSTRACT CLASSES

### A3.1 Payer Interface

```
<<abstract>> Payer
+ amountCovered(const Bill&) : double  [pure virtual]
+ name() : string                      [virtual]

SelfPay : amountCovered = bill.total()
Insurance(limit, coPay%) : covers min(limit, total*(1-coPay))
GovernmentScheme(packageRate) : covers min(total, packageRate)
```

### A3.2 Extension

Add `class CorporateTieUp : public Payer { ... };`  
No existing class is modified. Client code that holds `Payer*` continues to work.

### Alternative (function pointers / std::function)

Possible, but data (limit, coPay) must be stored separately (closures or parallel arrays). Virtual approach keeps data and behaviour together.

## TASK A4 – DESIGN THE EXCEPTION STRATEGY

### A4.1 Error Hierarchy

```
std::exception
  └── BillingError : runtime_error
        ├── NegativeChargeError
        ├── ClaimError
        ├── UnknownPackageError
        └── MissingPatientError
std::bad_alloc  (allocation failure – already standard)
std::invalid_argument (parsing)
```

### A4.2 Specifications

- Constructors and total() may throw (validation).
- Destructors, move operations, `print()` (if possible) → `noexcept`.
- Destructors must not throw: during stack unwinding a second exception calls `std::terminate`.

## TASK A5 – DESIGN RECOVERY AND RETHROWING

### A5.1 Catch Strategy

- Low-level: catch specific errors, log, rethrow with `throw;`
- Mid-level: translate or enrich
- Top-level (main / UI): catch, report user-friendly message, continue or exit cleanly.

### A5.2 RAII & Stack Unwinding

When exception is thrown, stack frames are destroyed in reverse order of construction; destructors of automatic objects run.  
`std::unique_ptr`, `std::vector`, file streams, etc. release resources automatically. No need for `delete` inside every catch.

### Predict the Output (before running)

```
~f ~main caught 42
```

"not printed" never appears because exception leaves the try block before that statement. Destructors run during unwinding.

## Evidence

All designs follow Open-Closed Principle, support runtime polymorphism, and guarantee resource safety via RAII + virtual destructors.  
