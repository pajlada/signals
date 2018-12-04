[![Build Status](https://travis-ci.org/pajlada/signals.svg?branch=master)](https://travis-ci.org/pajlada/signals) [![codecov](https://codecov.io/gh/pajlada/signals/branch/master/graph/badge.svg)](https://codecov.io/gh/pajlada/signals)
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
