#include <iostream>
#include "read.h"

int main() {
    PyLong a(-9223372036854775807L);
    PyLong b(9223372036854775807L);

    PyLong c = a - b;

    PyLong d = c;

    std::cout << (c.size < 0 ? "-" : "");

    for (int i = 0; i < abs(c.size); i++) {
        std::cout << c.ob_digit[i] << " ";
    }

    if (c.size == 0)
        std::cout << 0;

    return 0;
}