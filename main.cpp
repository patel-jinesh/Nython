#include "opcodes.h"
#include <iostream>
#include "read.h"
#include "execute.h"

int main() {
    const char * filename = "__pycache__/test.cpython-37.pyc";

    std::cout << sizeof(PyByte) << std::endl;

    PyCode * code = read(filename);

    for (int i = 0; i < code->instructions->len; i++)
        printf("%02x ", (unsigned char) code->instructions->value[i]);
    std::cout << std::endl;

    run(code);

    return 0;
}