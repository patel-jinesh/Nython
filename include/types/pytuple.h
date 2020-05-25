#pragma once

#include <stdint.h>
#include <memory>
#include <vector>
#include "pyobject.h"

using namespace std;

class PyTuple : public PyObject {
  private:
    vector<shared_ptr<PyObject>> items;
    uint32_t                     _size;

  public:
    PyTuple(vector<shared_ptr<PyObject>> items);
    shared_ptr<PyObject> & operator[](int index);
    const uint32_t &       size = _size;
};