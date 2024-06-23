#ifndef TURNOUT_H
#define TURNOUT_H

#include <ArduinoJson.h>

class Turnout {
public:
  int id;
  int address;
  int closedEndpoint;
  int openEndpoint;
  bool reversed;
  bool testInProgress;

  Turnout(int id, int address, int closedEndpoint, int openEndpoint, bool reversed, bool testInProgress)
    : id(id), address(address), closedEndpoint(closedEndpoint), openEndpoint(openEndpoint), reversed(reversed), testInProgress(testInProgress) {}

  String toJson() const;
  static Turnout fromJson(const JsonObject& json);
};

#endif // TURNOUT_H
