#include "execute.h"

struct VM {
    PyCode * code;
    std::stack<PyObject *> frame;
    int index;

    public:
        VM(PyCode * code) : index(0) { this->code = code; }
};

void load_name(VM * vm, unsigned char oparg);
void load_const(VM * vm, unsigned char oparg);
void call_function(VM * vm, unsigned char oparg);

void run(PyCode * code) {
    VM vm(code);

    while (vm.index < vm.code->instructions->len) {
        unsigned char opcode = vm.code->instructions->value[vm.index];
        unsigned char oparg = vm.code->instructions->value[vm.index + 1];

        switch (opcode) {
            case LOAD_NAME:
                load_name(&vm, oparg);
                vm.index+=2;
                break;

            case LOAD_CONST:
                load_const(&vm, oparg);
                vm.index+=2;
                break;
            
            case CALL_FUNCTION:
                call_function(&vm, oparg);
                vm.index+=2;
                break;
            
            case POP_TOP:
                vm.frame.pop();
                vm.index+=2;
                break;

            case RETURN_VALUE:
                vm.index+=2;
                break;
        }
    }
}

void load_name(VM * vm, unsigned char oparg) {
    vm->frame.push((*vm->code->names)[oparg]);
}

void load_const(VM * vm, unsigned char oparg) {
    vm->frame.push((*vm->code->consts)[oparg]);
}

void call_function(VM *vm, unsigned char oparg) {
    PyObject * args[oparg];
    
    for (int i = 0; i < oparg; i++) {
        args[i] = vm->frame.top();
        vm->frame.pop();
    }

    PyObject * callable = vm->frame.top();
    vm->frame.pop();

    if (PyString * name = dynamic_cast<PyString *>(callable)) {
        char * string = new char[name->len + 1]();
        memcpy((void *)string, name->value, name->len);
        string[name->len] = 0;

        if (strncmp(string, "print", 5) == 0) {
            for (int i = 0; i < oparg; i++) {
                if (PyString * value = dynamic_cast<PyString *>(args[i])) {
                    char * vp = new char[value->len + 1]();
                    memcpy((void *)vp, value->value, value->len);
                    vp[value->len] = 0;
                    std::cout << vp << " ";
                }
            }

            std::cout << std::endl;

            vm->frame.push(new PyNone());
        }
    }
}