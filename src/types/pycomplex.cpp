#include "types/types.h"

PyComplex::PyComplex(double a, double b) {
    this->a = a;
    this->b = b;
}

string PyComplex::toString() {
    return to_string(a) + " + " + to_string(b) + "j";
}