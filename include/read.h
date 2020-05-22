#ifndef READ_H
#define READ_H

#include <stdio.h>

#include <memory>
#include <unordered_map>

#include "types/typecodes.h"
#include "types/types.h"

#pragma region Function Declarations

struct PYFILE {
    FILE *                                        fptr;
    std::unordered_map<int, shared_ptr<PyObject>> refs;
};

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

inline shared_ptr<PyObject> read_object(PYFILE * file);

// Function to read pyc files.
shared_ptr<PyCode> read(const char * filename);
#pragma endregion

#endif