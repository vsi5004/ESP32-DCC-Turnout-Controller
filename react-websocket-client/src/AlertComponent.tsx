import React from "react";
import Snackbar from "@mui/material/Snackbar";
import Alert from "@mui/material/Alert";

interface AlertComponentProps {
  alert: { message: string; severity: "success" | "warning" | "info" | "error"; open: boolean };
  handleCloseAlert: () => void;
}

const AlertComponent: React.FC<AlertComponentProps> = ({ alert, handleCloseAlert }) => {
  return (
    <Snackbar
      open={alert.open}
      autoHideDuration={2000}
      onClose={handleCloseAlert}
      anchorOrigin={{ vertical: "top", horizontal: "center" }}
    >
      <Alert onClose={handleCloseAlert} severity={alert.severity} sx={{ width: '100%' }}>
        {alert.message}
      </Alert>
    </Snackbar>
  );
};

export default AlertComponent;
