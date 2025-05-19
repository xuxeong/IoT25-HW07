#include <WiFi.h>
#include <WebServer.h>
#include <BLEDevice.h>
#include <BLEScan.h>

const char* ssid = "김연주의 iPhone";   // Wi-Fi 이름
const char* password = "12345678";           // 비밀번호 없는 경우 빈 문자열

WebServer server(80);
BLEScan* pBLEScan;
const String targetName = "DHT_ESP32";
const int txPower = -59;             // 환경에 따라 조정 가능
const float n = 2.0;                 // 환경에 따라 2.0~3.0 조정
float lastDistance = -1;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == targetName) {
      int rssi = advertisedDevice.getRSSI();
      lastDistance = pow(10, (txPower - rssi) / (10 * n));
      Serial.printf("RSSI: %d dBm | Distance: %.2f m\n", rssi, lastDistance);
    }
  }
};

void handleRoot() {
  server.send(200, "text/plain", String(lastDistance, 2) + "m");
}

void setup() {
  Serial.begin(115200);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP());  // 웹서버 주소 출력

  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  pBLEScan->start(3, false);  // 3초간 BLE 스캔
  server.handleClient();
  delay(500);
}