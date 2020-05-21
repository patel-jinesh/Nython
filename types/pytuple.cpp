#include "types.h"

PyTuple::PyTuple(int size) : size(size) {
    data = new PyObject * [size];
}

PyTuple::~PyTuple(){
    delete[] data;
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