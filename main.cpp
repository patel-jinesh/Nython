#include "opcodes.h"
#include <iostream>
#include "read.h"

int main() {
    const char * filename = "__pycache__/test.cpython-37.pyc";

    std::cout << sizeof(PyByte) << std::endl;

    PyCode * code = read(filename);

    std::cout << code->toString();

    //run(code);

    return 0;
}