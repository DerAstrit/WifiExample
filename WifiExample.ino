#include "BeaconScanner.h"
#include "Modem.h"
#include "RestAPI.h"
#include <WiFi.h>

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  initGps();             // Initialize GPS
  connectToWiFi();       // Connect to WiFi
}

void loop() {
  // Ensure that the device is connected to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Searching for Wlan connection");
    connectToWiFi();
  } else {
    Serial.println("-------------------------------");
    // Retrieve beacon UUIDs, IMEI, and location
    std::vector<String> ble_beacon_list = getBeaconMacAddressList();
    //LOG
    for (String mac : ble_beacon_list) {
      Serial.println(mac);
    }
    String IMEI = getIMEI();
    //LOG
    Serial.println(IMEI);
    String location = getLocation();
    //LOG
    Serial.println(location);

    // Make a POST request with the retrieved data
    post(IMEI, location, ble_beacon_list);
  }
}
