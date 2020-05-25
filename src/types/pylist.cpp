#include "types/pylist.h"

PyList::PyList(vector<shared_ptr<PyObject>> items) {
    this->_size = items.size();
    this->items = items;
}

shared_ptr<PyObject> & PyList::operator[](int index) {
    if (index < 0 || index >= size)
        throw "Array index out of bound, exiting";

    return items[index];
}