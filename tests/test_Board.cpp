#include "../src/engine/Board.h"
#include "../src/shared/Constants.h"
#include <gtest/gtest.h>

static Projectile makeProjectile(float x, float y) {
  Projectile p;
  p.id = 0;
  p.x = x;
  p.y = y;
  p.vel_x = 1.0f;
  p.vel_y = 0.0f;
  return p;
}

// --- isOutOfBounds ---

TEST(BoardTest, IsOutOfBounds_LeftWall) {
  EXPECT_TRUE(Board::isOutOfBounds(makeProjectile(0.0f, TOWER_Y)));
}

TEST(BoardTest, IsOutOfBounds_RightWall) {
  EXPECT_TRUE(Board::isOutOfBounds(makeProjectile(BOARD_WIDTH, TOWER_Y)));
}

TEST(BoardTest, IsOutOfBounds_BottomWall) {
  EXPECT_TRUE(Board::isOutOfBounds(makeProjectile(BOARD_WIDTH / 2.0f, 0.0f)));
}

TEST(BoardTest, IsOutOfBounds_TopWall) {
  EXPECT_TRUE(
      Board::isOutOfBounds(makeProjectile(BOARD_WIDTH / 2.0f, BOARD_HEIGHT)));
}

TEST(BoardTest, IsOutOfBounds_ValidPosition) {
  EXPECT_FALSE(
      Board::isOutOfBounds(makeProjectile(BOARD_WIDTH / 2.0f, TOWER_Y)));
}

// --- hitsPlayerTower ---

TEST(BoardTest, HitsPlayerTower_AtTowerPosition) {
  EXPECT_TRUE(Board::hitsPlayerTower(makeProjectile(PLAYER_TOWER_X, TOWER_Y)));
}

TEST(BoardTest, HitsPlayerTower_BeyondTowerPosition) {
  EXPECT_TRUE(
      Board::hitsPlayerTower(makeProjectile(PLAYER_TOWER_X + 1.0f, TOWER_Y)));
}

TEST(BoardTest, HitsPlayerTower_BeforeTowerPosition) {
  EXPECT_FALSE(
      Board::hitsPlayerTower(makeProjectile(PLAYER_TOWER_X - 1.0f, TOWER_Y)));
}
