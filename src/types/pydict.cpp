#include "types/types.h"

PyDict::PyDict(int size, PyObject * items) {
    data = std::unordered_map<PyObject *, PyObject *>(size);

    for (int i = 0; i < size; i++) {
        data[items + 2 * i] = items + 2 * i + 1;
    }
}

string PyDict::toString() {
    return "Warning: dict nyi";
}

PyObject *& PyDict::operator[](PyObject * index) {
    return data[index];
}