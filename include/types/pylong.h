#pragma once

#include <string.h>
#include <memory>
#include "pyobject.h"
#include "stdint.h"

using namespace std;

class PyLong : public PyObject {
  private:
    typedef uint16_t digit;
    typedef uint32_t doubledigit;
    typedef int16_t  sdigit;
    typedef int32_t  sdoubledigit;

    digit * digits = nullptr;  //default value
    int32_t size   = 0;        //default value

    friend PyLong add(const PyLong & a, const PyLong & b);
    friend PyLong sub(const PyLong & a, const PyLong & b);
    friend PyLong mul(const PyLong & a, const PyLong & b);
    friend PyLong div(const PyLong & a, const PyLong & b);
    friend PyLong div1(const PyLong & a, digit n);
    friend PyLong kmul(const PyLong & a, const PyLong & b);
    friend digit  v_lshift(digit * z, digit * a, int32_t m, int32_t d);
    friend digit  v_rshift(digit * z, digit * a, int32_t m, int32_t d);

    friend PyLong operator-(const PyLong & a);
    friend PyLong operator-(const PyLong & a, const PyLong & b);
    friend PyLong operator+(const PyLong & a, const PyLong & b);
    friend PyLong operator*(const PyLong & a, const PyLong & b);
    friend PyLong operator/(const PyLong & a, const PyLong & b);

  public:
    PyLong(int32_t size, const digit data[]);
    PyLong(int64_t value);

    PyLong(const PyLong & other);
    PyLong(PyLong && other) noexcept;

    PyLong & operator=(PyLong && other) noexcept;
    PyLong & operator=(const PyLong & other);

    ~PyLong();

    string toString();
};