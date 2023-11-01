#include "RestAPI.h"
#include "DeviceConstants.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>



int post(const String& IMEI, const String& location, const std::vector<String>& macAddressList) {
  HTTPClient http;
  http.begin(REST_API_URL);
  http.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc(1024);
  doc["tenant_id"] = TENANT_ID;
  doc["imei"] = IMEI;
  doc["location"] = location;

  JsonArray beacons = doc.createNestedArray("beacons");
  for (const String& macAddress : macAddressList) {
    beacons.add(macAddress);
  }

  String jsonBody;
  serializeJson(doc, jsonBody);

  int httpCode = http.POST(jsonBody);
  // ... (handle the HTTP response, including different status codes and potential errors)

  http.end();
  return httpCode;
}
