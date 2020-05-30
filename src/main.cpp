#include <iostream>
#include "read.h"

int main() {
    PyLong a = 1ull << 62;
    PyLong b = 1 << 2;

    std::cout << a.toString() << std::endl;
    std::cout << b.toString() << std::endl;

    PyLong c = a / b;

    std::cout << c.toString() << std::endl;

    return 0;
}