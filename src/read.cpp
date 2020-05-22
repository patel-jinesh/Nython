
#include "read.h"

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

template<typename T>
inline T * read_object(PYFILE * file) {
    return (T *) read_object(file);
}

inline unsigned char read_byte(PYFILE * file) {
    return *read_bytes(file, 1);
}

inline PyObject * read_object(PYFILE * file) {
    unsigned char type    = read_byte(file);
    bool          isrefed = type & 0x80;

    PyObject * obj = nullptr;

    // clang-format off
    switch (type & ~0x80) {
        case TYPE_NULL:                                                             break;
        case TYPE_FALSE:                    obj = new PyFalse();                    break;
        case TYPE_TRUE:                     obj = new PyTrue();                     break;
        case TYPE_STOPITER:                 obj = new PyStopIteration();            break;
        case TYPE_ELLIPSIS:                 obj = new PyEllipsis();                 break;
        case TYPE_INT64:                    obj = read_pyint64(file);               break; // Only for python2 which is eol. Here for completeness.
        case TYPE_FLOAT:                                                            break; // this only happens when marshal version = 0 or 1., not supporting (for now perhaps)
        case TYPE_BINARY_FLOAT:             obj = read_pybinfloat(file);            break; 
        case TYPE_COMPLEX:                                                          break; // this only happens when marshal version = 0 or 1., not supporting (for now perhaps)
        case TYPE_BINARY_COMPLEX:           obj = read_pybincomplex(file);          break; 
        case TYPE_INT:                      obj = read_pyint(file);                 break; 
        case TYPE_LONG:                     obj = read_pylong(file);                break; 
        case TYPE_TUPLE:                    obj = read_tuple(file);                 break;
        case TYPE_LIST:                     obj = read_list(file);                  break;
        case TYPE_DICT:                     obj = read_dict(file);                  break;
        case TYPE_INTERNED:                 isrefed = true;
        case TYPE_UNICODE:                  obj = nullptr;                          break; // YTI 
        case TYPE_UNKNOWN:                                                          break;
        case TYPE_SET:
        case TYPE_FROZENSET:                obj = read_set(file);                   break;
        case TYPE_ASCII_INTERNED:           isrefed = true;
        case TYPE_ASCII:                    obj = read_ascii(file);                 break;
        case TYPE_CODE:                     obj = read_code(file);                  break;
        case TYPE_SMALL_TUPLE:              obj = read_small_tuple(file);           break;
        case TYPE_SHORT_ASCII_INTERNED:     isrefed = true;
        case TYPE_SHORT_ASCII:              obj = read_short_ascii(file);           break;
        case TYPE_STRING:                   obj = read_string(file);                break;
        case TYPE_REF:                      obj = file->refs[read_int(file) - 1];   break;
        case TYPE_NONE:                     obj = new PyNone();                     break; 
    }
    // clang-format on

    if (isrefed)
        file->refs[file->refs.size()] = obj;

    return obj;
}

inline PyComplex * read_pybincomplex(PYFILE * file) {
    return new PyComplex(*(double *) read_bytes(file, sizeof(double)), *(double *) read_bytes(file, sizeof(double)));
}

inline PyFloat * read_pybinfloat(PYFILE * file) {
    double * value = (double *) read_bytes(file, sizeof(double));
    return new PyFloat(*value);
}

inline PyDict * read_dict(PYFILE * file) {
    PyObject * key;
    PyObject * value;

    PyDict * dict = new PyDict(0, nullptr);

    do {
        key = read_object(file);

        if (key == nullptr)
            break;

        value        = read_object(file);
        (*dict)[key] = value;
    } while (key != NULL && value != NULL);

    return dict;
}

inline PySet * read_set(PYFILE * file) {
    int n = read_int(file);

    PySet * set = new PySet(n);

    for (int i = 0; i < n; i++) {
        (*set)[i] = read_object(file);
    }

    return set;
}

inline PyList * read_list(PYFILE * file) {
    int n = read_int(file);

    PyList * list = new PyList(n);

    for (int i = 0; i < n; i++) {
        (*list)[i] = read_object(file);
    }

    return list;
}

inline PyLong * read_pyint64(PYFILE * file) {
    return new PyLong((long long) read_int64(file));
}

inline PyLong * read_pyint(PYFILE * file) {
    return new PyLong((long) read_int(file));
}

inline PyLong * read_pylong(PYFILE * file) {
    int size = read_int(file);
    return new PyLong(size, (short *) read_bytes(file, (size < 0 ? -size : size) * 2));
}

inline PyString * read_ascii(PYFILE * file) {
    PyString * data = new PyString();
    data->len       = read_int(file);
    data->value     = (const char *) read_bytes(file, data->len);
    return data;
}

inline PyString * read_short_ascii(PYFILE * file) {
    PyString * data = new PyString();
    data->len       = read_byte(file);
    data->value     = (const char *) read_bytes(file, data->len);
    return data;
}

inline PyTuple * read_tuple(PYFILE * file) {
    int n = read_int(file);

    PyTuple * tuple = new PyTuple(n);

    for (int i = 0; i < n; i++) {
        (*tuple)[i] = read_object(file);
    }

    return tuple;
}

inline PyTuple * read_small_tuple(PYFILE * file) {
    int n = read_byte(file);

    PyTuple * tuple = new PyTuple(n);

    for (int i = 0; i < n; i++) {
        (*tuple)[i] = read_object(file);
    }

    return tuple;
}

inline long long read_int64(PYFILE * file) {
    long long data;
    fread(&data, sizeof(data), 1, file->fptr);
    return data;
}

inline int read_int(PYFILE * file) {
    int data;
    fread(&data, sizeof(data), 1, file->fptr);
    return data;
}

inline unsigned char * read_bytes(PYFILE * file, int num) {
    unsigned char * bytes = new unsigned char[num];
    fread(bytes, sizeof(unsigned char), num, file->fptr);
    return bytes;
}

inline PyString * read_string(PYFILE * file) {
    PyString * data = new PyString();
    data->len       = read_int(file);
    data->value     = (const char *) read_bytes(file, data->len);
    return data;
}

inline PyCode * read_code(PYFILE * file) {
    PyCode * code        = new PyCode();
    code->argcount       = read_int(file);
    code->kwonlyargcount = read_int(file);
    code->nlocals        = read_int(file);
    code->stacksize      = read_int(file);
    code->flags          = read_int(file);
    code->instructions   = read_object<PyString>(file);
    code->consts         = read_object<PyTuple>(file);
    code->names          = read_object<PyTuple>(file);
    code->varnames       = read_object<PyTuple>(file);
    code->freevars       = read_object<PyTuple>(file);
    code->cellvars       = read_object<PyTuple>(file);
    code->filename       = read_object<PyString>(file);
    code->name           = read_object<PyString>(file);
    code->firstlineno    = read_int(file);
    code->lnotab         = read_object<PyString>(file);
    return code;
}