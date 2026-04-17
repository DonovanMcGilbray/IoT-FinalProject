import React from "react";
import { View, Button } from "react-native";
import { sendCommand } from "../api/api";

export default function Control() {
  return (
    <View>
      <Button title="ON" onPress={() => sendCommand("ON")} />
      <Button title="OFF" onPress={() => sendCommand("OFF")} />
    </View>
  );
}