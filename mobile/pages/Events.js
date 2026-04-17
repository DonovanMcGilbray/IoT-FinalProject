import React from "react";
import { View, Image, ScrollView } from "react-native";
import { Dimensions } from 'react-native';

export default function Events({ route }) {
  const { event } = route.params;
  const { width } = Dimensions.get('window');

  return (
    <ScrollView>
      {event.frames.map((img, i) => (
        <Image
          key={i}
          source={{ uri: "http://YOUR_PC_IP:5000/static/" + img }}
          style={{ width: width, height: width * 0.75 }}
        />
      ))}
    </ScrollView>
  );
}