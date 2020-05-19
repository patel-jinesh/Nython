
#ifndef TYPES_H
#define TYPES_H
typedef unsigned char byte;

class PyObject {
    public: virtual ~PyObject() {}
};

class PyByte : public PyObject {
    public:
        unsigned char value;

        operator byte() const { return value; }
};


class PyString : public PyObject {
    public:
        const char * value;
        int len;
};

class PyLong : public PyObject {
    public:
        int value;

        operator int() const { return value; }
};

class PyTuple : public PyObject {
    private:
        PyObject ** data;
        int items;

    public:
        PyTuple(int items) {
            this->items = items;
            data = new PyObject *[items];
        }

        PyObject*& operator[](int index) {
            if (index >= items) { 
                throw "Array index out of bound, exiting"; 
            } 
            return data[index]; 
        }

        ~PyTuple() {
            delete[] data;
        }
};

class PyNone : public PyObject {

};

class PyCode : public PyObject {
    public:
        int argcount;
        int kwonlyargcount;
        int nlocals;
        int stacksize;
        int flags;
        PyString * instructions;
        PyTuple * consts;
        PyTuple * names;
        PyTuple * varnames;
        PyTuple * freevars;
        PyTuple * cellvars;
        PyString * filename;
        PyString * name;
        int firstlineno;
        PyString * lnotab;
};
#endif