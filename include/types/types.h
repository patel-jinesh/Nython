#pragma once

#include <stdint.h>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include "pybool.h"
#include "pycode.h"
#include "pycomplex.h"
#include "pydict.h"
#include "pyfloat.h"
#include "pylist.h"
#include "pylong.h"
#include "pynone.h"
#include "pyobject.h"
#include "pyset.h"
#include "pystring.h"
#include "pytuple.h"

// Other empty python object classes temporarily included below.

class PyEllipsis : public PyObject {
};

class PyStopIteration : public PyObject {
};