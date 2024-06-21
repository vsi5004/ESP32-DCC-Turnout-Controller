import "./App.css";
import { IMessageEvent, w3cwebsocket } from "websocket";
import { useState, useEffect, useRef, useCallback } from "react";
import Button from "@mui/material/Button";
import TurnoutSettings from "./Turnout";

const initialTurnoutSettings = Array.from({ length: 12 }, (_, index) => ({
  id: index,
  address: 1,
  closedEndpoint: 0,
  openEndpoint: 0,
  reversed: false,
}));

function App() {
  const websocket = useRef<w3cwebsocket | null>(null);
  const [LED, setLED] = useState(false);
  const [isConnected, setIsConnected] = useState(false);
  const [turnoutSettings, setTurnoutSettings] = useState(initialTurnoutSettings);

  useEffect(() => {
    websocket.current = new w3cwebsocket("ws://192.168.2.1/ws");

    websocket.current.onopen = () => {
      setIsConnected(true);
      console.log("WebSocket Client Connected");
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
        if (dataFromServer.type === "message") {
          setLED(dataFromServer.LED);
        }
      } catch (error) {
        console.error("Error parsing message", error);
      }
    };

    return () => {
      websocket.current?.close();
    };
  }, []);

  const sendUpdate = useCallback(({ led }: { led: boolean }) => {
    if (websocket.current?.readyState === w3cwebsocket.OPEN) {
      websocket.current.send(
        JSON.stringify({
          type: "message",
          LED: led,
        })
      );
    } else {
      console.error("WebSocket is not open");
    }
  }, []);

  const sendTurnoutSettings = useCallback(() => {
    if (websocket.current?.readyState === w3cwebsocket.OPEN) {
      websocket.current.send(
        JSON.stringify({
          type: "turnoutSettings",
          settings: turnoutSettings,
        })
      );
    } else {
      console.error("WebSocket is not open");
    }
  }, [turnoutSettings]);

  const toggleLed = useCallback(() => sendUpdate({ led: !LED }), [LED, sendUpdate]);

  const handleChange = (id: number, field: string, value: any) => {
    setTurnoutSettings((prevSettings) =>
      prevSettings.map((setting) =>
        setting.id === id ? { ...setting, [field]: value } : setting
      )
    );
  };

  return (
    <div className="centered">
      <div className="wrapper">
        <h1>
          <span>Currently </span>
          <span>{LED ? "ON" : "OFF"}</span>
        </h1>
        <Button variant="contained" onClick={toggleLed} disabled={!isConnected}>
          {LED ? "Turn Off" : "Turn On"}
        </Button>
        <p>{isConnected ? "Connected" : "Disconnected"}</p>
        <TurnoutSettings
          turnoutSettings={turnoutSettings}
          handleChange={handleChange}
          sendTurnoutSettings={sendTurnoutSettings}
          isConnected={isConnected}
        />
      </div>
    </div>
  );
}

export default App;
