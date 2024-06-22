import {
  Accordion,
  AccordionSummary,
  AccordionDetails,
  TextField,
  Slider,
  Switch,
  Button,
  Typography,
  Box,
  CircularProgress
} from "@mui/material";
import ExpandMoreIcon from "@mui/icons-material/ExpandMore";

interface TurnoutSetting {
  id: number;
  address: number;
  closedEndpoint: number;
  openEndpoint: number;
  reversed: boolean;
  testInProgress?: boolean;
}

interface TurnoutProps {
  turnoutSettings: TurnoutSetting[];
  handleChange: (id: number, field: string, value: any) => void;
  sendTurnoutSettings: () => void;
  sendTurnoutTest: (id: number) => void;
  isConnected: boolean;
}

const Turnout: React.FC<TurnoutProps> = ({
  turnoutSettings,
  handleChange,
  sendTurnoutSettings,
  sendTurnoutTest,
  isConnected
}) => {
  const handleSliderChange = (id: number, values: number[]) => {
    handleChange(id, "openEndpoint", values[0]);
    handleChange(id, "closedEndpoint", values[1]);
  };

  const handleTestServoRange = (id: number) => {
    handleChange(id, "testInProgress", true);
    sendTurnoutTest(id);
  };

  return (
    <>
      {turnoutSettings.map((turnout) => (
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
                min={0}
                max={100}
                valueLabelDisplay="on"
              />
              <Box display="flex" alignItems="center" gap={1}>
                <Typography>Reversed Operation</Typography>
                <Switch
                  checked={turnout.reversed}
                  onChange={(e) => handleChange(turnout.id, "reversed", e.target.checked)}
                />
              </Box>
              {turnout.testInProgress ? (
                <Box display="flex" justifyContent="center" alignItems="center">
                  <CircularProgress />
                </Box>
              ) : (
                <Button
                  variant="contained"
                  onClick={() => handleTestServoRange(turnout.id)}
                  disabled={!isConnected}
                >
                  Test Servo Range
                </Button>
              )}
              <Button
                variant="contained"
                onClick={sendTurnoutSettings}
                disabled={!isConnected}
              >
                Save Settings
              </Button>
            </Box>
          </AccordionDetails>
        </Accordion>
      ))}
    </>
  );
};

export default Turnout;
