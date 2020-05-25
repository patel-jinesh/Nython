#pragma once

#include <stdio.h>
#include <memory>
#include <unordered_map>
#include "types/types.h"

#pragma region Function Declarations

struct PYFILE {
    FILE *                                        fptr;
    std::unordered_map<int, shared_ptr<PyObject>> refs;
};

shared_ptr<PyObject> read_object(PYFILE * file);

// Function to read pyc files.
shared_ptr<PyCode> read(const char * filename);
#pragma endregion