#include "../src/engine/PlayerProcess.hpp"
#include <gtest/gtest.h>
#include <unistd.h>

// ---------------------------------------------------------------------------
// Lifecycle — using /bin/cat as a dummy long-running process
// ---------------------------------------------------------------------------

TEST(PlayerProcessTest, Launch_SetsPidAndIsAlive) {
    PlayerProcess player("/bin/cat");
    player.launch();

    EXPECT_TRUE(player.is_alive());

    player.terminate();
}

TEST(PlayerProcessTest, Terminate_ProcessIsNoLongerAlive) {
    PlayerProcess player("/bin/cat");
    player.launch();
    player.terminate();

    EXPECT_FALSE(player.is_alive());
}

TEST(PlayerProcessTest, IsAlive_ReturnsFalse_BeforeLaunch) {
    PlayerProcess player("/bin/cat");

    EXPECT_FALSE(player.is_alive());
}

TEST(PlayerProcessTest, IsAlive_ReturnsFalse_AfterProcessExitsNaturally) {
    // /bin/true exits immediately with success.
    PlayerProcess player("/bin/true");
    player.launch();

    // Give the process time to exit.
    usleep(50'000);

    EXPECT_FALSE(player.is_alive());
}

TEST(PlayerProcessTest, Terminate_SafeToCallOnAlreadyExitedProcess) {
    PlayerProcess player("/bin/true");
    player.launch();

    usleep(50'000);

    EXPECT_NO_THROW(player.terminate());
}
