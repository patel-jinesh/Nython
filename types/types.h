
#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <sstream>
#include <regex>
#include <unordered_map>

using namespace std;

class PyObject {
    public: 
        virtual ~PyObject() {}
        virtual string toString() = 0;
};

class PyFalse : public PyObject {
     public:
        virtual string toString() {
            return "False";
        }
};

class PyTrue : public PyObject {
    public:
        virtual string toString() {
            return "True";
        }
};

class PyEllipsis : public PyObject {
    public:
        virtual string toString() {
            return "...";
        }
};

class PyStopIteration : public PyObject {
    public:
        virtual string toString() {
            return "StopIteration??";
        }
};

class PyNone : public PyObject {
    public:
        string toString();
};

class PyString : public PyObject {
    public:
        int len;
        const char * value;

        string toString();
};

class PyLong : public PyObject {
    public:
        short * ob_digit;
        int size;

        // Only supporting PyShift = 15 for now.
        PyLong(int size, short data[]);

        PyLong(long value);

        string toString();
};

class PyTuple : public PyObject {
    private:
        PyObject ** data;
        int size;

    public:
        PyTuple(int size);

        PyObject * & operator[](int index) {
            if (index < 0 || index >= size)
                throw "Array index out of bound, exiting"; 
            
            return data[index]; 
        }

        ~PyTuple();

        string toString();
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

        string toString();

        ~PyCode();
};

class PySet : public PyObject {
    private:
        int size;
        PyObject ** data;

    public:
        PySet(int size);

        PyObject * & operator[](int index) {
            if (index < 0 || index >= size)
                throw "Array index out of bound, exiting"; 
            
            return data[index]; 
        }

        ~PySet();

        string toString();
};


class PyList : public PyObject {
    private:
        int size;
        PyObject ** data;

    public:
        PyList(int size);

        PyObject * & operator[](int index) {
            if (index < 0 || index >= size)
                throw "Array index out of bound, exiting"; 
            
            return data[index]; 
        }

        ~PyList();

        string toString();
};

class PyDict : public PyObject {
    private:
        int size;
        std::unordered_map<PyObject *, PyObject *> data;

    public:
        PyDict(int size, PyObject * items);

        PyObject * & operator[](PyObject * index) {
            return data[index]; 
        }

        string toString();
};

#endif