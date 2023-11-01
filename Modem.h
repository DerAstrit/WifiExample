#ifndef MODEM_H
#define MODEM_H



// Initializes the modem
void initModem();

// Initializes the GPS
void initGps();

// Gets the location from the GPS
String getLocation();

// Gets the IMEI of the modem
String getIMEI();

// Connects to the WiFi
void connectToWiFi();


#endif  // MODEM_H
