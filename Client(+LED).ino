#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>

// BLE 설정
int scanTime = 5;
BLEScan* pBLEScan;
const String targetName = "DHT_ESP32";

// 거리 계산 파라미터
const int txPower = -59;
const float pathLossExponent = 2.0;
float lastDistance = -1.0;

// LED 핀 설정
const int ledPin = 21;  // GPIO21

// 거리 계산 함수
float calculateDistance(int rssi) {
  return pow(10.0, ((float)(txPower - rssi)) / (10.0 * pathLossExponent));
}

// BLE 스캔 콜백 클래스
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == targetName.c_str()) {
      int rssi = advertisedDevice.getRSSI();
      lastDistance = calculateDistance(rssi);

      Serial.print("RSSI: ");
      Serial.print(rssi);
      Serial.print(" dBm | Distance: ");
      Serial.print(lastDistance, 2);
      Serial.println(" m");
    }
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
}

void loop() {
  // BLE 스캔 수행
  pBLEScan->start(scanTime, false);  // 반환값 저장하지 않음
  pBLEScan->clearResults();

  // 거리 조건 확인 후 LED 깜빡이기
  if (lastDistance > 0 && lastDistance <= 1.0) {
    digitalWrite(ledPin, HIGH);
    delay(800);
    digitalWrite(ledPin, LOW);
    delay(800);
  } else {
    digitalWrite(ledPin, LOW);
    delay(500);
  }
}
