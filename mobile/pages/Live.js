import React from "react";
import { View, StyleSheet, ActivityIndicator } from "react-native";
import { WebView } from "react-native-webview";

export default function Live() {
  // replace with actual ESP32 IP address
  const ESP32_URL = "http://172.20.10.4/"; 

  return (
    <View style={styles.container}>
      <WebView
        source={{ uri: ESP32_URL }}
        scalesPageToFit={true}
        scrollEnabled={false}
        startInLoadingState={true}
        renderLoading={() => (
          <ActivityIndicator size="large" color="#0000ff" style={styles.loader} />
        )}
      />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#000",
  },
  loader: {
    position: "absolute",
    top: "50%",
    left: "50%",
    marginLeft: -20,
    marginTop: -20,
  },
});