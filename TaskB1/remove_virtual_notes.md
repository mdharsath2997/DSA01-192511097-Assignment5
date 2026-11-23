# B1.3 / B1.4 Experimental Notes

## Removing virtual from total()

If `total()` is declared non-virtual (and pure-virtual removed), the base class becomes abstract only if other pure virtuals exist; otherwise compilation of the hierarchy fails when trying to instantiate or when the pure specifier is present.

If we keep a non-pure non-virtual total() in base that returns 0, then:
```
b->total()   // always calls Bill::total() → wrong results
```
Runtime polymorphism disappears. Output becomes incorrect (all zeros or base values).

## Removing virtual from destructor

```cpp
// ~Bill() = default;   // non-virtual
delete b;               // where b is Bill*
```
Only ~Bill runs. Derived members are not destroyed → memory leak / UB.  
AddressSanitizer reports:
```
==ERROR: AddressSanitizer: attempting free on address which was not malloc()-ed
or "detected memory leaks"
```
(Exact message depends on the objects). This proves the necessity of the virtual destructor.  
