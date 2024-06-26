import React from 'react';
import { Box, Typography, Switch } from '@mui/material';

interface LabeledSwitchProps {
  checked: boolean;
  onChange: (id: number, name: string, checked: boolean) => void;
  id: number;
  labelOn: string;
  labelOff: string;
  name: string;
}

const LabeledSwitch: React.FC<LabeledSwitchProps> = ({ checked, onChange, id, labelOn, labelOff, name }) => {
  return (
    <Box display="flex" alignItems="center" gap={1}>
      <Switch
        checked={checked}
        onChange={(e) => onChange(id, name, e.target.checked)}
      />
      <Typography>{checked ? labelOn : labelOff}</Typography>
    </Box>
  );
};

export default LabeledSwitch;
