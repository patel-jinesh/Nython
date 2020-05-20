
#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <sstream>
#include <regex>

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
        virtual string toString() {
            return "None";
        }
};

class PyInt64 : public PyObject {
    long value;

    public:
        virtual string toString() {
            return to_string(value);
        }
};

class PyByte : public PyObject {
    public:
        unsigned char value;

        virtual string toString() {
            return to_string(value);
        }
};

class PyString : public PyObject {
    public:
        int len;
        const char * value;

        virtual string toString() {
            return string(value, len);
        }
};

class PyLong : public PyObject {
    private:
        static const int PyLongShift = 15; // or 30, only supporting 15 for now.
        static const int PyMarshalShift = 15;
        static const int PyMarshalRatio = PyLongShift / PyMarshalShift; // is always 1, for now.
        static const int PyMarshalBase = 1 << PyMarshalShift;
        static const int PyLongBase = 1 << PyLongShift;
        static const int PyLongMask = PyLongShift - 1;
    public:
        short * ob_digit;
        int size;

        // Only supporting PyShift = 15 for now.
        PyLong(int size, short data[]) : size(size){
            ob_digit = new short[size < 0 ? -size : size];

            for (int i = 0; i < (size < 0 ? -size : size); i++)
                ob_digit[i] = data[i];
        }

        PyLong(long value) : size(0) {
            unsigned long abs_ival;
            unsigned long t;  /* unsigned so >> doesn't propagate sign bit */
            int ndigits = 0;
            int sign;

            if (value < 0) {
                /* negate: can't write this as abs_ival = -ival since that
                invokes undefined behaviour when ival is LONG_MIN */
                abs_ival = 0U-(unsigned long)value;
                sign = -1;
            } else {
                abs_ival = (unsigned long)value;
                sign = value == 0 ? 0 : 1;
            }

            /* Larger numbers: loop to determine number of digits */
            t = abs_ival;
            while (t) {
                ++size;
                t >>= PyLongShift;
            }

            ob_digit = new short[ndigits];
            short * p = ob_digit;

            t = abs_ival;
            while (t) {
                *p++ = t & PyLongMask;
                t >>= PyLongShift;
            }
        }

        virtual string toString() {
            return "Warning: Long num temp";
        }
};

class PyCode : public PyObject {
    public:
        int argcount;
        int kwonlyargcount;
        int nlocals;
        int stacksize;
        int flags;
        PyString * instructions;
        PyObject * consts;
        PyObject * names;
        PyObject * varnames;
        PyObject * freevars;
        PyObject * cellvars;
        PyString * filename;
        PyString * name;
        int firstlineno;
        PyString * lnotab;

        virtual string toString() {
            stringstream ss;
            ss << "argcount:       " << argcount << "\n";
            ss << "kwonlyargcount: " << kwonlyargcount << "\n";
            ss << "nlocals:        " << nlocals << "\n";
            ss << "stacksize:      " << stacksize << "\n";
            ss << "flags:          " << flags << "\n";
            // TODO: Need to print out hex or actual bytecode;
            ss << "instructions:   " << instructions->toString() << "\n";
            ss << "consts:         " << consts->toString() << "\n";
            ss << "names:          " << names->toString() << "\n";
            ss << "varnames:       " << varnames->toString() << "\n";
            ss << "freevars:       " << freevars->toString() << "\n";
            ss << "cellvars:       " << cellvars->toString() << "\n";
            ss << "filename:       " << filename->toString() << "\n";
            ss << "name:           " << name->toString() << "\n";
            ss << "firstlineno:    " << firstlineno << "\n";
            ss << "lnotab:         " << lnotab->toString() << "\n";
            return ss.str();
        }

        ~PyCode() {
            delete instructions;
            delete consts;
            delete names;
            delete varnames;
            delete freevars;
            delete cellvars;
            delete filename;
            delete name;
            delete lnotab;
        }
};

class PyTuple : public PyObject {
    private:
        PyObject ** data;
        int size;

    public:
        PyTuple(int size) : size(size) {
            data = new PyObject * [size];
        }

        PyObject * & operator[](int index) {
            if (index < 0 || index >= size)
                throw "Array index out of bound, exiting"; 
            
            return data[index]; 
        }

        ~PyTuple(){
            delete[] data;
        }

        string toString() {
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
};

#endif