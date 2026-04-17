import axios from "axios";

const BASE = "http://YOUR_PC_IP:5000";

export const getEvents = () => axios.get(`${BASE}/events`);
export const sendCommand = (cmd) => axios.get(`${BASE}/command/${cmd}`);