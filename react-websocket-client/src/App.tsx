import "./App.css";
import { useState, useEffect, useRef, useCallback } from "react";
import Fab from "@mui/material/Fab";
import AddIcon from "@mui/icons-material/Add";
import Snackbar from "@mui/material/Snackbar";
import Alert from "@mui/material/Alert";
import TurnoutList from "./TurnoutList";
import { IMessageEvent, w3cwebsocket } from "websocket";
import { TurnoutSetting } from "./types";
import { TURNOUT_MAX_ENDPOINT, TURNOUT_MAX_THROW_SPEED, TURNOUT_MIN_ENDPOINT, TURNOUT_MIN_THROW_SPEED } from "./Turnout";

function App() {
  const websocket = useRef<w3cwebsocket | null>(null);
  const [isConnected, setIsConnected] = useState(false);
  const [turnoutSettings, setTurnoutSettings] = useState<TurnoutSetting[]>([]);
  const [alert, setAlert] = useState<{ message: string; severity: "success" | "warning" | "info" | "error"; open: boolean }>({
    message: "",
    severity: "success",
    open: false
  });

  useEffect(() => {
    websocket.current = new w3cwebsocket("ws://192.168.2.1/ws");

    websocket.current.onopen = () => {
      setIsConnected(true);
      console.log("WebSocket Client Connected");
      websocket.current?.send(JSON.stringify({ type: "getTurnouts" }));
    };

    websocket.current.onclose = () => {
      setIsConnected(false);
      console.log("WebSocket Client Disconnected");
    };

    websocket.current.onerror = (error) => {
      console.error("WebSocket Error", error);
    };

    websocket.current.onmessage = (message: IMessageEvent) => {
      try {
        const dataFromServer = JSON.parse(message.data.toString());
        if (dataFromServer.type === "turnoutsList") {
          setTurnoutSettings(dataFromServer.turnouts);
          setAlert({ message: "Turnouts list updated", severity: "success", open: true });
        } else if (dataFromServer.type === "turnoutTestComplete") {
          handleTestComplete(dataFromServer.turnoutId);
        }
      } catch (error) {
        console.error("Error parsing message", error);
      }
    };

    return () => {
      websocket.current?.close();
    };
  }, []);

  const sendMessage = (message: object) => {
    if (websocket.current?.readyState === w3cwebsocket.OPEN) {
      websocket.current.send(JSON.stringify(message));
    } else {
      console.error("WebSocket is not open");
    }
  };

  const sendTurnoutSetting = useCallback((setting: TurnoutSetting) => {
    sendMessage({ type: "turnoutSettings", settings: setting });
  }, []);

  const sendTurnoutTest = useCallback((id: number, targetPosition: number) => {
    const setting = turnoutSettings.find((s) => s.id === id);
    if (setting) {
      sendMessage({ type: "turnoutTest", settings: setting, targetPosition });
    }
  }, [turnoutSettings]);

  const handleChange = (id: number, field: string, value: any) => {
    setTurnoutSettings((prevSettings) =>
      prevSettings.map((setting) =>
        setting.id === id ? { ...setting, [field]: value } : setting
      )
    );
  };

  const handleAddTurnout = () => {
    if (turnoutSettings.length < 12) {
      const newTurnout: TurnoutSetting = {
        id: turnoutSettings.length,
        address: turnoutSettings.length > 0 ? turnoutSettings[turnoutSettings.length - 1].address + 1 : 0,
        closedEndpoint: TURNOUT_MIN_ENDPOINT,
        openEndpoint: TURNOUT_MAX_ENDPOINT,
        reversed: false,
        startClosed: true,
        moveInProgress: false,
        throwSpeed: (TURNOUT_MAX_THROW_SPEED + TURNOUT_MIN_THROW_SPEED) / 2,
        poweredFrog: true,
        reverseFrogPolarity: false,
      };
      sendMessage({ type: "turnoutSettings", settings: newTurnout });
    } else {
      setAlert({ message: "Device supports a maximum of 12 turnouts, delete existing turnout before you add another", severity: "warning", open: true });
    }
  };

  const handleCloseAlert = () => {
    setAlert((prevAlert) => ({ ...prevAlert, open: false }));
  };

  const handleTestComplete = (id: number) => {
    setTurnoutSettings((prevSettings) =>
      prevSettings.map((setting) =>
        setting.id === id ? { ...setting, testInProgress: false } : setting
      )
    );
  };

  return (
    <div className="centered">
      <div className="wrapper">
        <p>{isConnected ? "Connected" : "Disconnected"}</p>
        <TurnoutList
          turnoutSettings={turnoutSettings}
          handleChange={handleChange}
          sendTurnoutSetting={sendTurnoutSetting}
          sendTurnoutTest={sendTurnoutTest}
          isConnected={isConnected}
        />
        <Fab
          color="primary"
          aria-label="add"
          onClick={handleAddTurnout}
          style={{ position: "fixed", bottom: 16, right: 16 }}
        >
          <AddIcon />
        </Fab>
        <Snackbar
          open={alert.open}
          autoHideDuration={1500}
          onClose={handleCloseAlert}
          anchorOrigin={{ vertical: "top", horizontal: "center" }}
        >
          <Alert onClose={handleCloseAlert} severity={alert.severity} sx={{ width: '100%' }}>
            {alert.message}
          </Alert>
        </Snackbar>
      </div>
    </div>
  );
}

export default App;
