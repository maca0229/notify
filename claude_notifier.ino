#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

const int MOTOR_PIN = 10;
WebServer server(80);

bool motorActive = false;
unsigned long lastToggle = 0;
bool motorState = false;
unsigned long motorStartTime = 0;
const int MOTOR_TIMEOUT = 10000; // 10秒自动停止

// 脉冲节奏：转300ms，停200ms，循环
const int ON_DURATION = 300;
const int OFF_DURATION = 200;

void handleOn() {
  motorActive = true;
  motorStartTime = millis();
  lastToggle = millis();
  motorState = true;
  digitalWrite(MOTOR_PIN, HIGH);
  server.send(200, "text/plain", "motor ON");
}

void handleOff() {
  motorActive = false;
  motorState = false;
  digitalWrite(MOTOR_PIN, LOW);
  server.send(200, "text/plain", "motor OFF");
}

void handleRoot() {
  server.send(200, "text/plain", "claude notifier online");
}

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.begin();
}

void loop() {
  server.handleClient();

  if (motorActive) {
    unsigned long now = millis();
    if (now - motorStartTime >= MOTOR_TIMEOUT) {
      motorActive = false;
      motorState = false;
      digitalWrite(MOTOR_PIN, LOW);
      return;
    }
    unsigned long duration = motorState ? ON_DURATION : OFF_DURATION;
    if (now - lastToggle >= duration) {
      motorState = !motorState;
      digitalWrite(MOTOR_PIN, motorState ? HIGH : LOW);
      lastToggle = now;
    }
  }
}
