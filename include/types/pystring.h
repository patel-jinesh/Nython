#pragma once

#include <stdint.h>
#include <memory>
#include "pyobject.h"

using namespace std;

class PyString : public PyObject {
  private:
    uint32_t        len;
    const char8_t * value;

  public:
    PyString(uint32_t len, const char8_t value[]);
    ~PyString();
};
