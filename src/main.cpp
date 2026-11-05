#include <Arduino.h>
#include <HTTPClient.h>
#include <Servo.h>
#include <Wifi.h>

// Servo + Gate Pin GP-Nr.
const Servo servo;
const int SERVO_OUT_PIN = 16;

// Wifi Credentials
const char *SSID = "MyWifi";
const char *WIFI_PW = "MyWifiPassword";

// Server-IP
const String SERVER = "http://192.168.254.254";

// Gate-type: entry | exit
const String GATE_TYPE = "entry";

bool fetchGateState(HTTPClient http) {
  http.begin(SERVER + "/api/gatestate/" + GATE_TYPE);
  if (http.GET() == 200) {
    return (bool)http.getString();
  } else if (http.GET == 400) {
    Serial.println("HTTP GET Failed. Couldn't reach endpoint.");
    Serial.println("Retrying in 5 seconds...");
    Serial.println();
    delay(5000);
    return false
  } else {
    return false;
  }
}

bool waitForStateChange(HTTPClient http) {
  bool gateState = false;
  while (!gateState) {
    delay(500);
    gateState = fetchGateState(http);
  }
  return gateState;
}

void setup() {
  Serial.begin(115200);

  Wifi.begin(SSID, WIFI_PW);
  while (Wifi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(Wifi.localIP());

  pinMode(LED_BUILTIN, OUTPUT);
  servo.attach(SERVO_OUT_PIN, 500, 2400);
}

void loop() {
  static bool gateState = false;
  static HTTPClient http;
  if (gateState) {
    servo.Write(90);
    delay(5000);
    servo.Write(0);
    gateState = false;
  } else {
    gateState = waitForStateChange(http);
  }
}