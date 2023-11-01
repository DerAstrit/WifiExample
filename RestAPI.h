#pragma once

#include <Arduino.h>
#include <vector>


// Posts data to a REST API endpoint.
// Parameters:
// - IMEI: The IMEI of the device.
// - location: The location data.
// - uuids: A vector of UUIDs.
// Returns:
// - An integer status code indicating the result of the operation.
int post(const String& IMEI, const String& location, const std::vector<String>& macAddressList);
