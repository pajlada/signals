[![codecov](https://codecov.io/gh/pajlada/signals/branch/master/graph/badge.svg)](https://codecov.io/gh/pajlada/signals)
# signals
simple C++ signal library

## Usage
```c++
pajlada::Signals::Signal<int, int> s;

uint64_t cX = s.connect([](int a, int b) {
    cout << "X " << a << ", " << b << endl;  //
});

// prints X 0, 1
s.invoke(0, 1);

s.disconnect(cX);

// prints nothing
s.invoke(1, 2);
    
```

## Development

```sh
# Create build dir
mkdir build
cd build

cmake \
    -DPAJLADA_SIGNALS_BUILD_TESTS=On \
    -DPAJLADA_SIGNALS_BUILD_COVERAGE=On \
    ..

# Build
cmake --build .

# Run tests
ctest

# Generate coverage
make coverage

# Open generated coverage in your browser
firefox tests/coverage/index.html
```
