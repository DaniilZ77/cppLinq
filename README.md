# cppLinq
Implemented optimised Linq library in C++ with doctest

Why is it optimized?

- It contains only neccessary invokation of copy constructor of template value
- The same applies to functors
- Most of the cases are tested using doctest
