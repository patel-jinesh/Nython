#include "types/types.h"

string PyString::toString() {
    return string(value, len);
}

PyString::~PyString() {
    delete[] value;
}