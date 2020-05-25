#include "types/pyfloat.h"

PyFloat::PyFloat() {
    this->data = 0;
}

PyFloat::PyFloat(double data) {
    this->data = data;
}