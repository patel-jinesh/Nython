#include "read.h"
#include "types/typecodes.h"

// clang-format off
// Reading native objects:
inline unsigned char   read_byte        (PYFILE * file);
inline unsigned char * read_bytes       (PYFILE * file, int num);
inline int             read_int         (PYFILE * file);
inline long long       read_int64       (PYFILE * file);
inline double          read_double      (PYFILE * file);

// Reading python objects:
inline shared_ptr<PyTuple>       read_small_tuple  (PYFILE * file);
inline shared_ptr<PyTuple>       read_tuple        (PYFILE * file);
inline shared_ptr<PyString>      read_string       (PYFILE * file);
inline shared_ptr<PyCode>        read_code         (PYFILE * file);
inline shared_ptr<PyString>      read_short_ascii  (PYFILE * file);
inline shared_ptr<PyString>      read_ascii        (PYFILE * file);
inline shared_ptr<PyLong>        read_pylong       (PYFILE * file);
inline shared_ptr<PyFloat>       read_pybinfloat   (PYFILE * file);
inline shared_ptr<PyComplex>     read_pybincomplex (PYFILE * file);
inline shared_ptr<PyLong>        read_pyint        (PYFILE * file);
inline shared_ptr<PyLong>        read_pyint64      (PYFILE * file);
inline shared_ptr<PySet>         read_set          (PYFILE * file);
inline shared_ptr<PyList>        read_list         (PYFILE * file);
inline shared_ptr<PyDict>        read_dict         (PYFILE * file);
// clang-format on

shared_ptr<PyCode> read(const char * filename) {
    PYFILE file;

    file.fptr = fopen(filename, "r");

    long magic;
    long mod;

    fread(&magic, sizeof(long), 1, file.fptr);
    fread(&mod, sizeof(long), 1, file.fptr);

    unsigned char type;

    std::unordered_map<int, PyObject *> ref;

    shared_ptr<PyCode> code = read_code(&file);

    fclose(file.fptr);

    return code;
}

template<typename T>
shared_ptr<T> read_object(PYFILE * file) {
    return dynamic_pointer_cast<T>(read_object(file));
}

inline unsigned char read_byte(PYFILE * file) {
    unsigned char byte;
    fread(&byte, sizeof(unsigned char), 1, file->fptr);
    return byte;
}

shared_ptr<PyObject> read_object(PYFILE * file) {
    unsigned char type    = read_byte(file);
    bool          isrefed = type & 0x80;

    shared_ptr<PyObject> obj = nullptr;

    // clang-format off
    switch (type & ~0x80) {
        case TYPE_NULL:                                                             break;
        case TYPE_FALSE:                    obj = make_shared<PyFalse>();           break;
        case TYPE_TRUE:                     obj = make_shared<PyTrue>();            break;
        case TYPE_STOPITER:                 obj = make_shared<PyStopIteration>();   break;
        case TYPE_ELLIPSIS:                 obj = make_shared<PyEllipsis>();        break;
        case TYPE_INT64:                    obj = read_pyint64(file);               break; // Only generated by python2 which is eol. Here for completeness.
        case TYPE_FLOAT:                                                            break; // this only happens when marshal version <= 1., not supporting (for now perhaps)
        case TYPE_BINARY_FLOAT:             obj = read_pybinfloat(file);            break; 
        case TYPE_COMPLEX:                                                          break; // this only happens when marshal version <= 1., not supporting (for now perhaps)
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
        case TYPE_NONE:                     obj = make_shared<PyNone>();            break; 
        case TYPE_REF:                      
            obj = file->refs[read_int(file) - 1];
            break;
    }
    // clang-format on

    if (isrefed) {
        file->refs[file->refs.size()] = obj;
    }

    return obj;
}

shared_ptr<PyComplex> read_pybincomplex(PYFILE * file) {
    return make_shared<PyComplex>(read_double(file), read_double(file));
}

shared_ptr<PyFloat> read_pybinfloat(PYFILE * file) {
    return make_shared<PyFloat>(read_double(file));
}

shared_ptr<PyDict> read_dict(PYFILE * file) {
    unordered_map<shared_ptr<PyObject>, shared_ptr<PyObject>> data;

    do {
        shared_ptr<PyObject> key = read_object(file);

        if (key == nullptr)
            break;

        data[key] = read_object(file);
    } while (true);

    return make_shared<PyDict>(data);
}

shared_ptr<PySet> read_set(PYFILE * file) {
    int                          n = read_int(file);
    vector<shared_ptr<PyObject>> data(n);

    for (int i = 0; i < n; i++)
        data[i] = read_object(file);

    return make_shared<PySet>(data);
}

shared_ptr<PyList> read_list(PYFILE * file) {
    int                          n = read_int(file);
    vector<shared_ptr<PyObject>> data(n);

    for (int i = 0; i < n; i++)
        data[i] = read_object(file);

    return make_shared<PyList>(data);
}

shared_ptr<PyLong> read_pyint64(PYFILE * file) {
    return make_shared<PyLong>((long long) read_int64(file));
}

shared_ptr<PyLong> read_pyint(PYFILE * file) {
    return make_shared<PyLong>((long) read_int(file));
}

shared_ptr<PyLong> read_pylong(PYFILE * file) {
    int size = read_int(file);
    return make_shared<PyLong>(size, (uint16_t *) read_bytes(file, (size < 0 ? -size : size) * 2));
}

shared_ptr<PyString> read_ascii(PYFILE * file) {
    int32_t              size  = read_int(file);
    const char8_t *      value = (const char8_t *) read_bytes(file, size);
    shared_ptr<PyString> data  = make_shared<PyString>(size, value);
    return data;
}

shared_ptr<PyString> read_short_ascii(PYFILE * file) {
    int8_t               size  = read_int(file);
    const char8_t *      value = (const char8_t *) read_bytes(file, size);
    shared_ptr<PyString> data  = make_shared<PyString>(size, value);
    return data;
}

shared_ptr<PyTuple> read_tuple(PYFILE * file) {
    int                          n = read_int(file);
    vector<shared_ptr<PyObject>> data(n);

    for (int i = 0; i < n; i++)
        data[i] = read_object(file);

    return make_shared<PyTuple>(data);
}

shared_ptr<PyTuple> read_small_tuple(PYFILE * file) {
    int                          n = read_byte(file);
    vector<shared_ptr<PyObject>> data(n);

    for (int i = 0; i < n; i++)
        data[i] = read_object(file);

    return make_shared<PyTuple>(data);
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

inline double read_double(PYFILE * file) {
    double data;
    fread(&data, sizeof(data), 1, file->fptr);
    return data;
}

inline unsigned char * read_bytes(PYFILE * file, int num) {
    unsigned char * bytes = new unsigned char[num];
    fread(bytes, sizeof(unsigned char), num, file->fptr);
    return bytes;
}

shared_ptr<PyString> read_string(PYFILE * file) {
    return read_ascii(file);
}

shared_ptr<PyCode> read_code(PYFILE * file) {
    shared_ptr<PyCode> code = make_shared<PyCode>();
    code->argcount          = read_int(file);
    code->kwonlyargcount    = read_int(file);
    code->nlocals           = read_int(file);
    code->stacksize         = read_int(file);
    code->flags             = read_int(file);
    code->instructions      = read_object<PyString>(file);
    code->consts            = read_object<PyTuple>(file);
    code->names             = read_object<PyTuple>(file);
    code->varnames          = read_object<PyTuple>(file);
    code->freevars          = read_object<PyTuple>(file);
    code->cellvars          = read_object<PyTuple>(file);
    code->filename          = read_object<PyString>(file);
    code->name              = read_object<PyString>(file);
    code->firstlineno       = read_int(file);
    code->lnotab            = read_object<PyString>(file);
    return code;
}