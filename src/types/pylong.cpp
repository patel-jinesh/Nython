#include <iostream>
#include <memory>

#include "types/types.h"

const int PyLongShift    = 15;
const int PyMarshalShift = 15;
const int PyMarshalRatio = PyLongShift / PyMarshalShift;
const int PyMarshalBase  = 1 << PyMarshalShift;
const int PyLongBase     = 1 << PyLongShift;
const int PyLongMask     = PyLongBase - 1;

// Only supporting PyShift = 15, CPython marshal.c always writes in base 2**15 for portability.
PyLong::PyLong(int size, short data[]) {
    this->size     = size < 0 ? -size : size;
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

    short * data = new short[3];
    short * p    = data;

    while (abs_ival) {
        size++;
        *p++ = abs_ival & PyLongMask;
        abs_ival >>= PyLongShift;
    }

    ob_digit = new short[size]();
    memcpy(this->ob_digit, data, size);
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
    memcpy(this->ob_digit, data, size);
    size *= sign;  // add sign to size;

    delete[] data;
}

string PyLong::toString() {
    stringstream ss;
    for (int i = 0; i < (size < 0 ? -size : size); i++) {
        if (ob_digit)
            ss << ob_digit[i] << " ";
    }
    return ss.str();
}
