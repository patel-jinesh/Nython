#include "types/types.h"

PyList::PyList(int size)
    : size(size) {
    data = new PyObject *[size];
}

PyList::~PyList() {
    delete[] data;
}

PyObject *& PyList::operator[](int index) {
    if (index < 0 || index >= size)
        throw "Array index out of bound, exiting";

    return data[index];
}

string PyList::toString() {
    stringstream ss;
    ss << "{";
    for (int i = 0; i < size; i++)
        if (PyCode * code = dynamic_cast<PyCode *>(data[i]))
            ss << "\n\t\t\t" << regex_replace(code->toString(), std::regex("\n"), "\n\t\t\t") << ",";
        else
            ss << data[i]->toString() + (i == size - 1 ? "" : ", ");
    ss << "}";
    return ss.str();
}