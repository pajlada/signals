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
