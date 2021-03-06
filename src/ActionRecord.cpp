/*
*Copyright 2019 Fafnir
*/

#include "ActionRecord.h"
#include <vector>
#include <string>

std::vector<std::string> ActionRecord::record;

void ActionRecord::addRecord(std::string text) {
  record.insert(record.begin(), text);
  if (record.size() > MAXRECORDS) {
    record.pop_back();
  }
}


std::string ActionRecord::getLatest() {
  return record[0];
}

std::vector<std::string> ActionRecord::getFullRecord() {
  return record;
}
