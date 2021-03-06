/*
*Copyright 2019 Fafnir
*/

#include "Player.h"
#include <vector>
#include <string>

Player::Player() {
}

// Input format:
// {HP,Hunger,RoomID{ItemID0;ItemID1;...;ItemIDn},{CompletedRiddleIds}}
Player::Player(std::string inParam) {
  inParam.erase(std::remove(inParam.begin(),
                            inParam.end(), '{'), inParam.end());
  inParam.erase(std::remove(inParam.begin(),
                            inParam.end(), '}'), inParam.end());
  std::vector<std::string> params =
    StringManipulations::Split(inParam,
                               ATTRIBDELIM);
  health = std::stoi(params[0]);
  hunger = std::stoi(params[1]);
  currentRoom = std::stoi(params[2]);

  if (params[3] != EMPTY) {
    std::vector<std::string> invIds = StringManipulations::Split(params[3],
                                      OBJDELIM);
    for (std::string s : invIds) {
      inventory.push_back(ITEMS.at(std::stoi(s)));
    }
  }

  if (params[4] != EMPTY) {
    std::vector<std::string> compRids = StringManipulations::Split(params[4],
                                        OBJDELIM);
    for (std::string s : compRids) {
      completedRiddles.push_back(std::stoi(s));
    }
  }
  dead = false;
}


void Player::moveTo(int i) {
  currentRoom = i;
  updateValues();
}

int Player::getCurrentRoom() {
  return currentRoom;
}

void Player::addItem(Item i) {
  inventory.push_back(i);
}

void Player::updateValues() {
  if (hunger < MAXHUNGER) {
    hunger += HUNGERGAIN;
  } else {
    health -= HEALTHLOSS;
    ActionRecord::addRecord("You are hungry. "
                            "You need to find food or you will die!");
  }

  if (hunger > MAXHUNGER) {
    hunger = MAXHUNGER;
  }

  if (health < 1) {
    ActionRecord::addRecord("You have starved to death.");
    dead = true;
  }
}

bool Player::removeItem(Item it) {
  if (inventory.size() < 1) {
    return false;
  }

  std::vector<Item>::iterator itr;
  itr = std::find(inventory.begin(), inventory.end(), it);

  if (itr != inventory.end()) {
    inventory.erase(itr);
    return true;
  }

  return false;
}

bool Player::removeItem(int it) {
  if (inventory.size() > it && it >= 0) {
    inventory.erase(inventory.begin() + it);
    return true;
  }
  return false;
}

bool Player::consumeItem(Item it) {
  if (inventory.size() < 1) {
    return false;
  }

  for (int i = 0; i < inventory.size(); i++) {
    if (inventory[i] == it) {
      return consumeItem(i);
    }
  }
  return false;
}
bool Player::consumeItem(int i) {
  if (i < 0 || inventory.size() < i) {
    ActionRecord::addRecord("Please enter a number corresponding to the item.");
    return false;
  }

  Item it = inventory[i];
  std::string valStr = std::to_string(it.value);
  switch (it.type) {
  case Food:
    // check if food value is positive or negative
    if (it.value >= 0) {
      if (hunger - it.value >= 0) {
        hunger = hunger - inventory[i].value;
        ActionRecord::addRecord("You eat the " + it.name +
                                ". It reduces your hunger by " + valStr + ".");
      } else {
        hunger = 0;
        ActionRecord::addRecord("You eat the " + it.name + ". You are full.");
      }
    } else {
      // if negative, reduce hunger but also hurt player.
      if (hunger + it.value >= 0) {
        hunger = hunger + inventory[i].value;
        health = health + inventory[i].value;
        ActionRecord::addRecord("You eat the " + it.name +
                                ". It reduces your hunger by " + valStr +
                                ", but it also damages you! " + valStr);
      } else {
        hunger = 0;
        ActionRecord::addRecord("You eat the " + it.name + ". You are full, "
                                + ", but it also damages you! " + valStr);
      }
    }
    inventory.erase(inventory.begin() + i);
    return true;
  case Potion:
    if (health + it.value <= MAXHEALTH) {
      health += it.value;
      if (it.value < 0) {
        ActionRecord::addRecord("You drink the " + it.name +
                                ". It tastes like garbage "
                                "and it hurts you! " + valStr + " health.");
      } else {
        ActionRecord::addRecord("You drink the "
                                + it.name + ". It refills " + valStr +
                                " health.");
      }
    } else {
      health = MAXHEALTH;
      ActionRecord::addRecord("You drink the "
                              + it.name + ". You are fully healed.");
    }
    inventory.erase(inventory.begin() + i);
    return true;
  case Treasure:
    ActionRecord::addRecord("You attempt to eat the " + it.name +
                            ". You choke and die.");
    dead = true;
    return false;
  case Weapon:
    ActionRecord::addRecord("You attempt to eat the " + it.name +
                            ". You choke and die.");
    dead = true;
    return false;
  case Key:
    ActionRecord::addRecord("You attempt to eat the " + it.name +
                            ". You choke and die.");
    dead = true;
    return false;

  case Loot:
    ActionRecord::addRecord("You attempt to eat the " + it.name +
                            ". You choke and die.");
    dead = true;
    return false;
  }
  return false;
}

bool Player::dropItem(int it, Room *room) {
  if (inventory.size() > it && it >= 0) {
    Item item = inventory[it];
    inventory.erase(inventory.begin() + it);
    room->addItem(item);
    ActionRecord::addRecord("You drop the " + item.name +".");
    return true;
  }
  return false;
}

std::string Player::examineItem(int it) {
  if (inventory.size() > it && it >= 0) {
      ActionRecord::addRecord(inventory[it].examine);
  return inventory[it].examine;
  }
  return "That item is not in your inventory.";
}

void Player::finishRiddle(int id) {
  completedRiddles.push_back(id);
}

bool Player::checkRiddle(int id) {
  for (int r : completedRiddles) {
    if (r == id) {
      return true;
    }
  }
  return false;
}

std::vector<Item> Player::getInventory() {
  return inventory;
}

int Player::getHP() {
  return health;
}

int Player::getHunger() {
  return hunger;
}

std::vector<std::string> Player::getInventoryList() {
  std::vector<std::string> outVec;
  for (int i = 0; i < inventory.size(); i++) {
    outVec.push_back(std::to_string(i+1) + ": " + inventory[i].name);
  }
  return outVec;
}

std::vector<int> Player::getCompletedRiddles() {
  return completedRiddles;
}

bool Player::itemInInventory(Item it) {
  std::vector<Item>::iterator itr;
  itr = std::find(inventory.begin(), inventory.end(), it);
  if (itr != inventory.end()) {
    return true;
  }
  return false;
}

Player::~Player() {
}
