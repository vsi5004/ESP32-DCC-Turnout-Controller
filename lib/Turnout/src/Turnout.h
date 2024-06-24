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
  bool startClosed;
  bool testInProgress;
  int currentPosition;
  int targetPosition;
  unsigned long lastMoveTime;
  int throwSpeed;
  bool poweredFrog;
  bool reverseFrogPolarity;

  Turnout(int id, int address, int closedEndpoint, int openEndpoint, bool reversed, bool startClosed, bool testInProgress, int throwSpeed, bool poweredFrog, bool reverseFrogPolarity)
      : id(id), address(address), closedEndpoint(closedEndpoint), openEndpoint(openEndpoint), reversed(reversed), testInProgress(testInProgress),
        currentPosition((openEndpoint + closedEndpoint) / 2), targetPosition(currentPosition), lastMoveTime(millis()), throwSpeed(throwSpeed),
        poweredFrog(poweredFrog), reverseFrogPolarity(reverseFrogPolarity), startClosed(startClosed) {};
  String toJson() const;
  static Turnout fromJson(const JsonObject &json);
};

#endif // TURNOUT_H
