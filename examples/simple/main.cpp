#include <pajlada/signals/signal.hpp>

#include <iostream>

int
main()
{
    int a = 0;
    pajlada::Signals::Signal<int> incrementSignal;
    auto incrementA = [&](int v) {
        printf("Incrementing a (%d) by %d", a, v);
        a += v;
        printf(". A is now %d\n", a);
    };
    auto conn = incrementSignal.connect(incrementA);

    incrementSignal.invoke(1);

    if (a != 1) {
        std::cerr << "error\n";
        return 1;
    }

    incrementSignal.invoke(5);

    if (a != 6) {
        std::cerr << "error\n";
        return 1;
    }

    return 0;
}
