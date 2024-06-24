#ifndef TURNOUT_MANAGER_H
#define TURNOUT_MANAGER_H

#include "Turnout.h"

#define MAX_TURNOUTS 12

class TurnoutManager
{
public:
  TurnoutManager();
  void loadTurnouts();
  void saveTurnouts();
  void updateTurnout(const Turnout &turnout);
  String turnoutsToJson() const;
  void initServos();
  void updateServoPositions();
  void setTurnoutPosition(int turnoutId, int targetPosition, int throwSpeed);

  Turnout *turnouts[MAX_TURNOUTS];
  int turnoutCount;

  static constexpr const char *TYPE_TURNOUTS_LIST = "turnoutsList";
  static constexpr const char *TYPE_GET_TURNOUTS = "getTurnouts";
  static constexpr const char *TYPE_TURNOUT_TEST = "turnoutTest";
  static constexpr const char *TYPE_TURNOUT_TEST_COMPLETE = "turnoutTestComplete";
  static constexpr const char *TYPE_TURNOUT_SETTINGS = "turnoutSettings";

private:
};

#endif // TURNOUT_MANAGER_H
