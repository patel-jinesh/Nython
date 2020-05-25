#include "types/pydict.h"

PyDict::PyDict() {
    this->items = unordered_map<shared_ptr<PyObject>, shared_ptr<PyObject>>();
    this->_size = 0;
}

PyDict::PyDict(unordered_map<shared_ptr<PyObject>, shared_ptr<PyObject>> items) {
    this->items = items;
    this->_size = items.size();
}

shared_ptr<PyObject> & PyDict::operator[](shared_ptr<PyObject> index) {
    return items[index];
}