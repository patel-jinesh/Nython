
#ifndef TYPES_H
#define TYPES_H

#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>

using namespace std;

class PyObject {
  public:
    virtual string toString() = 0;
};

class PyFalse : public PyObject {
  public:
    string toString() {
        return "False";
    }
};

class PyTrue : public PyObject {
  public:
    string toString() {
        return "True";
    }
};

class PyEllipsis : public PyObject {
  public:
    string toString() {
        return "...";
    }
};

class PyStopIteration : public PyObject {
  public:
    string toString() {
        return "StopIteration??";
    }
};

class PyNone : public PyObject {
  public:
    string toString() {
        return "None";
    }
};

class PyString : public PyObject {
  public:
    int          len;
    const char * value;

    string toString();
    ~PyString();
};

class PyLong : public PyObject {
  public:
    short * ob_digit;
    int     size;

    PyLong(int size, short data[]);

    PyLong(long value);
    PyLong(long long value);

    string toString();
    ~PyLong();
};

class PyTuple : public PyObject {
  private:
    vector<shared_ptr<PyObject>> data;
    int                          size;

  public:
    PyTuple(int size);

    shared_ptr<PyObject> & operator[](int index);

    string toString();
};

class PyCode : public PyObject {
  public:
    int                  argcount;
    int                  kwonlyargcount;
    int                  nlocals;
    int                  stacksize;
    int                  flags;
    shared_ptr<PyString> instructions;
    shared_ptr<PyTuple>  consts;
    shared_ptr<PyTuple>  names;
    shared_ptr<PyTuple>  varnames;
    shared_ptr<PyTuple>  freevars;
    shared_ptr<PyTuple>  cellvars;
    shared_ptr<PyString> filename;
    shared_ptr<PyString> name;
    int                  firstlineno;
    shared_ptr<PyString> lnotab;

    string toString();
};

class PySet : public PyObject {
  private:
    int                          size;
    vector<shared_ptr<PyObject>> data;

  public:
    PySet(int size);

    shared_ptr<PyObject> & operator[](int index);

    string toString();
};

class PyList : public PyObject {
  private:
    int                          size;
    vector<shared_ptr<PyObject>> data;

  public:
    PyList(int size);

    shared_ptr<PyObject> & operator[](int index);

    string toString();
};

class PyDict : public PyObject {
  private:
    std::unordered_map<shared_ptr<PyObject>, shared_ptr<PyObject>> data;

  public:
    PyDict(vector<shared_ptr<PyObject>> items);

    shared_ptr<PyObject> & operator[](shared_ptr<PyObject> index);

    string toString();
};

class PyFloat : public PyObject {
  public:
    double data;
    PyFloat(double value);

    string toString();
};

class PyComplex : public PyObject {
  public:
    double a;
    double b;

    PyComplex(double a, double b);
    string toString();
};

#endif