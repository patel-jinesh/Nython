#include "types/types.h"

string PyCode::toString() {
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

PyCode::~PyCode() {
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