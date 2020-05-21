#include "types/types.h"

PyTuple::PyTuple(int size)
    : size(size) {
    data = new PyObject *[size];
}

PyTuple::~PyTuple() {
    delete[] data;
}

PyObject *& PyTuple::operator[](int index) {
    if (index < 0 || index >= size)
        throw "Array index out of bound, exiting";

    return data[index];
}

string PyTuple::toString() {
    stringstream ss;
    ss << "(";
    for (int i = 0; i < size; i++)
        if (PyCode * code = dynamic_cast<PyCode *>(data[i]))
            ss << "\n\t\t\t" << regex_replace(code->toString(), std::regex("\n"), "\n\t\t\t") << ",";
        else
            ss << data[i]->toString() + (i == size - 1 ? "" : ", ");
    ss << ")";
    return ss.str();
}