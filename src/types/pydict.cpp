#include "types/types.h"

PyDict::PyDict(vector<shared_ptr<PyObject>> items) {
    data = std::unordered_map<shared_ptr<PyObject>, shared_ptr<PyObject>>(items.size());

    for (int i = 0; i < items.size(); i += 2)
        data[items[i]] = items[i + 1];
}

string PyDict::toString() {
    return "Warning: dict nyi";
}

shared_ptr<PyObject> & PyDict::operator[](shared_ptr<PyObject> index) {
    return data[index];
}