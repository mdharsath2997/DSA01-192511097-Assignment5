# TASK B4 – FINAL ENGINEERING DECISION

## B4.1 Practical Extension

Added without modifying any existing class:
- New bill type: `DayCareBill`, `EmergencyBill` (already in full_billing.cpp)
- New payer: `CorporateTieUp`

All previous tests (totals, print via base pointers, exception paths) continue to pass.

## B4.2 Resource Safety

Programs compiled with `-fsanitize=address` show no leaks when exceptions are thrown and caught, because:
- All bills are held in `std::unique_ptr` or deleted via virtual destructor through base pointer.
- Automatic objects (Guards, locals) are destroyed by stack unwinding.

## B4.3 Production Decision

**Decision: Yes – the polymorphic design with exceptions is suitable.**

Reasons:
1. **Extensibility**: Hospital group adds 2 bill types + 3 payers every year. Virtual hierarchy + abstract Payer requires only new derived classes – zero changes to existing code or client loops. Switch-based design would require editing every switch.
2. **Throughput**: 200 000 bills/day is modest. Measured virtual overhead is ~5 µs per million bills (negligible). Exception cost is paid only on the rare error path; with proper validation the invalid rate stays << 1 %.
3. **Safety**: RAII + virtual destructors + catch-and-rethrow give leak-free, auditable error handling – essential for financial data.
4. **Maintainability**: Clear separation of concerns; new staff can add a bill type by following the existing pattern.

**When to prefer alternatives**:
- Extremely hot inner loops with known closed set of types → switch or `std::variant` + `std::visit`.
- Soft real-time with zero allocation / exception overhead → return codes + optional.

For this domain (billing, evolving types, correctness critical) polymorphism + exceptions is the correct engineering choice.

## Alternative – Plug-ins via configuration

Rates could be loaded from JSON/YAML. Interpretation cost replaces virtual call; space for rule tables grows. Risk of runtime misconfiguration vs compile-time type safety. Hybrid (core types as classes, parameters from config) is often best.  
