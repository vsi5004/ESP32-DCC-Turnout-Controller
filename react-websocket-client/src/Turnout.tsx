import {
    Accordion,
    AccordionSummary,
    AccordionDetails,
    TextField,
    Slider,
    Switch,
    Button,
    Typography,
    Box
  } from "@mui/material";
  import ExpandMoreIcon from "@mui/icons-material/ExpandMore";
  
  interface TurnoutSetting {
    id: number;
    address: number;
    closedEndpoint: number;
    openEndpoint: number;
    reversed: boolean;
  }
  
  interface TurnoutSettingsProps {
    turnoutSettings: TurnoutSetting[];
    handleChange: (id: number, field: string, value: any) => void;
    sendTurnoutSettings: () => void;
    isConnected: boolean;
  }
  
  const TurnoutSettings: React.FC<TurnoutSettingsProps> = ({
    turnoutSettings,
    handleChange,
    sendTurnoutSettings,
    isConnected
  }) => {
    const handleSliderChange = (id: number, values: number[]) => {
      handleChange(id, "openEndpoint", values[0]);
      handleChange(id, "closedEndpoint", values[1]);
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
                  label="Turnout Address"
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
                />
                <Box display="flex" alignItems="center" gap={1}>
                  <Typography>Reversed Operation</Typography>
                  <Switch
                    checked={turnout.reversed}
                    onChange={(e) => handleChange(turnout.id, "reversed", e.target.checked)}
                  />
                </Box>
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
  
  export default TurnoutSettings;
  