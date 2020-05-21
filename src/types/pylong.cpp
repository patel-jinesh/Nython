#include "types/types.h"

const int PyLongShift    = 15;  // or 30, only supporting 15 for now.
const int PyMarshalShift = 15;
const int PyMarshalRatio = PyLongShift / PyMarshalShift;  // is always 1, for now.
const int PyMarshalBase  = 1 << PyMarshalShift;
const int PyLongBase     = 1 << PyLongShift;
const int PyLongMask     = PyLongShift - 1;

// Only supporting PyShift = 15 for now.
PyLong::PyLong(int size, short data[])
    : size(size) {
    ob_digit = new short[size < 0 ? -size : size];

    for (int i = 0; i < (size < 0 ? -size : size); i++)
        ob_digit[i] = data[i];
}

PyLong::PyLong(long value)
    : size(0) {
    unsigned long abs_ival;
    unsigned long t; /* unsigned so >> doesn't propagate sign bit */
    int           ndigits = 0;
    int           sign;

    if (value < 0) {
        /* negate: can't write this as abs_ival = -ival since that
        invokes undefined behaviour when ival is LONG_MIN */
        abs_ival = 0U - (unsigned long) value;
        sign     = -1;
    } else {
        abs_ival = (unsigned long) value;
        sign     = value == 0 ? 0 : 1;
    }

    /* Larger numbers: loop to determine number of digits */
    t = abs_ival;
    while (t) {
        ++size;
        t >>= PyLongShift;
    }

    ob_digit  = new short[ndigits];
    short * p = ob_digit;

    t = abs_ival;
    while (t) {
        *p++ = t & PyLongMask;
        t >>= PyLongShift;
    }
}

string PyLong::toString() {
    return "Warning: long nyi";
}