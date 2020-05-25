#include "types/pycomplex.h"

PyComplex::PyComplex() {
    this->_real = 0;
    this->_imag = 0;
}

PyComplex::PyComplex(PyFloat real, PyFloat imag) {
    this->_real = real;
    this->_imag = imag;
}