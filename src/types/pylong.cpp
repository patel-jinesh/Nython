#include "types/pylong.h"
#include <iostream>

const uint32_t PyLongShift        = 15;
const uint32_t PyMarshalShift     = 15;
const uint32_t PyMarshalRatio     = PyLongShift / PyMarshalShift;
const uint32_t PyMarshalBase      = 1 << PyMarshalShift;
const uint32_t PyLongBase         = 1 << PyLongShift;
const uint32_t PyLongMask         = PyLongBase - 1;
const uint32_t PyLongDecimalShift = 4;     /* max(e such that 10**e fits in a digit) */
const uint32_t PyLongDecimalBase  = 10000; /* 10 ** DECIMAL_SHIFT */

// Only supporting PyShift = 15, CPython marshal.c always writes in base 2**15 for portability.
PyLong::PyLong(int32_t size, const digit data[]) {
    uint32_t abs = size < 0 ? -size : size;
    this->size   = size;
    this->digits = new digit[abs];
    memcpy(digits, data, abs * sizeof(digit));
}

PyLong::PyLong(const PyLong & other) {
    uint32_t abs = size < 0 ? -size : size;
    size         = other.size;
    digits       = new digit[abs];
    memcpy(digits, other.digits, abs * sizeof(digit));
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
    digits = new digit[size < 0 ? -size : size];
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

    digits = new digit[size];

    memcpy(digits, data, size * sizeof(digit));
    size *= sign;  // add sign to size;
}

string PyLong::toString() {
    uint64_t size_a   = this->size < 0 ? -this->size : this->size;
    uint32_t negative = this->size < 0;

    int32_t  d    = (33 * PyLongDecimalShift) / (10 * PyLongShift - 33 * PyLongDecimalShift);
    uint32_t size = 1 + size_a + size_a / d;

    uint16_t pout[size];

    size = 0;
    for (int32_t i = size_a - 1; i >= 0; --i) {
        short hi = digits[i];
        for (int32_t j = 0; j < size; j++) {
            int32_t z = (int32_t) pout[j] << PyLongShift | hi;
            hi        = (int16_t)(z / PyLongDecimalBase);
            pout[j]   = (int16_t)(z - (int32_t) hi * PyLongDecimalBase);
        }
        while (hi) {
            pout[size++] = hi % PyLongDecimalBase;
            hi /= PyLongDecimalBase;
        }
    }

    if (size == 0)
        pout[size++] = 0;

    int32_t strlen = negative + 1 + (size - 1) * PyLongDecimalShift;
    int32_t tenpow = 10;
    int32_t rem    = pout[size - 1];

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

    PyLong::digit data[mx + 1];

    const PyLong & small = abs_a < abs_b ? a : b;
    const PyLong & big   = abs_a < abs_b ? b : a;

    PyLong::digit carry = 0;

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

    PyLong::digit data[mx];

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

PyLong mul(const PyLong & a, const PyLong & b) {
    int32_t size_a = a.size < 0 ? -a.size : a.size;
    int32_t size_b = b.size < 0 ? -b.size : b.size;

    PyLong::digit data[size_a + size_b];

    memset(data, 0, (size_a + size_b) * sizeof(PyLong::digit));

    for (int i = 0; i < size_a; ++i) {
        PyLong::doubledigit carry = 0;
        PyLong::doubledigit f     = a.digits[i];
        PyLong::digit *     pz    = data + i;
        PyLong::digit *     pb    = b.digits;
        PyLong::digit *     pbend = b.digits + size_b;

        while (pb < pbend) {
            carry += *pz + *pb++ * f;
            *pz++ = (PyLong::digit)(carry & PyLongMask);
            carry >>= PyLongShift;
        }

        if (carry)
            *pz += (PyLong::digit)(carry & PyLongMask);
    }

    return PyLong(size_a + size_b, data);
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

PyLong operator*(const PyLong & a, const PyLong & b) {
    PyLong result = kmul(a, b);
    /* Negate if exactly one of the inputs is negative. */
    if ((a.size ^ b.size) < 0)
        result = -result;

    return result;
}

const int KARATSUBA_CUTOFF = 70;

PyLong kmul(const PyLong & a, const PyLong & b) {
    int32_t        size_a = a.size < 0 ? -a.size : a.size;
    int32_t        size_b = b.size < 0 ? -b.size : b.size;
    const PyLong & mn     = size_a < size_b ? a : b;
    const PyLong & mx     = size_a < size_b ? b : a;

    if (mn.size == 0)
        return PyLong(0);
    if (mn.size <= KARATSUBA_CUTOFF) {
        return mul(a, b);
    }

    int64_t shift  = mn.size >> 1;
    int64_t left_a = a.size - shift;
    int64_t left_b = b.size - shift;

    const PyLong ah = PyLong(shift, a.digits);
    const PyLong al = PyLong(left_a, a.digits + shift);
    const PyLong bh = PyLong(shift, b.digits);
    const PyLong bl = PyLong(left_b, b.digits + shift);

    const PyLong & t1 = kmul(al, bl);
    const PyLong & t2 = kmul((al + bh), (ah + bl)) * PyLong(32768);  // this is just a shift by << 1 digit, TODO
    const PyLong & t3 = kmul(ah, bh) * PyLong(1073741824);           // this is just a shift by << 2 digits, TODO

    return t3 + t2 + t1;
}

PyLong::digit v_lshift(PyLong::digit * z, PyLong::digit * a, int32_t m, int32_t d) {
    PyLong::digit carry = 0;

    for (int32_t i = 0; i < m; i++) {
        PyLong::doubledigit acc = (PyLong::doubledigit) a[i] << d | carry;
        z[i]                    = (PyLong::digit) acc & PyLongMask;
        carry                   = (PyLong::digit)(acc >> PyLongShift);
    }

    return carry;
}

PyLong::digit v_rshift(PyLong::digit * z, PyLong::digit * a, int32_t m, int32_t d) {
    PyLong::digit carry = 0;
    PyLong::digit mask  = ((PyLong::digit) 1 << d) - 1U;

    for (int32_t i = m; i-- > 0;) {
        PyLong::doubledigit acc = (PyLong::doubledigit) carry << PyLongShift | a[i];
        carry                   = (PyLong::digit) acc & mask;
        z[i]                    = (PyLong::digit)(acc >> d);
    }

    return carry;
}

/* Unsigned int division with remainder -- the algorithm.  The arguments v1
   and w1 should satisfy 2 <= abs(b.size) <= abs(a.size). */

PyLong div(const PyLong & a, const PyLong & b) {
    int32_t        size_a = a.size < 0 ? -a.size : a.size;
    int32_t        size_b = b.size < 0 ? -b.size : b.size;
    const PyLong & mn     = size_a < size_b ? a : b;
    const PyLong & mx     = size_a < size_b ? b : a;

    int32_t (*msb)(int16_t) = [](int16_t value) -> int32_t {
        int32_t bittable[] = {
            // clang-format off
            0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
            5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
            // clang-format on
        };

        int32_t pos = 0;

        while (value >= 32) {
            pos += 6;
            value >> 6;
        }

        return pos + bittable[value];
    };

    /* normalize: shift w1 left so that its top digit is >= PyLong_BASE/2.
       shift v1 left by the same amount.  Results go into w and v. */
    int32_t d = PyLongShift - msb(b.digits[size_b - 1]);

    PyLong::digit data_a[size_a + 1];
    PyLong::digit data_b[size_b];

    PyLong::digit carry = v_lshift(data_b, b.digits, size_b, d);
    carry               = v_lshift(data_a, a.digits, size_a, d);
    if (carry != 0 || a.digits[size_a - 1] >= b.digits[size_b - 1]) {
        a.digits[size_a] = carry;
        size_a++;
    }

    /* Now v->ob_digit[size_v-1] < w->ob_digit[size_w-1], so quotient has
       at most (and usually exactly) k = size_v - size_w digits. */
    int32_t       k = size_a - size_b;
    PyLong::digit data_res[size_a - size_b];

    PyLong::digit * v0  = a.digits;
    PyLong::digit * w0  = b.digits;
    PyLong::digit   wm1 = w0[size_b - 1];
    PyLong::digit   wm2 = w0[size_b - 2];

    for (PyLong::digit *vk = v0 + k, *ak = data_res + k; vk-- > v0;) {
        /* inner loop: divide vk[0:size_w+1] by w0[0:size_w], giving
           single-digit quotient q, remainder in vk[0:size_w]. */

        /* estimate quotient digit q; may overestimate by 1 (rare) */
        PyLong::digit       vtop = vk[size_b];
        PyLong::doubledigit vv   = ((PyLong::doubledigit) vtop << PyLongShift) | vk[size_b - 1];
        PyLong::digit       q    = (PyLong::digit)(vv / wm1);
        PyLong::digit       r    = (PyLong::digit)(vv - (PyLong::doubledigit) wm1 * q); /* r = vv % wm1 */

        while ((PyLong::doubledigit) wm2 * q > (((PyLong::doubledigit) r << PyLongShift) | vk[size_b - 2])) {
            --q;
            r += wm1;
            if (r >= PyLongBase)
                break;
        }

        /* subtract q*w0[0:size_w] from vk[0:size_w+1] */
        PyLong::sdigit zhi = 0;
        for (int32_t i = 0; i < size_b; ++i) {
            /* invariants: -PyLong_BASE <= -q <= zhi <= 0;
               -PyLong_BASE * q <= z < PyLong_BASE */
            PyLong::sdigit z = (PyLong::sdigit) vk[i] + zhi - (PyLong::sdoubledigit) q * (PyLong::sdoubledigit) w0[i];
            vk[i]            = (PyLong::digit) z & PyLongMask;
            zhi              = (PyLong::sdigit) z >> PyLongShift;  // this is diff
        }

        /* add w back if q was too large (this branch taken rarely) */
        if ((PyLong::sdigit) vtop + zhi < 0) {
            carry = 0;
            for (int32_t i = 0; i < size_b; ++i) {
                carry += vk[i] + w0[i];
                vk[i] = carry & PyLongMask;
                carry >>= PyLongShift;
            }
            --q;
        }

        /* store quotient digit */
        *--ak = q;
    }

    /* unshift remainder; we reuse w to store the result */
    carry = v_rshift(w0, v0, size_b, d);

    //*prem = long_normalize(w);
    return PyLong(size_a - size_b, data_res);
}

PyLong div1(const PyLong & a, PyLong::digit n) {
    int32_t       size = a.size < 0 ? -a.size : a.size;
    PyLong::digit data[size];

    PyLong::doubledigit rem = 0;

    PyLong::digit * pin  = a.digits;
    PyLong::digit * pout = data;

    pin += size;
    pout += size;

    for (int32_t i = size; --i >= 0;) {
        PyLong::digit hi;
        rem     = (rem << PyLongShift) | *--pin;
        *--pout = hi = (PyLong::digit)(rem / n);
        rem -= (PyLong::doubledigit) hi * n;
    }

    return PyLong(size, data);
}

PyLong operator/(const PyLong & a, const PyLong & b) {
    int32_t size_a = a.size < 0 ? -a.size : a.size;
    int32_t size_b = b.size < 0 ? -b.size : b.size;

    PyLong z = 0;

    if (size_b == 0) {
        throw "Divide by zero";
    }

    if (size_a < size_b || (size_a == size_b && a.digits[size_a - 1] < b.digits[size_b - 1])) {
        return 0;
    }

    if (size_b == 1) {
        z = div1(a, b.digits[0]);
    } else {
        z = div(a, b);
    }

    if ((a.size < 0) != (b.size < 0)) {
        z = -z;
    }

    return z;
}