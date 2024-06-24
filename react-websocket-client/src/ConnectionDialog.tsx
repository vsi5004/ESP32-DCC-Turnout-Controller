import React from 'react';
import Dialog from "@mui/material/Dialog";
import DialogTitle from "@mui/material/DialogTitle";
import DialogContent from "@mui/material/DialogContent";
import DialogActions from "@mui/material/DialogActions";
import Typography from "@mui/material/Typography";
import Button from "@mui/material/Button";

interface ConnectionDialogProps {
  open: boolean;
  isConnected: boolean;
  onClose: () => void;
}

const ConnectionDialog: React.FC<ConnectionDialogProps> = ({ open, isConnected, onClose }) => {
  return (
    <Dialog open={open} onClose={onClose}>
      <DialogTitle>Connection Status</DialogTitle>
      <DialogContent>
        <Typography variant="body1">
          {isConnected ? "Active connection to ESP32 Websocket" : "WebSocket is disconnected. Ensure that you are connected to the correct wifi network and refresh the page to reconnect."}
        </Typography>
      </DialogContent>
      <DialogActions>
        <Button onClick={onClose} color="primary">
          Close
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default ConnectionDialog;
