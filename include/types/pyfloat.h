#pragma once

#include "pyobject.h"

class PyFloat : public PyObject {
  private:
    double data;

  public:
    PyFloat();
    PyFloat(double value);
    operator double();
};
