import React, { useEffect, useState } from "react";
import {
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Button,
  TextField,
  Switch,
  FormControlLabel,
  Box
} from "@mui/material";
import { AppSettings } from "../types";

interface SettingsDialogProps {
  open: boolean;
  onClose: () => void;
  onSave: (settings: AppSettings) => void;
  settings: AppSettings;
}

const SettingsDialog: React.FC<SettingsDialogProps> = ({
  open,
  onClose,
  onSave,
  settings
}) => {
  const [localSettings, setLocalSettings] = useState<AppSettings>(settings);
  const [ssidError, setSsidError] = useState<string | null>(null);
  const [nameError, setNameError] = useState<string | null>(null);

  useEffect(() => {
    setLocalSettings(settings);
  }, [settings]);

  const handleChange = (field: keyof AppSettings) => (
    event: React.ChangeEvent<HTMLInputElement>
  ) => {
    const value =
      event.target.type === "checkbox"
        ? event.target.checked
        : event.target.value;

    // Validate SSID field
    if (field === "wifiSSID" && typeof value === 'string') {
      const validSsidRegex = /^[a-zA-Z0-9-_]+$/;
      if (!validSsidRegex.test(value)) {
        setSsidError("SSID can only contain letters, numbers, hyphens, and underscores.");
      } else {
        setSsidError(null);
      }
    }

    // Validate Controller Name field
    if (field === "controllerName" && typeof value === 'string') {
      const validNameRegex = /^[a-zA-Z0-9-_ ]+$/; // Updated to allow spaces
      if (!validNameRegex.test(value)) {
        setNameError("Name can only contain letters, numbers, hyphens, underscores, and spaces.");
      } else {
        setNameError(null);
      }
    }

    setLocalSettings({ ...localSettings, [field]: value });
  };

  const handleSave = () => {
    if (!ssidError && !nameError) {
      onSave(localSettings);
      onClose();
    }
  };

  return (
    <Dialog open={open} onClose={onClose}>
      <DialogTitle>App Settings</DialogTitle>
      <DialogContent>
        <Box display="flex" flexDirection="column" gap={2}>
          <TextField
            label="Controller Name"
            value={localSettings.controllerName}
            onChange={handleChange("controllerName")}
            inputProps={{ maxLength: 32 }}
            fullWidth
            error={!!nameError}
            helperText={nameError}
          />
          <TextField
            label="WiFi SSID"
            value={localSettings.wifiSSID}
            onChange={handleChange("wifiSSID")}
            inputProps={{ maxLength: 32 }}
            fullWidth
            error={!!ssidError}
            helperText={ssidError}
          />
          <FormControlLabel
            control={
              <Switch
                checked={localSettings.darkMode}
                onChange={handleChange("darkMode")}
              />
            }
            label="Dark Mode"
          />
          <FormControlLabel
            control={
              <Switch
                checked={localSettings.autoOpenOn}
                onChange={handleChange("autoOpenOn")}
              />
            }
            label="Auto Open On"
          />
        </Box>
      </DialogContent>
      <DialogActions>
        <Button onClick={onClose}>Cancel</Button>
        <Button onClick={handleSave} variant="contained" color="primary" disabled={!!ssidError || !!nameError}>
          Save
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default SettingsDialog;
