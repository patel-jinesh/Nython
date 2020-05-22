#ifndef READ_H
#define READ_H

#include <stdio.h>

#include <memory>
#include <unordered_map>

#include "types/typecodes.h"
#include "types/types.h"

#pragma region Function Declarations

struct PYFILE {
    FILE *                              fptr;
    std::unordered_map<int, PyObject *> refs;
};

// clang-format off
// Reading native objects:
inline unsigned char   read_byte        (PYFILE * file);
inline unsigned char * read_bytes       (PYFILE * file, int num);
inline int             read_int         (PYFILE * file);
inline long long       read_int64       (PYFILE * file);

// Reading python objects:
inline PyTuple *       read_small_tuple  (PYFILE * file);
inline PyTuple *       read_tuple        (PYFILE * file);
inline PyString *      read_string       (PYFILE * file);
inline PyCode *        read_code         (PYFILE * file);
inline PyString *      read_short_ascii  (PYFILE * file);
inline PyString *      read_ascii        (PYFILE * file);
inline PyLong *        read_pylong       (PYFILE * file);
inline PyFloat *       read_pybinfloat   (PYFILE * file);
inline PyComplex *     read_pybincomplex (PYFILE * file);
inline PyLong *        read_pyint        (PYFILE * file);
inline PySet *         read_set          (PYFILE * file);
inline PyList *        read_list         (PYFILE * file);
inline PyDict *        read_dict         (PYFILE * file);
// clang-format on

inline PyObject * read_object(PYFILE * file);

// Function to read pyc files.
PyCode * read(const char * filename);
#pragma endregion

#endif