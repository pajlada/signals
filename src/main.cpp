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
    cout << "# s.invoke(0, 1): Should print nothing" << endl;
    s.invoke(0, 1);
    cout << "\n\n";

    cout << "+ connected X" << endl;
    auto cX = s.connect([](int a, int b) {
        cout << "X " << a << ", " << b << endl;  //
    });

    // Should print:
    // X 1, 2
    cout << "# s.invoke(1, 2): Should print 'X 1, 2'" << endl;
    s.invoke(1, 2);
    cout << "\n\n";

    cout << "+ connected Y" << endl;
    auto cY = s.connect([](int a, int b) {
        cout << "Y " << b << ", " << a << endl;  //
    });

    cout << "# s.invoke(2, 3): Should print 'X 2, 3' and 'Y 3, 2'" << endl;
    s.invoke(2, 3);
    cout << "\n\n";

    cout << "% blocked X" << endl;
    assert(cX.block());   // first block, this should succeed
    assert(!cX.block());  // cX is already blocked, this should fail

    cout << "# s.invoke(2, 3): Should print 'Y 3, 2'" << endl;
    s.invoke(2, 3);
    cout << "\n\n";

    cout << "% unblocked X" << endl;
    assert(cX.unblock());   // cX is currently blocked, this should succeed
    assert(!cX.unblock());  // cX is not blocked, this should fail

    cout << "# s.invoke(2, 3): Should print 'X 2, 3' and 'Y 3, 2'" << endl;
    s.invoke(2, 3);
    cout << "\n\n";

    cout << "# s.invoke(2, 3): Should print 'X 2, 3' and 'Y 3, 2'" << endl;
    s.invoke(2, 3);
    cout << "\n\n";

    cout << "- disconnected X" << endl;
    assert(cX.disconnect());
    assert(!cX.disconnect());

    cout << "# s.invoke(3, 4): Should print 'Y 4, 3'" << endl;
    s.invoke(3, 4);
    cout << "\n\n";

    cout << "- disconnected Y" << endl;
    assert(cY.disconnect());
    assert(!cY.disconnect());

    // Should print nothing
    cout << "# s.invoke(4, 5): Should print nothing" << endl;
    s.invoke(4, 5);
    cout << "\n\n";

    return 0;
}
