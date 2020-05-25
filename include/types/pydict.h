#pragma once

#include <memory>
#include <unordered_map>
#include "pyobject.h"

using namespace std;

class PyDict : public PyObject {
  private:
    unordered_map<shared_ptr<PyObject>, shared_ptr<PyObject>> items;
    uint32_t                                                  _size;

  public:
    PyDict();
    PyDict(unordered_map<shared_ptr<PyObject>, shared_ptr<PyObject>>);

    shared_ptr<PyObject> & operator[](shared_ptr<PyObject> index);

    const uint32_t & size = _size;
};
