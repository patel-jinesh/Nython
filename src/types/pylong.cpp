#include "types/pylong.h"
#include <iostream>

const int PyLongShift        = 15;
const int PyMarshalShift     = 15;
const int PyMarshalRatio     = PyLongShift / PyMarshalShift;
const int PyMarshalBase      = 1 << PyMarshalShift;
const int PyLongBase         = 1 << PyLongShift;
const int PyLongMask         = PyLongBase - 1;
const int PyLongDecimalShift = 4;     /* max(e such that 10**e fits in a digit) */
const int PyLongDecimalBase  = 10000; /* 10 ** DECIMAL_SHIFT */

// Only supporting PyShift = 15, CPython marshal.c always writes in base 2**15 for portability.
PyLong::PyLong(int32_t size, const int16_t data[]) {
    uint32_t abs = size < 0 ? -size : size;
    this->size   = size;
    this->digits = new int16_t[abs];
    memcpy(digits, data, abs * sizeof(int16_t));
}

PyLong::PyLong(const PyLong & other) {
    uint32_t abs = size < 0 ? -size : size;
    size         = other.size;
    digits       = new int16_t[abs];
    memcpy(digits, other.digits, abs * sizeof(int16_t));
}

PyLong::PyLong(PyLong && other) noexcept {
    digits       = other.digits;
    size         = other.size;
    other.size   = 0;
    other.digits = nullptr;
}

PyLong & PyLong::operator=(const PyLong & other) {
    if (&other == this)
        return *this;

    delete[] digits;
    digits = new int16_t[size < 0 ? -size : size];
    size   = other.size;

    return *this;
}

PyLong & PyLong::operator=(PyLong && other) noexcept {
    if (&other == this)
        return *this;

    delete[] digits;
    digits       = other.digits;
    other.digits = nullptr;

    size       = other.size;
    other.size = 0;

    return *this;
}

PyLong::~PyLong() {
    delete[] digits;
}

// Construct the PyLong by converting value to base 2**15.
// this is used only for TYPE_INT64 (8 bytes), thus we only
// need 5 shorts to represent the value in base 2**15
PyLong::PyLong(int64_t value) {
    uint64_t abs_val = value < 0 ? 0U - (uint64_t) value : (uint64_t) value;
    int32_t  sign    = value == 0 ? 0 : (value < 0 ? -1 : 1);

    uint16_t   data[5];
    uint16_t * p = data;

    while (abs_val) {
        size++;
        *p++ = abs_val & PyLongMask;
        abs_val >>= PyLongShift;
    }

    digits = new int16_t[size];

    memcpy(digits, data, size * sizeof(int16_t));
    size *= sign;  // add sign to size;
}

string PyLong::toString() {
    long  size, strlen, size_a;
    short rem, tenpow;
    int   negative;
    int   d;

    size_a   = this->size < 0 ? -this->size : this->size;
    negative = this->size < 0;

    d    = (33 * PyLongDecimalShift) / (10 * PyLongShift - 33 * PyLongDecimalShift);
    size = 1 + size_a + size_a / d;

    short pout[size];

    size = 0;
    for (int i = size_a; --i >= 0;) {
        short hi = this->digits[i];
        for (int j = 0; j < size; j++) {
            int z   = (int) pout[j] << PyLongShift | hi;
            hi      = (short) (z / PyLongDecimalBase);
            pout[j] = (short) (z - (int) hi * PyLongDecimalBase);
        }
        while (hi) {
            pout[size++] = hi % PyLongDecimalBase;
            hi /= PyLongDecimalBase;
        }
    }

    if (size == 0)
        pout[size++] = 0;

    strlen = negative + 1 + (size - 1) * PyLongDecimalShift;
    tenpow = 10;
    rem    = pout[size - 1];
    while (rem >= tenpow) {
        tenpow *= 10;
        strlen++;
    }

    char out[strlen + 1];
    out[strlen] = 0;
    char * p    = out + strlen;

    for (int i = 0; i < size - 1; i++) {
        rem = pout[i];
        for (int j = 0; j < PyLongDecimalShift; j++) {
            *--p = '0' + rem % 10;
            rem /= 10;
        }
    }
    rem = pout[size - 1];
    do {
        *--p = '0' + rem % 10;
        rem /= 10;
    } while (rem != 0);

    if (negative)
        *--p = '-';

    return string(p);
}

PyLong add(const PyLong & a, const PyLong & b) {
    uint32_t abs_a = a.size < 0 ? -a.size : a.size;
    uint32_t abs_b = b.size < 0 ? -b.size : b.size;
    uint32_t mx    = max(abs_a, abs_b);
    uint32_t mn    = min(abs_a, abs_b);

    int16_t data[mx + 1];

    const PyLong & small = abs_a < abs_b ? a : b;
    const PyLong & big   = abs_a < abs_b ? b : a;

    //can add
    uint16_t carry = 0;

    for (uint32_t i = 0; i < mn; ++i) {
        carry += small.digits[i] + big.digits[i];
        data[i] = carry & PyLongMask;
        carry >>= PyLongShift;
    }
    for (uint32_t i = mn; i < mx; ++i) {
        carry += big.digits[i];
        data[i] = carry & PyLongMask;
        carry >>= PyLongShift;
    }

    data[mx] = carry;

    return PyLong(mx + (carry != 0), data);
}

PyLong sub(const PyLong & a, const PyLong & b) {
    uint8_t  sign   = 1;
    uint16_t borrow = 0;
    uint32_t abs_a  = a.size < 0 ? -a.size : a.size;
    uint32_t abs_b  = b.size < 0 ? -b.size : b.size;
    uint32_t mx     = max(abs_a, abs_b);
    uint32_t mn     = min(abs_a, abs_b);

    const PyLong * small = &b;
    const PyLong * big   = &a;

    if (abs_a < abs_b) {
        sign  = -1;
        small = &a;
        big   = &b;
    } else if (abs_a == abs_b) {
        /* Find highest digit where a and b differ: */
        uint32_t i = abs_a;
        while (--i >= 0 && a.digits[i] == b.digits[i])
            ;
        if (i < 0)
            return PyLong(0L);
        if (a.digits[i] < b.digits[i]) {
            small = &a;
            big   = &b;
        }
        mx = mn = i + 1;
    }

    int16_t data[mx];

    for (int32_t i = 0; i < mn; ++i) {
        /* The following assumes unsigned arithmetic
           works module 2**N for some N>PyLong_SHIFT. */
        borrow  = big->digits[i] - small->digits[i] - borrow;
        data[i] = borrow & PyLongMask;
        borrow >>= PyLongShift;
        borrow &= 1; /* Keep only one sign bit */
    }

    for (int32_t i = mn; i < mx; ++i) {
        borrow  = big->digits[i] - borrow;
        data[i] = borrow & PyLongMask;
        borrow >>= PyLongShift;
        borrow &= 1; /* Keep only one sign bit */
    }

    if (abs_a != abs_b)
        return PyLong(sign < 0 ? -(mx - 1) : (mx - 1), data);  // if size a != size b
    else
        return PyLong(sign < 0 ? -mx : mx, data);  // if size a == size b
}

PyLong operator-(const PyLong & a) {
    return PyLong(-a.size, a.digits);
}

PyLong operator-(const PyLong & a, const PyLong & b) {
    PyLong result(0, nullptr);

    if (a.size < 0) {
        if (b.size < 0)
            result = sub(a, b);
        else
            result = add(a, b);

        result.size = -result.size;
    } else {
        if (b.size < 0)
            result = add(a, b);
        else
            result = sub(a, b);
    }

    return result;
}

PyLong operator+(const PyLong & a, const PyLong & b) {
    PyLong result(0, nullptr);

    if (a.size < 0) {
        if (b.size < 0) {
            result      = add(a, b);
            result.size = -result.size;
        } else
            result = sub(b, a);
    } else {
        if (b.size < 0)
            result = sub(a, b);
        else
            result = add(a, b);
    }

    return result;
}