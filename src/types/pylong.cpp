#include <iostream>
#include <memory>
#include "types/types.h"

const int PyLongShift        = 15;
const int PyMarshalShift     = 15;
const int PyMarshalRatio     = PyLongShift / PyMarshalShift;
const int PyMarshalBase      = 1 << PyMarshalShift;
const int PyLongBase         = 1 << PyLongShift;
const int PyLongMask         = PyLongBase - 1;
const int PyLongDecimalShift = 4;     /* max(e such that 10**e fits in a digit) */
const int PyLongDecimalBase  = 10000; /* 10 ** DECIMAL_SHIFT */

// Only supporting PyShift = 15, CPython marshal.c always writes in base 2**15 for portability.
PyLong::PyLong(int size, short data[]) {
    this->size     = size;
    this->ob_digit = data;
}

PyLong::~PyLong() {
    delete[] ob_digit;
}

// Construct the PyLong by converting value to base 2**15.
// this is used only for TYPE_INT (4 bytes), thus we only
// need 3 shorts to represent the value in base 2**15
PyLong::PyLong(long value) {
    this->size = 0;

    unsigned long abs_ival;
    int           sign;

    if (value < 0) {
        /* negate: can't write this as abs_ival = -ival since that
        invokes undefined behaviour when ival is LONG_MIN */
        abs_ival = 0U - (unsigned long) value;
        sign     = -1;
    } else {
        abs_ival = (unsigned long) value;
        sign     = value != 0;
    }

    short * data = new short[5];
    short * p    = data;

    while (abs_ival) {
        size++;
        *p++ = abs_ival & PyLongMask;
        abs_ival >>= PyLongShift;
    }

    ob_digit = new short[size]();
    memcpy(this->ob_digit, data, size * sizeof(short));
    size *= sign;  // add sign to size;

    delete[] data;
}

// Construct the PyLong by converting value to base 2**15.
// this is used only for TYPE_INT64 (8 bytes), thus we only
// need 5 shorts to represent the value in base 2**15
PyLong::PyLong(long long value) {
    this->size = 0;

    unsigned long long abs_ival;
    int                sign;

    if (value < 0) {
        /* negate: can't write this as abs_ival = -ival since that
        invokes undefined behaviour when ival is LONG_MIN */
        abs_ival = 0U - (unsigned long long) value;
        sign     = -1;
    } else {
        abs_ival = (unsigned long long) value;
        sign     = value != 0;
    }

    short * data = new short[5];
    short * p    = data;

    while (abs_ival) {
        size++;
        *p++ = abs_ival & PyLongMask;
        abs_ival >>= PyLongShift;
    }

    ob_digit = new short[size]();
    memcpy(this->ob_digit, data, size * sizeof(short));
    size *= sign;  // add sign to size;

    delete[] data;
}

string PyLong::toString() {
    long  size, strlen, size_a;
    short rem, tenpow;
    int   negative;
    int   d;

    size_a   = abs(this->size);
    negative = this->size < 0;

    d    = (33 * PyLongDecimalShift) / (10 * PyLongShift - 33 * PyLongDecimalShift);
    size = 1 + size_a + size_a / d;

    short pout[size];

    size = 0;
    for (int i = size_a; --i >= 0;) {
        short hi = this->ob_digit[i];
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
    int mx = max(abs(a.size), abs(b.size));
    int mn = min(abs(a.size), abs(b.size));

    short * data = new short[mx + 1];

    const PyLong & small = abs(a.size) < abs(b.size) ? a : b;
    const PyLong & big   = abs(a.size) < abs(b.size) ? b : a;

    //can add
    short carry = 0;

    for (int i = 0; i < mn; ++i) {
        carry += small.ob_digit[i] + big.ob_digit[i];
        data[i] = carry & PyLongMask;
        carry >>= PyLongShift;
    }
    for (int i = mn; i < mx; ++i) {
        carry += big.ob_digit[i];
        data[i] = carry & PyLongMask;
        carry >>= PyLongShift;
    }

    data[mx] = carry;

    return PyLong(mx + (carry != 0), data);
}

PyLong sub(const PyLong & a, const PyLong & b) {
    int sign   = 1;
    int borrow = 0;
    int mx     = max(abs(a.size), abs(b.size));
    int mn     = min(abs(a.size), abs(b.size));

    PyLong small = b;
    PyLong big   = a;

    if (abs(a.size) < abs(b.size)) {
        sign  = -1;
        small = a;
        big   = b;
    } else if (abs(a.size) == abs(b.size)) {
        /* Find highest digit where a and b differ: */
        int i = abs(big.size);
        while (--i >= 0 && a.ob_digit[i] == b.ob_digit[i])
            ;
        if (i < 0)
            return PyLong(0L);
        if (a.ob_digit[i] < b.ob_digit[i]) {
            small = a;
            big   = b;
        }
        mx = mn = i + 1;
    }

    short * data = new short[mx];

    for (int i = 0; i < mn; ++i) {
        /* The following assumes unsigned arithmetic
           works module 2**N for some N>PyLong_SHIFT. */
        borrow  = a.ob_digit[i] - b.ob_digit[i] - borrow;
        data[i] = borrow & PyLongMask;
        borrow >>= PyLongShift;
        borrow &= 1; /* Keep only one sign bit */
    }
    for (int i = mn; i < mx; ++i) {
        borrow  = a.ob_digit[i] - borrow;
        data[i] = borrow & PyLongMask;
        borrow >>= PyLongShift;
        borrow &= 1; /* Keep only one sign bit */
    }

    //assert(borrow == 0);

    if (abs(a.size) != abs(b.size))
        return PyLong(sign < 0 ? -(mx - 1) : (mx - 1), data);  // if size a != size b
    else
        return PyLong(sign < 0 ? -mx : mx, data);  // if size a == size b
}

PyLong operator-(const PyLong & a) {
    short * data = new short[abs(a.size)];
    memcpy(data, a.ob_digit, abs(a.size) * sizeof(short));
    return PyLong(-a.size, data);
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

PyLong::PyLong(const PyLong & other) {
    this->size     = other.size;
    this->ob_digit = new short[abs(other.size)];
    memcpy(this->ob_digit, other.ob_digit, abs(other.size) * sizeof(short));
}

PyLong::PyLong(PyLong && other) noexcept {
    this->ob_digit = other.ob_digit;
    this->size     = other.size;
    other.ob_digit = nullptr;
}

PyLong & PyLong::operator=(const PyLong & other) {
    return *this = PyLong(other);
}

PyLong & PyLong::operator=(PyLong && other) noexcept {
    short * ob_digit = this->ob_digit;
    this->ob_digit   = other.ob_digit;
    other.ob_digit   = ob_digit;

    int size   = this->size;
    this->size = other.size;
    other.size = size;

    return *this;
}