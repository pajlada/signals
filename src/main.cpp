#include "pajlada/signals/signal.hpp"

#include <assert.h>
#include <iostream>

using namespace pajlada::Signals;
using namespace std;

int
main(int, char **)
{
    Signal<int, int> s;

    // Should print nothing
    s.invoke(0, 1);

    uint64_t cX = s.connect([](int a, int b) {
        cout << "X " << a << ", " << b << endl;  //
    });

    // Should print:
    // X 1, 2
    s.invoke(1, 2);

    uint64_t cY = s.connect([](int a, int b) {
        cout << "Y " << b << ", " << a << endl;  //
    });

    // Should print:
    // X 2, 3
    // Y 3, 2
    s.invoke(2, 3);

    assert(s.disconnect(cX));
    assert(!s.disconnect(cX));

    // Should print:
    // Y: 4, 3
    s.invoke(3, 4);

    assert(s.disconnect(cY));
    assert(!s.disconnect(cY));

    // Should print nothing
    s.invoke(4, 5);

    NoArgBoltSignal bs;

    // Should print nothing
    bs.invoke();

    bs.connect([]() {
        cout << "only printed once" << endl;
    });

    // Should print "only printed once"
    bs.invoke();

    // Should print nothing
    bs.invoke();

    return 0;
}
