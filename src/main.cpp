#include <iostream>
#include "read.h"

int main() {
    PyLong a(9223372036854775807L);
    PyLong b(9223372036854775807L);

    PyLong c = a + b + a;

    std::cout << a.toString() << std::endl;
    std::cout << b.toString() << std::endl;
    std::cout << c.toString() << std::endl;

    return 0;
}