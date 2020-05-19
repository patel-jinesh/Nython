
#include "read.h"

struct PYFILE {
    FILE * fptr;
    std::unordered_map<int, PyObject *> refs;
};

inline byte read_byte(PYFILE * file);
inline byte * read_bytes(PYFILE * file, int num);
inline PyLong read_long(PYFILE * file);
inline PyTuple * read_small_tuple(PYFILE * file);
inline PyString * read_string(PYFILE * file);
inline PyCode * read_code(PYFILE * file);
inline PyObject * read_object(PYFILE * file);

template <typename T>
inline T * read_object(PYFILE * file) {
    return (T*) read_object(file);
}

inline byte read_byte(PYFILE * file) {
    return *read_bytes(file, 1);
}

PyCode * read(const char * filename) {
    PYFILE file;

    file.fptr = fopen(filename, "r");

    long magic;
    long mod;

    fread(&magic, sizeof(long), 1, file.fptr);
    fread(&mod, sizeof(long), 1, file.fptr);

    unsigned char type;

    std::unordered_map<int, PyObject *> ref;

    PyCode * code = (PyCode *) read_object(&file);

    fclose(file.fptr);

    return code;
}

inline PyObject * read_object(PYFILE * file) {
    PyTuple * t;
    PyString * s;
    switch (read_byte(file) & ~0x80) {
        case TYPE_CODE:
            return read_code(file);
            break;
        case TYPE_SMALL_TUPLE:
            t = read_small_tuple(file);
            file->refs[1] = t;
            return t;
            break;
        case TYPE_SHORT_ASCII_INTERNED:
        case TYPE_SHORT_ASCII:
            s = new PyString();
            s->len = read_byte(file);
            s->value = (const char *) read_bytes(file, s->len);
            return s;
        case TYPE_STRING:
            return read_string(file);
            break;
        case TYPE_REF:
            return file->refs[read_long(file)];
            break;
        case TYPE_NONE:
            return new PyNone();
    }

    return nullptr;
}

inline PyTuple * read_small_tuple(PYFILE * file) {
    int n = read_byte(file);

    PyTuple * tuple = new PyTuple(n);

    for (int i = 0; i < n; i++) {
        (*tuple)[i] = read_object(file);
    }

    return tuple;
}

inline PyLong read_long(PYFILE * file) {
    PyLong data;
    fread(&(data.value), sizeof(data.value), 1, file->fptr);
    return data;
}

inline byte * read_bytes(PYFILE * file, int num) {
    byte * bytes = new byte[num];
    fread(bytes, sizeof(byte), num, file->fptr);
    return bytes;
}

inline PyString * read_string(PYFILE * file) {
    PyString * data = new PyString();
    data->len = read_long(file);
    data->value = (const char *) read_bytes(file, data->len);
    return data;
}

inline PyCode * read_code(PYFILE * file) {
    PyCode * code = new PyCode();
    code->argcount = read_long(file);
    code->kwonlyargcount = read_long(file);
    code->nlocals = read_long(file);
    code->stacksize = read_long(file);
    code->flags = read_long(file);
    code->instructions = read_object<PyString>(file);
    code->consts = read_object<PyTuple>(file);
    code->names = read_object<PyTuple>(file);
    code->varnames = read_object<PyTuple>(file);
    code->freevars = read_object<PyTuple>(file);
    code->cellvars = read_object<PyTuple>(file);
    code->filename = read_object<PyString>(file);
    code->name = read_object<PyString>(file);
    code->firstlineno = read_long(file);
    code->lnotab = read_object<PyString>(file);
    return code;
}