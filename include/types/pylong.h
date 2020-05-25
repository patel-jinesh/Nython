#pragma once

#include <string.h>
#include <memory>
#include "pyobject.h"
#include "stdint.h"

using namespace std;

class PyLong : public PyObject {
  private:
    int16_t * digits = nullptr;  //default value
    uint32_t  size   = 0;        //default value

    friend PyLong add(const PyLong & a, const PyLong & b);
    friend PyLong sub(const PyLong & a, const PyLong & b);

    friend PyLong operator-(const PyLong & a);
    friend PyLong operator-(const PyLong & a, const PyLong & b);
    friend PyLong operator+(const PyLong & a, const PyLong & b);

  public:
    PyLong(int32_t size, const int16_t data[]);
    PyLong(int64_t value);

    PyLong(const PyLong & other);
    PyLong(PyLong && other) noexcept;

    PyLong & operator=(PyLong && other) noexcept;
    PyLong & operator=(const PyLong & other);

    ~PyLong();

    string toString();
};