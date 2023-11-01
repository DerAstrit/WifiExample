#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <vector>
#include "DeviceConstants.h"


const uint32_t SCAN_TIME = 30;  //30 Sekunden

BLEScan* pBLEScan;
std::vector<String> macList;

class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    int rssi = advertisedDevice.getRSSI();
    if (rssi > RSSI_THRESHOLD) {                                      // Stellen Sie sicher, dass RSSI_THRESHOLD definiert ist
      String mac = advertisedDevice.getAddress().toString().c_str();  //
      if (mac != "" && mac != "<NULL>" && std::find(macList.begin(), macList.end(), mac) == macList.end()) {
        macList.push_back(mac);
      }
    }
  }
};


void initiateScan() {
  macList.clear();
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(SCAN_TIME, false);
}



std::vector<String> getBeaconMacAddressList() {
  initiateScan();
  return macList;
}
