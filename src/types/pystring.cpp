#include "types/pystring.h"

PyString::PyString(uint32_t len, const char8_t * value) {
    this->len   = len;
    this->value = value;
}

PyString::~PyString() {
    delete[] value;
}