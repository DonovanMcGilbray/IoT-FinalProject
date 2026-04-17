import React, { useEffect, useState } from "react";
import { View, Text, Button, FlatList, TouchableOpacity } from "react-native";
import { getEvents } from "../api/api";

export default function Home({ navigation }) {
  const [events, setEvents] = useState([]);

  useEffect(() => {
    getEvents().then(res => setEvents(res.data));
  }, []);

  return (
    <View>
      <Button title="Control" onPress={() => navigation.navigate("Control")} />
      <Button title="Live View" onPress={() => navigation.navigate("Live")} />

      <FlatList
        data={events}
        keyExtractor={(item) => item.event_id}
        renderItem={({ item }) => (
          <TouchableOpacity
            onPress={() => navigation.navigate("Events", { event: item })}
          >
            <Text>{item.timestamp}</Text>
          </TouchableOpacity>
        )}
      />
    </View>
  );
}