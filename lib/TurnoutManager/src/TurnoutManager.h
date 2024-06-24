#ifndef TURNOUT_MANAGER_H
#define TURNOUT_MANAGER_H

#include "Turnout.h"

class TurnoutManager
{
public:
  static constexpr int MAX_TURNOUTS = 12;
  static constexpr const char *TYPE_TURNOUTS_LIST = "turnoutsList";
  static constexpr const char *TYPE_GET_TURNOUTS = "getTurnouts";
  static constexpr const char *TYPE_TURNOUT_TEST = "turnoutTest";
  static constexpr const char *TYPE_TURNOUT_TEST_COMPLETE = "turnoutTestComplete";
  static constexpr const char *TYPE_TURNOUT_SETTINGS = "turnoutSettings";

  TurnoutManager();
  void init();
  void initTurnouts();
  void loadTurnouts();
  void saveTurnouts();
  void updateTurnout(const Turnout &turnout);
  String turnoutsToJson() const;
  void updateTurnoutPositions();
  void setTurnoutPosition(int turnoutId, int targetPosition, bool frogPolarity, int throwSpeed);
  static bool calculateFrogPolarityClosed(const Turnout &turnout, int targetPosition);

  Turnout *turnouts[MAX_TURNOUTS];
  int turnoutCount;

private:
};

#endif // TURNOUT_MANAGER_H
