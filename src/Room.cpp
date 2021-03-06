/*
*Copyright 2019 Fafnir
*/

#include "Room.h"
#include <vector>
#include <string>

Room::Room(int roomId, std::string desc) {
  description = desc;
  id = roomId;
  locked = false;
  visited = false;
}

Room::Room(std::string inParam) {
  inParam.erase(std::remove(inParam.begin(),
                            inParam.end(), '{'), inParam.end());
  inParam.erase(std::remove(inParam.begin(),
                            inParam.end(), '}'), inParam.end());
  std::vector<std::string> params = StringManipulations::Split(inParam,
                                    ATTRIBDELIM);
  id = std::stoi(params[0]);
  locked = static_cast<bool>(std::stoi(params[1]));
  description = ROOMDESC[id];

  if (params[2] != EMPTY) {
    std::vector<std::string> itemIds = StringManipulations::Split(params[2],
                                       OBJDELIM);
    for (std::string s : itemIds) {
      items.push_back(ITEMS.at(std::stoi(s)));
    }
  }
  if (params[3] != EMPTY) {
    std::vector<std::string> npcIds = StringManipulations::Split(params[3],
                                      OBJDELIM);
    for (std::string s : npcIds) {
      npcs.push_back(NPCS.at(std::stoi(s)));
    }
  }
  visited = static_cast<bool>(std::stoi(params[4]));
}


std::string Room::getDescription() {
  std::stringstream ss;

  ss << description << std::endl;

  if (items.size() > 0) {
    ss << "item size: " << items.size() << std::endl;
    ss << "You find the following items on the ground: " << std::endl;
    for (int i = 0; i < items.size(); i++) {
      ss << items[i].name << std::endl;
    }
  }
  ss << "There are " << npcs.size() << " NPCS in the room." << std::endl;
  return ss.str();
}

std::string Room::getDescriptionOnly() {
  return description;
}

void Room::addNPC(NPC npc) {
  npcs.push_back(npc);
}

void Room::addItem(Item item) {
  items.push_back(item);
}

void Room::setDescription(std::string desc) {
  description = desc;
}

std::vector<Item> Room::getItems() {
  return items;
}

void Room::removeAllItems() {
  items.clear();
}

std::string Room::showNPCS() {
  std::stringstream ss;
  ss << "NPCS In Room:" << std::endl;
  for (int i = 0; i < npcs.size(); i++) {
    ss << i+1 << ". " << npcs[i].name << std::endl;
  }
  return ss.str();
}

bool Room::tryKey(Item i) {
  if (i.type == Key && i.value == id) {
    locked = false;
    return true;
  }
  return false;
}

Room::Room(int roomId) {
  id = roomId;
  locked = false;
}

std::vector<std::string> Room::getItemList() {
  std::vector<std::string> retVec;
  for (Item i : items) {
    retVec.push_back(i.name);
  }
  return retVec;
}

std::vector<std::string> Room::getNPCList() {
  std::vector<std::string> retVec;
  for (int i = 0; i < npcs.size(); i++) {
    retVec.push_back(std::to_string(i + 1) + ": " + npcs[i].name);
  }
  return retVec;
}

std::vector<NPC> Room::getNPCS() {
  return npcs;
}


int Room::getId() {
  return id;
}

Room::~Room() {
}
