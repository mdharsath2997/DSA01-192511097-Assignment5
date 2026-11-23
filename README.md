# DSA01-192511097-Assignment5

**Student Name:** Mohammed Harsath N  
**Register Number:** 192511097  
**Course:** DSA0120 - OBJECT ORIENTED PROGRAMMING WITH C++  
**Faculty:** Dr Mary Agnes  
**Assignment:** ASSIGNMENT 5 – CO5 : RELIABLE SOFTWARE WITH RUNTIME POLYMORPHISM AND EXCEPTIONS  

## Compiler and Build

- Compiler: g++ (GCC) 13.x or compatible
- Standard: C++17
- Flags: `-std=c++17 -Wall -Wextra`
- For sanitizer tests: add `-fsanitize=address`
- For performance: add `-O2`

Example compile command:
```bash
g++ -std=c++17 -Wall -Wextra -o program source.cpp
./program
```

## Folder Structure

- **TaskA1/** : Design notes for polymorphic Bill hierarchy (see docs/ or report)
- **TaskA2/** : Runtime binding prediction program + explanation
- **TaskA3/** : Payer interface design
- **TaskA4/** : Exception strategy
- **TaskA5/** : Recovery, rethrow, RAII prediction
- **TaskB1/** : Full polymorphic billing implementation + Complete-the-Code
- **TaskB2/** : Error engineering + Complete-the-Code for claims
- **TaskB3/** : Performance comparison (virtual vs switch vs exceptions)
- **TaskB4/** : Extension demonstration and final decision notes
- **docs/** : Additional design documents, UML text representations

## How to Run Each Program

### TaskB1 - Bill Hierarchy (Complete the Code)
```bash
cd TaskB1
g++ -std=c++17 -Wall -Wextra bill_hierarchy.cpp -o bill_hierarchy
./bill_hierarchy
```
Expected:
```
OP Ravi: 1700
IP Meena: 47500
Total 49200
```

### TaskB1 - Full Billing System
```bash
g++ -std=c++17 -Wall -Wextra full_billing.cpp -o full_billing
./full_billing
```

### TaskB2 - Claim Approve (Complete the Code)
```bash
cd TaskB2
g++ -std=c++17 -Wall -Wextra claim_approve.cpp -o claim_approve
./claim_approve
```
Expected:
```
approved 40000
audit: exceeds cover
claim rejected: exceeds cover
error: negative amount
```

### TaskA2 / TaskA5 / TaskB2 Predictions
Compile and run the predict_*.cpp files similarly. Predictions were written before execution and match actual output.

### TaskB3 - Performance
```bash
cd TaskB3
g++ -std=c++17 -O2 -Wall -Wextra performance.cpp -o performance
./performance
```

## Results Summary

- All Complete-the-Code programs produce the exact expected output.
- Virtual functions correctly dispatch at runtime.
- Virtual destructor ensures proper cleanup (verified conceptually; address sanitizer clean when used).
- Exceptions are caught, logged, and rethrown correctly; RAII guarantees no leaks.
- Performance: Switch-based dispatch is faster for homogeneous processing; virtual has modest overhead but superior extensibility. Exceptions have high cost on the error path (especially high invalid rates) but zero/low cost on the happy path and cleaner code.
- Extension: New bill types (DayCare, Emergency) and payer (CorporateTieUp) added without modifying existing classes.

## Notes

- Progressive commits would be made in a real Git workflow (one per task).
- UML diagrams, vtable illustrations and detailed design reasoning are provided in the PDF report and in docs/.
- Address sanitizer should be used with `-fsanitize=address` for leak/destructor checks.

**GitHub Repository:** (to be filled after upload)  
