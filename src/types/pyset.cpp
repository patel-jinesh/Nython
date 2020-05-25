#include "types/types.h"

PySet::PySet(vector<shared_ptr<PyObject>> items) {
    this->_size = items.size();
    this->items = items;
}

shared_ptr<PyObject> & PySet::operator[](int index) {
    if (index < 0 || index >= size)
        throw "Array index out of bound, exiting";

    return items[index];
}