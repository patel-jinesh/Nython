import marshal

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
    tuple([0]),
    (0, 0),
    (1, ''),
    ((),),
    ((0), 0),
    '',
    'Hello',
    set(),
    {0},
    frozenset(),
    frozenset([0]),
    dict(),
    {0: 0},
    {0: ''},
    {'': 0},
    '😀'
)

with open("output.bin", 'wb') as f:
    for value in values:
        marshal.dump(value, f)
