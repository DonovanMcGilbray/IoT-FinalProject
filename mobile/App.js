import React from "react";
import { NavigationContainer } from "@react-navigation/native";
import { createStackNavigator } from "@react-navigation/stack";

import Home from "./pages/Home";
import Event from "./pages/Event";
import Control from "./pages/Control";
import Live from "./pages/Live";

const Stack = createStackNavigator();

export default function App() {
  return (
    <NavigationContainer>
      <Stack.Navigator>
        <Stack.Screen name="Home" component={Home} />
        <Stack.Screen name="Events" component={Events} />
        <Stack.Screen name="Control" component={Control} />
        <Stack.Screen name="Live" component={Live} />
      </Stack.Navigator>
    </NavigationContainer>
  );
}