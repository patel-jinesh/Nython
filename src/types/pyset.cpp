#include "types/types.h"

PySet::PySet(int size) {
    this->size = size;
    data       = vector<shared_ptr<PyObject>>(size);
}

shared_ptr<PyObject> & PySet::operator[](int index) {
    if (index < 0 || index >= size)
        throw "Array index out of bound, exiting";

    return data[index];
}

string PySet::toString() {
    stringstream ss;
    ss << "(";
    for (int i = 0; i < size; i++)
        if (shared_ptr<PyCode> code = dynamic_pointer_cast<PyCode>(data[i]))
            ss << "\n\t\t\t" << regex_replace(code->toString(), std::regex("\n"), "\n\t\t\t") << ",";
        else
            ss << data[i]->toString() + (i == size - 1 ? "" : ", ");
    ss << ")";
    return ss.str();
}