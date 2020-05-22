#include "types/types.h"

PyFloat::PyFloat(double data) {
    this->data = data;
}

string PyFloat::toString() {
    return to_string(data);
}