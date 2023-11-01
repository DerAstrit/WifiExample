#ifndef BeaconScanner_h
#define BeaconScanner_h

#include <Arduino.h>
#include <vector>


// Gets the UUIDs of detected beacons.
// Returns a vector containing the UUIDs as strings.
std::vector<String> getBeaconMacAddressList();


#endif
