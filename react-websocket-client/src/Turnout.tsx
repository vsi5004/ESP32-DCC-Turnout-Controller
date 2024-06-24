import React from 'react';
import {
  Accordion,
  AccordionSummary,
  AccordionDetails,
  TextField,
  Slider,
  Switch,
  Button,
  ButtonGroup,
  Typography,
  Box,
  CircularProgress
} from "@mui/material";
import ExpandMoreIcon from "@mui/icons-material/ExpandMore";
import { TurnoutSetting } from "./types";
import LabeledSwitch from "./LabeledSwitch";

interface TurnoutProps {
  turnout: TurnoutSetting;
  handleChange: (id: number, field: string, value: any) => void;
  sendTurnoutSetting: (setting: TurnoutSetting) => void;
  sendTurnoutTest: (id: number, targetPosition: number) => void;
  isConnected: boolean;
}

export const TURNOUT_MIN_THROW_SPEED = 1;
export const TURNOUT_MAX_THROW_SPEED = 20;
export const TURNOUT_MIN_ENDPOINT = 0;
export const TURNOUT_MAX_ENDPOINT = 180;

const Turnout: React.FC<TurnoutProps> = ({
  turnout,
  handleChange,
  sendTurnoutSetting,
  sendTurnoutTest,
  isConnected
}) => {
  const handleSliderChange = (id: number, values: number[]) => {
    handleChange(id, "openEndpoint", values[0]);
    handleChange(id, "closedEndpoint", values[1]);
  };

  const handleTestThrow = (id: number) => {
    handleChange(id, "testInProgress", true);
    sendTurnoutTest(id, turnout.openEndpoint);
  };

  const handleTestClose = (id: number) => {
    handleChange(id, "testInProgress", true);
    sendTurnoutTest(id, turnout.closedEndpoint);
  };

  return (
    <Accordion key={turnout.id}>
      <AccordionSummary expandIcon={<ExpandMoreIcon />}>
        <Typography>Turnout {turnout.id + 1}</Typography>
      </AccordionSummary>
      <AccordionDetails>
        <Box display="flex" flexDirection="column" gap={2}>
          <TextField
            label="Turnout DCC Address"
            type="number"
            value={turnout.address}
            onChange={(e) => handleChange(turnout.id, "address", parseInt(e.target.value))}
            inputProps={{ min: 1, max: 999 }}
            fullWidth
          />
          <Typography>Endpoint Range</Typography>
          <Slider
            value={[turnout.openEndpoint, turnout.closedEndpoint]}
            onChange={(_e, values) => handleSliderChange(turnout.id, values as number[])}
            step={1}
            min={TURNOUT_MIN_ENDPOINT}
            max={TURNOUT_MAX_ENDPOINT}
            valueLabelDisplay="on"
            marks={[
              { value: TURNOUT_MIN_ENDPOINT + 20, label: turnout.reversed ? 'Closed' : 'Thrown' },
              { value: TURNOUT_MAX_ENDPOINT - 20, label: turnout.reversed ? 'Thrown' : 'Closed' }
            ]}
          />
          <Box display="flex" alignItems="center" gap={1}>
            <Typography>Reversed Operation</Typography>
            <Switch
              checked={turnout.reversed}
              onChange={(e) => handleChange(turnout.id, "reversed", e.target.checked)}
            />
          </Box>
          <Box display="flex" alignItems="center" gap={1}>
            <Typography>Throw Speed</Typography>
            <Slider
              value={turnout.throwSpeed}
              onChange={(_e, value) => handleChange(turnout.id, "throwSpeed", value as number)}
              step={4}
              min={TURNOUT_MIN_THROW_SPEED}
              max={TURNOUT_MAX_THROW_SPEED}
              valueLabelDisplay="on"
              marks={[
                { value: TURNOUT_MIN_THROW_SPEED + 2, label: 'Fast' },
                { value: TURNOUT_MAX_THROW_SPEED - 2, label: 'Slow' }
              ]}
            />
          </Box>
          <Box display="flex" alignItems="center" gap={1}>
            <Typography>Powered Frog</Typography>
            <Switch
              checked={turnout.poweredFrog}
              onChange={(e) => handleChange(turnout.id, "poweredFrog", e.target.checked)}
            />
          </Box>
          {turnout.poweredFrog && (
            <Box display="flex" alignItems="center" gap={1}>
              <Typography>Frog Polarity</Typography>
              <LabeledSwitch
                checked={turnout.reverseFrogPolarity}
                onChange={handleChange}
                id={turnout.id}
                labelOn="Reversed"
                labelOff="Normal"
                name="reverseFrogPolarity"
              />
            </Box>
          )}
          <Box display="flex" alignItems="center" gap={1}>
            <Typography>Starting Position</Typography>
            <LabeledSwitch
              checked={turnout.startClosed}
              onChange={handleChange}
              id={turnout.id}
              labelOn="Closed"
              labelOff="Thrown"
              name="startClosed"
            />
          </Box>
          {turnout.testInProgress ? (
            <Box display="flex" justifyContent="center" alignItems="center">
              <CircularProgress />
            </Box>
          ) : (
            <ButtonGroup variant="contained" fullWidth>
              <Button
                onClick={() => handleTestThrow(turnout.id)}
                disabled={!isConnected}
              >
                {turnout.reversed ? 'Test Close' : 'Test Throw'}
              </Button>
              <Button
                onClick={() => handleTestClose(turnout.id)}
                disabled={!isConnected}
              >
                {turnout.reversed ? 'Test Throw' : 'Test Close'}
              </Button>
            </ButtonGroup>
          )}
          <Button
            variant="contained"
            onClick={() => sendTurnoutSetting(turnout)}
            disabled={!isConnected}
          >
            Save Settings
          </Button>
        </Box>
      </AccordionDetails>
    </Accordion>
  );
};

export default Turnout;
