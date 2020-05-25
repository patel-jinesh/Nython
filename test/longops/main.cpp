#include <iostream>
#include "read.h"

int main() {
    PyLong a(-9223372036854775807L);
    PyLong b(9223372036854775807L);
    PyLong d(-5, new short[5] {0, 0, 0, 0, 16});

    PyLong c = a - b;

    for (int i = 0; i < abs(a.size); i++) {
        std::cout << a.ob_digit[i] << " ";
    }

    std::cout << std::endl;

    std::cout << d.toString();

    return 0;
}