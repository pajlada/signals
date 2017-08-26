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

    auto cX = s.connect([](int a, int b) {
        cout << "X " << a << ", " << b << endl;  //
    });

    // Should print:
    // X 1, 2
    cout << "# s.invoke(1, 2): Should print 'X 1, 2'" << endl;
    s.invoke(1, 2);
    cout << "\n\n";

    auto cY = s.connect([](int a, int b) {
        cout << "Y " << b << ", " << a << endl;  //
    });

    cout << "# s.invoke(2, 3): Should print 'X 2, 3' and 'Y 3, 2'" << endl;
    s.invoke(2, 3);
    cout << "\n\n";

    cX.block();

    cout << "# s.invoke(2, 3): Should print 'Y 3, 2'" << endl;
    s.invoke(2, 3);
    cout << "\n\n";

    cX.unblock();

    cout << "# s.invoke(2, 3): Should print 'X 2, 3' and 'Y 3, 2'" << endl;
    s.invoke(2, 3);
    cout << "\n\n";

    {
        ScopedBlock blockX(cX);

        cout << "# s.invoke(2, 3): Should print 'Y 3, 2'" << endl;
        s.invoke(2, 3);
        cout << "\n\n";

        {
            ScopedBlock blockY(cY);

            cout << "# s.invoke(4, 5): Should print nothing" << endl;
            s.invoke(4, 5);
            cout << "\n\n";

            {
                ScopedBlock blockX2(cX);

                cout << "# s.invoke(4, 5): Should print nothing" << endl;
                s.invoke(4, 5);
                cout << "\n\n";
            }

            cout << "# s.invoke(4, 5): Should print nothing" << endl;
            s.invoke(4, 5);
            cout << "\n\n";
        }

        cout << "# s.invoke(2, 3): Should print 'Y 3, 2'" << endl;
        s.invoke(2, 3);
        cout << "\n\n";
    }

    cout << "# s.invoke(2, 3): Should print 'X 2, 3' and 'Y 3, 2'" << endl;
    s.invoke(2, 3);
    cout << "\n\n";

    assert(cX.disconnect());
    assert(!cX.disconnect());

    cout << "# s.invoke(3, 4): Should print 'Y 4, 3'" << endl;
    s.invoke(3, 4);
    cout << "\n\n";

    assert(cY.disconnect());
    assert(!cY.disconnect());

    // Should print nothing
    cout << "# s.invoke(4, 5): Should print nothing" << endl;
    s.invoke(4, 5);
    cout << "\n\n";

    NoArgBoltSignal bs;

    // Should print nothing
    bs.invoke();

    bs.connect([]() {
        cout << "only printed once" << endl;  //
    });

    cout << "# bs.invoke(): Should print 'only printed once'" << endl;
    bs.invoke();
    cout << "\n\n";

    cout << "# bs.invoke(): Should print nothing" << endl;
    bs.invoke();
    cout << "\n\n";

    return 0;
}
