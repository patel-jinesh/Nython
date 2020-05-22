#include <iostream>

#include "read.h"

/*
values = (
    True,
    False,
    None,
    - 1,
    0,
    1,
    - 999999999999999999,
    999999999999999999,
    - 1.0,
    0.0,
    1.0,
    1.2e-39,
    1.2e39,
    0 + 0j,
    1 + 1j,
    - 1 + 1j,
    1 + -1j,
    - 1 + -1j,
    [],
    [0],
    [0, 0],
    [1, ''],
    [[]],
    [[0], 0],
    (),
    (0),
    (0, 0),
    (1, ''),
    (()),
    ((0), 0),
    '',
    'Hello',
    {},
    {0},
    frozenset(),
    frozenset([0]),
    dict(),
    {0: 0},
    {0: ''},
    {'': 0},
    '😀'
)
*/

int main() {
    const char * filename = "test/marshal/output.bin";

    PYFILE * file = new PYFILE();
    file->fptr    = fopen(filename, "r");

    const int size = 40;

    shared_ptr<PyObject> arr[size];

    for (int i = 0; i < size; i++) {
        arr[i] = read_object(file);
        std::cout << i + 1 << ": ";
        if (arr[i] != NULL) {
            std::cout << "PASS\n";
        } else {
            std::cout << "FAIL\n";
        }
    }

    for (int i = 0; i < size; i++) {
        std::cout << i << ": " << arr[i]->toString() << "\n";
    }

    for (int i = 0; i < size; i++) {
        std::cout << "Currently deleting: " << i << std::endl;
    }

    fclose(file->fptr);
    delete file;
    return 0;
}