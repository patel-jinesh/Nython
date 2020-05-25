#pragma once

#include "pyobject.h"
#include "pystring.h"
#include "pytuple.h"

class PyCode : public PyObject {
  public:
    uint32_t             argcount;
    uint32_t             kwonlyargcount;
    uint32_t             nlocals;
    uint32_t             stacksize;
    uint32_t             flags;
    uint32_t             firstlineno;
    shared_ptr<PyString> instructions;
    shared_ptr<PyTuple>  consts;
    shared_ptr<PyTuple>  names;
    shared_ptr<PyTuple>  varnames;
    shared_ptr<PyTuple>  freevars;
    shared_ptr<PyTuple>  cellvars;
    shared_ptr<PyString> filename;
    shared_ptr<PyString> name;
    shared_ptr<PyString> lnotab;
};
