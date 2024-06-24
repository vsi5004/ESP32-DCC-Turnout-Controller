import React from "react";
import Turnout from "./Turnout";
import { TurnoutSetting } from "./types";

interface TurnoutListProps {
  turnoutSettings: TurnoutSetting[];
  handleChange: (id: number, field: string, value: any) => void;
  sendTurnoutSetting: (setting: TurnoutSetting) => void;
  sendTurnoutTest: (id: number, targetPosition: number) => void;
  isConnected: boolean;
}

const TurnoutList: React.FC<TurnoutListProps> = ({
  turnoutSettings,
  handleChange,
  sendTurnoutSetting,
  sendTurnoutTest,
  isConnected
}) => {
  return (
    <>
      {turnoutSettings.map((turnout) => (
        <Turnout
          key={turnout.id}
          turnout={turnout}
          handleChange={handleChange}
          sendTurnoutSetting={sendTurnoutSetting}
          sendTurnoutTest={sendTurnoutTest}
          isConnected={isConnected}
        />
      ))}
    </>
  );
};

export default TurnoutList;
