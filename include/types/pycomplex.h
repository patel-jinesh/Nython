#pragma once

#include "pyfloat.h"
#include "pyobject.h"

class PyComplex : public PyObject {
  private:
    PyFloat _real;
    PyFloat _imag;

  public:
    PyComplex();
    PyComplex(PyFloat real, PyFloat imag);
    const PyFloat & real = _real;
    const PyFloat & imag = _imag;
};