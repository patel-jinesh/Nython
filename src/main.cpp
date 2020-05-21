#include <iostream>

#include "read.h"
#include "vm/opcodes.h"

int main() {
    const char * filename = "__pycache__/test.cpython-37.pyc";

    PyCode * code = read(filename);

    std::cout << code->toString();

    //run(code);

    return 0;
}