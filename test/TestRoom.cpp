/*
*Copyright 2019 Fafnir
*/

#include "gtest/gtest.h"
#include "Room.h"
#include "Item.h"
#include "GameConstants.h"

TEST(Room, MakeID) {
  Room r(1, "TEST");
  EXPECT_EQ(r.getId(), 1);
}

TEST(Room, MakeDesc) {
  Room r(1, "TEST");
  EXPECT_EQ(r.getDescriptionOnly(), "TEST");
}

TEST(Room, MakeInParamLock) {
  Room r("{2,1,{NULL},{NULL}, 0}");
  EXPECT_TRUE(r.locked);
}

TEST(Room, MakeInParamItem) {
  Room r("{2,1,{200},{NULL}, 0}");
  Item i = ITEMS.at(200);
  EXPECT_EQ(r.getItems()[0], i);
}

TEST(Room, SetDesc) {
  Room r(0);
  r.setDescription("TEST");
  EXPECT_EQ(r.getDescriptionOnly(), "TEST");
}

TEST(Room, RemAll) {
  Room r("{2,1,{200;200;200;200;200},{NULL}, 0}");
  r.removeAllItems();
  EXPECT_EQ(r.getItems().size(), 0);
}
