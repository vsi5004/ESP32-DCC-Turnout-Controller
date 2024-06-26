import React from 'react';
import Turnout from './Turnout'; // Adjust the path as needed
import { TurnoutSetting, TurnoutSettingValue } from './types';
import Box from '@mui/material/Box';

interface TurnoutListProps {
  turnoutSettings: TurnoutSetting[];
  handleChange: (id: number, field: string, value: TurnoutSettingValue) => void;
  sendTurnoutSetting: (setting: TurnoutSetting) => void;
  sendTurnoutTest: (id: number, targetPosition: number) => void;
  isConnected: boolean;
  expandedAccordion: number | false;
  setExpandedAccordion: React.Dispatch<React.SetStateAction<number | false>>;
}

const TurnoutList: React.FC<TurnoutListProps> = ({
  turnoutSettings,
  handleChange,
  sendTurnoutSetting,
  sendTurnoutTest,
  isConnected,
  expandedAccordion,
  setExpandedAccordion
}) => {
  return (
    <div>
      {turnoutSettings.map((turnout) => (
        <Box key={turnout.id} marginY={2}>
          <Turnout
            key={turnout.id}
            turnout={turnout}
            handleChange={handleChange}
            sendTurnoutSetting={sendTurnoutSetting}
            sendTurnoutTest={sendTurnoutTest}
            isConnected={isConnected}
            expandedAccordion={expandedAccordion}
            setExpandedAccordion={setExpandedAccordion}
          />
        </Box>
      ))}
    </div>
  );
};

export default TurnoutList;
