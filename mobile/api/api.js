import axios from "axios";

const BASE = "http://YOUR_PC_IP:5000";

export const getEvents = () => axios.get(`${BASE}/events`, {
    headers: { 'X-API-KEY': 'ProjectKey123' }
});
export const sendCommand = (cmd) => axios.get(`${BASE}/command/${cmd}`);

const response = await fetch('http://YOUR_PC_IP:5000/events', {
    method: 'GET',
    headers: {
        'X-API-KEY': 'ProjectKey123',
    },
});