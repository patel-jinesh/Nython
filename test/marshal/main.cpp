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

    PyObject * arr[40];

    for (int i = 0; i < 40; i++) {
        arr[i] = read_object(file);
        std::cout << i + 1 << ": ";
        if (arr[i] != NULL) {
            std::cout << "PASS\n";
        } else {
            std::cout << "FAIL\n";
        }
    }

    for (int i = 0; i < 40; i++) {
        std::cout << i << ": " << arr[i]->toString() << "\n";
    }

    fclose(file->fptr);
    return 0;
}