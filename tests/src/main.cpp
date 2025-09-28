#include <gtest/gtest.h>

#include <vector>

// TODO(pajlada): Make tests that try to put connections/signals in various STL containers

int
main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
