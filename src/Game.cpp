#include "Game.h"

Game::Game(std::string filePath) {
  std::ifstream in(filePath);
  std::string str;
  std::vector<std::string> data;
  while (std::getline(in, str)) {
    data.push_back(str);
  }
  in.close();
  player = Player(data[0]);
  for (int i = 1 ; i < data.size(); i++) {
    rooms.push_back(Room(data[i]));
  }
  state = Play;
}

void Game::save(std::string filePath) {
  std::vector<Item> pInv = player.getInventory();
  std::ofstream out;
  out.open(filePath);

  out <<  "{" << player.getHP() << ATTRIBDELIM << player.getHunger() <<
      ATTRIBDELIM << player.getCurrentRoom() << ATTRIBDELIM << "{";
  if (pInv.size() < 1) {
    out << EMPTY;
  }
  for (int i = 0; i < pInv.size(); i++) {
    for (auto const& mI : ITEMS) {
      if (pInv[i] == mI.second) {
        out << mI.first;
        if (i < pInv.size() - 1) {
          out << OBJDELIM;
        }
        break;
      }
    }
  }
  out << "}}" << std::endl;
  for (Room r : rooms) {
    out << "{" << r.getId() << ATTRIBDELIM << (int)r.locked << ATTRIBDELIM << "{";
    std::vector<Item> rItems = r.getItems();
    if (rItems.size() < 1) {
      out << EMPTY;
    }
    for (int i = 0; i < rItems.size(); i++) {
      for (auto const& I : ITEMS) {
        if (rItems[i] == I.second) {
          out << I.first;
          if (i < rItems.size() - 1) {
            out << OBJDELIM;
          }
          break;
        }
      }
    }
    out << "}" << ATTRIBDELIM;
    out << "{";
    std::vector<NPC> rNPCs = r.getNPCs();
    if (rNPCs.size() < 1) {
      out << EMPTY;
    }
    for (int i = 0; i < rNPCs.size(); i++) {
      for (auto const& N : NPCS) {
        if (rNPCs[i] == N.second) {
          out << N.first;
          if (i < rNPCs.size() - 1) {
            out << OBJDELIM;
          }
          break;
        }
      }
    }
    out << "}}" << std::endl;
  }
  out.close();
  ActionRecord::addRecord("Game saved as " + filePath);
}

Game::Game(int roomCount) {
  double sr = sqrt(roomCount);
  if ((sr - floor(sr)) != 0) {
    throw room_count_not_square_error("The number of rooms must be a square number");
  }
  if (roomCount > 25) {
    throw room_count_too_large_error("Cannot have more than 25 rooms");
  }

  state = Play;

  // Create rooms
  for (int i = 0; i < roomCount; i++) {

    Room newRoom(i, ROOMDESC[i]);
    rooms.push_back(newRoom);

  }


  for (auto const& x : ITEMLOCATIONS) {
    for (int i : x.second) {
      rooms[i].addItem(ITEMS.at(x.first));
    }
    if (ITEMS.find(x.first)->second.type == Key) {
      //If the item is a key, lock the room corresponding to its value
      rooms[ITEMS.find(x.first)->second.value].locked = true;
    }
  }

  //add npc to room
  for (auto const& x : NPCLOCATIONS) {
    for (int i : x.second) {
      rooms[i].addNPC(NPCS.at(x.first));
    }
  }

}

void Game::getInput(std::istream& inStr) {
  int inInt = 0;
  char inChar = ' ';
  std::string inString = "";

  switch (state) {
  case MainMenu:
  case Play:
  case Inventory:
  case Pause:
    inStr >> inChar;
    inChar = std::toupper(inChar);
    break;
  case ItemUse:
  case ItemDrop:
  case ItemExamine:
    inStr >> inInt;
    break;
  case Save:
    inStr >> inString;
    break;
  }

  switch (state) {
  case MainMenu:
  case Play:
    switch (inChar) {
    case INVENTORY:
      if (player.getInventory().size() > 0) {
        state = Inventory;
        ActionRecord::addRecord("You open your inventory.");
      } else {
        ActionRecord::addRecord("Your inventory is empty.");
      }
      break;
    case UP:
    case DOWN:
    case LEFT:
    case RIGHT:
      movePlayer(inChar);
      break;
    case PICK:
      lootRoom();
      break;
    case EXIT:
      state = Pause;
      break;
    }
    break;
  case Inventory:
    switch (inChar) {
    case USE:
      state = ItemUse;
      break;
    case DROP:
      state = ItemDrop;
      break;
    case EXAMINE:
      state = ItemExamine;
      break;
    case EXIT:
      state = Play;
      ActionRecord::addRecord("You close your inventory.");
      break;
    }
    break;
  case ItemUse:
    player.consumeItem(inInt - 1);
    state = Inventory;
    break;
  case ItemDrop:
    player.dropItem(inInt - 1, &rooms[player.getCurrentRoom()]);
    state = Inventory;
    break;
  case ItemExamine:
    player.examineItem(inInt - 1);
    state = Inventory;
    break;
  case Pause:
    switch (inChar) {
    case EXIT:
      state = Play;
      break;
    case QUIT:
    std::cout << std::endl;
      exit(0);
      break;
    case SAVE:
      state = Save;
      break;
    }
    break;
  case Save:
    if(inString != std::string(1, SAVE)) {
      if(!(StringManipulations::hasEnding(inString, FILEEXT))) {
        inString += FILEEXT;
      }
      save(inString);
    }
    state = Pause;
    break;
  }
  inStr.clear();
}

void Game::movePlayer(char dir) {
  int moveInt = 0;
  int currentRoom = player.getCurrentRoom();
  int mapWidth = sqrt(rooms.size());
  bool moved = false;
  std::string dirString;
  switch (std::toupper(dir)) {
  case UP :
    if (currentRoom < mapWidth) {
      ActionRecord::addRecord("You cannot go that way.");
      break;
    }
    dirString = "north";
    moveInt = -mapWidth;
    moved = true;
    break;
  case DOWN :
    if (currentRoom + mapWidth >= rooms.size() ) {
      ActionRecord::addRecord("You cannot go that way.");
      break;
    }
    dirString = "south";
    moveInt = mapWidth;
    moved = true;
    break;
  case LEFT :
    if (currentRoom % mapWidth == 0 ) {
      ActionRecord::addRecord("You cannot go that way.");
      break;
    }
    dirString = "west";
    moveInt = -1;
    moved = true;
    break;
  case RIGHT :
    if ((currentRoom + 1) % mapWidth == 0 ) {
      ActionRecord::addRecord("You cannot go that way.");
      break;
    }
    dirString = "east";
    moveInt = 1;
    moved = true;
    break;
  default :
    return;
    //break;
  }
  if (moved) {
    int roomIndex = currentRoom + moveInt;

    if (rooms[roomIndex].locked) {
      for (Item it : player.getInventory()) {
        if (rooms[roomIndex].tryKey(it)) {
          ActionRecord::addRecord("You use " + it.name + " to unlock the room.");
          player.removeItem(it);
        }
      }
    }
    if (!rooms[roomIndex].locked) {
      player.moveTo(roomIndex);
      ActionRecord::addRecord("You move " + dirString + ".");
    } else {
      ActionRecord::addRecord("That room is locked");
    }
  }
}

void Game::lootRoom() {
  int currentRoom = player.getCurrentRoom();
  std::vector<Item> items = rooms[currentRoom].getItems();
  if (items.size() > 0) {
    for (int i = 0; i < items.size(); i++) {
      player.addItem(items[i]);
    }
    rooms[currentRoom].removeAllItems();
    ActionRecord::addRecord("You loot the room.");
  }
}

void Game::show_ascii(std::string loc) {
  std::ifstream open(loc);
  std::string out;
  if (open.is_open()) {
    while (open.eof() == false) {
      getline(open, out);
      std::cout << out << std::endl;
    }
  }
}

std::vector<std::string> Game::miniMap() {
  int roomCount = rooms.size();
  int s = sqrt(roomCount);

  std::vector<std::string> outVec;
  int currentRoom = player.getCurrentRoom();
  int tempCounter = 0;

  for (int h = 0; h < s; h++) {
    std::stringstream ss;
    for (int w = 0; w < s; w++) {
      if (currentRoom == tempCounter) {
        ss << "[*]";
      } else {
        ss << "[ ]";
      }

      tempCounter++;
    }
    outVec.push_back(ss.str());
  }
  return outVec;
}

std::string Game::getRoomDescription(int r) {
  return rooms[r].getDescriptionOnly();
}

std::vector<std::string> Game::getRoomItemNames() {
  return rooms[player.getCurrentRoom()].getItemList();
}

std::vector<std::string> Game::getRoomNPCNames() {
  return rooms[player.getCurrentRoom()].getNPCList();
}

Game::~Game() {

}
