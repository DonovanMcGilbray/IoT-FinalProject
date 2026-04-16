from flask import Flask, request, render_template, jsonify, url_for
from pymongo import MongoClient
from datetime import datetime
import os
import paho.mqtt.client as mqtt

app = Flask(__name__)

client = MongoClient("mongodb://localhost:27017/")
db = client["camera_db"]

UPLOAD_FOLDER = "static/images"
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

current_event = None

def on_message(client, userdata, msg):
    global current_event

    message = msg.payload.decode()

    if message == "motion_start":
        current_event = datetime.now().strftime("%Y%m%d_%H%M%S")
    elif message == "motion_stop":
        current_event = None

mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1)
mqtt_client.connect("broker.emqx.io", 1883)
mqtt_client.subscribe("SyDo/cam1/event")
mqtt_client.on_message = on_message
mqtt_client.loop_start()

# upload
@app.route("/upload", methods=["POST"])
def upload():
    global current_event

    if not current_event:
        return {"status": "no event"}, 400
            
    ts = datetime.now().strftime("%H-%M-%S")
    file_name_only = f"{current_event}_{ts}.jpg"
    full_path = os.path.join(UPLOAD_FOLDER, file_name_only)

    with open(full_path, "wb") as f:
        f.write(request.data)

    db_path = f"images/{file_name_only}"

    db.events.update_one(
        {"event_id": current_event},
        {
            "$push": {"frames": db_path},
            "$setOnInsert": {"timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S")}
        },
        upsert=True
    )

    return {"status": "saved"}

# get events
@app.route("/events")
def events():
    return jsonify(list(db.events.find({}, {"_id": 0})))

# dashboard
@app.route("/")
def dashboard():
    logs = list(db.events.find({}, {"_id": 0}))
    return render_template("dashboard.html", logs=logs)

@app.route("/event/<event_id>")
def event_view(event_id):
    event = db.events.find_one({"event_id": event_id}, {"_id": 0})
    return render_template("event.html", event=event)

# command
mqtt_pub = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1)
mqtt_pub.connect("broker.emqx.io", 1883)

@app.route("/command/<cmd>")
def command(cmd):
    mqtt_pub.publish("SyDo/cam1/control", cmd)
    return {"status": "sent"}

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000, debug=True)