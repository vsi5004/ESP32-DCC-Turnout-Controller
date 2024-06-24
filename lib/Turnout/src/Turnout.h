#ifndef TURNOUT_H
#define TURNOUT_H

#include <ArduinoJson.h>

class Turnout
{
public:
  int id;
  int address;
  int closedEndpoint;
  int openEndpoint;
  bool reversed;
  bool testInProgress;
  int currentPosition;
  int targetPosition;
  unsigned long lastMoveTime;
  int throwSpeed;

  Turnout(int id, int address, int closedEndpoint, int openEndpoint, bool reversed, bool testInProgress, int throwSpeed)
      : id(id), address(address), closedEndpoint(closedEndpoint), openEndpoint(openEndpoint), reversed(reversed), testInProgress(testInProgress),
        currentPosition((openEndpoint + closedEndpoint) / 2), targetPosition(currentPosition), lastMoveTime(millis()), throwSpeed(throwSpeed) {};
  String toJson() const;
  static Turnout fromJson(const JsonObject &json);
};

#endif // TURNOUT_H
