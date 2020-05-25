#pragma once

#include <memory>
#include <vector>
#include "pyobject.h"

using namespace std;

class PyList : public PyObject {
  private:
    vector<shared_ptr<PyObject>> items;
    uint32_t                     _size;

  public:
    PyList(vector<shared_ptr<PyObject>> items);
    shared_ptr<PyObject> & operator[](int index);
    const uint32_t &       size = _size;
};