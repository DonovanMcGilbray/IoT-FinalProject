#include "esp_camera.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include "esp_http_server.h"

// wifi
const char* ssid = "wifi";
const char* password = "";

// mqtt
const char* mqtt_server = "broker.emqx.io";
const char* topic_cmd = "SyDo/cam1/control";
const char* topic_event = "SyDo/cam1/event";

WiFiClient espClient;
PubSubClient client(espClient);

// server
const char* serverUrl = "http://YOUR_PC_IP:5000/upload";

// ir sensor
#define IR_PIN 13

bool recording = false;
unsigned long lastMotionTime = 0;
const unsigned long motionTimeout = 5000; // stop if no motion for 5 seconds

// camera
void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;

  config.pin_d0 = 5;  config.pin_d1 = 18;
  config.pin_d2 = 19; config.pin_d3 = 21;
  config.pin_d4 = 36; config.pin_d5 = 39;
  config.pin_d6 = 34; config.pin_d7 = 35;

  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;

  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;

  config.pin_pwdn = 32;
  config.pin_reset = -1;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  esp_camera_init(&config);
}

// stream
static esp_err_t stream_handler(httpd_req_t *req){
  camera_fb_t * fb = NULL;

  httpd_resp_set_type(req, "multipart/x-mixed-replace;boundary=frame");

  while(true){
    fb = esp_camera_fb_get();
    if (!fb) continue;

    char header[64];
    sprintf(header,
      "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n",
      fb->len);

    httpd_resp_send_chunk(req, header, strlen(header));
    httpd_resp_send_chunk(req, (const char*)fb->buf, fb->len);
    httpd_resp_send_chunk(req, "\r\n", 2);

    esp_camera_fb_return(fb);
  }
}

void startStreamServer(){
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  httpd_uri_t uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = stream_handler
  };

  httpd_handle_t server = NULL;
  httpd_start(&server, &config);
  httpd_register_uri_handler(server, &uri);
}

// mqtt
void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  msg.toUpperCase();

  if (msg == "ON") recording = true;
  if (msg == "OFF") recording = false;
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("esp32cam")) {
      client.subscribe(topic_cmd);
    } else delay(2000);
  }
}

// upload frames
void captureFrame() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) return;

  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "image/jpeg");
  http.POST(fb->buf, fb->len);

  http.end();
  esp_camera_fb_return(fb);
}

void setup() {
  Serial.begin(115200);

  pinMode(IR_PIN, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(300);

  startCamera();
  startStreamServer();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  int motion = digitalRead(IR_PIN);

  // motion start
  if (motion == HIGH) {
    lastMotionTime = millis();

    if (!recording) {
      recording = true;
      Serial.println("Motion detected → START recording");
      client.publish(topic_event, "motion_start");
    }

    captureFrame();
    delay(300);
  }

  // motion stop
  if (recording && (millis() - lastMotionTime > motionTimeout)) {
    recording = false;
    Serial.println("No motion → STOP recording");
    client.publish(topic_event, "motion_stop");
  }
}