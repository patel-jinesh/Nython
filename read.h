#ifndef READ_H
#define READ_H

#include "typecodes.h"
#include <stdio.h>
#include <memory>
#include <unordered_map>
#include "types/types.h"

PyCode * read(const char * filename);
#endif