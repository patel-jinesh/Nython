#ifndef READ_H
#define READ_H

#include <stdio.h>

#include <memory>
#include <unordered_map>

#include "types/typecodes.h"
#include "types/types.h"

PyCode * read(const char * filename);
#endif