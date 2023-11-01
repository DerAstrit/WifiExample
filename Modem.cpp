#include "esp32-hal.h"
#include "WString.h"
/**
 * @file      MinimalModemNBIOTExample.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2022  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2022-09-16
 *
 */

#define TINY_GSM_RX_BUFFER 1024
#define TINY_GSM_MODEM_SIM7080
#define XPOWERS_CHIP_AXP2101

#include <Arduino.h>
#include <TinyGsmClient.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "XPowersLib.h"
#include "DeviceConstants.h"
#include "utilities.h"
#include "Modem.h"
XPowersPMU PMU;
String imei;
HardwareSerial SerialAT(1);

// See all AT commands, if wanted
//#define DUMP_AT_COMMANDS

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

const char *register_info[] = {
  "Not registered, MT is not currently searching an operator to register to.The GPRS service is disabled, the UE is allowed to attach for GPRS if requested by the user.",
  "Registered, home network.",
  "Not registered, but MT is currently trying to attach or searching an operator to register to. The GPRS service is enabled, but an allowable PLMN is currently not available. The UE will start a GPRS attach as soon as an allowable PLMN is available.",
  "Registration denied, The GPRS service is disabled, the UE is not allowed to attach for GPRS if it is requested by the user.",
  "Unknown.",
  "Registered, roaming.",
};


enum {
  MODEM_CATM = 1,
  MODEM_NB_IOT = 2,
  MODEM_CATM_NBIOT = 3,
};

void getPsmTimer();



bool level = false;







void initGps() {

  /*********************************
     *  step 1 : Initialize power chip,
     *  turn on modem and gps antenna power channel
    ***********************************/
  if (!PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, I2C_SDA, I2C_SCL)) {
    Serial.println("Failed to initialize power.....");
    while (1) {
      delay(5000);
    }
  }
  //Set the working voltage of the modem, please do not modify the parameters
  PMU.setDC3Voltage(3000);  //SIM7080 Modem main power channel 2700~ 3400V
  PMU.enableDC3();

  //Modem GPS Power channel
  PMU.setBLDO2Voltage(3300);
  PMU.enableBLDO2();  //The antenna power must be turned on to use the GPS function

  // TS Pin detection must be disable, otherwise it cannot be charged
  PMU.disableTSPinMeasure();


  /*********************************
     * step 2 : start modem
    ***********************************/

  Serial.println("*********************************");
  Serial.println("Start modem....");
  SerialAT.begin(115200, SERIAL_8N1, BOARD_MODEM_RXD_PIN, BOARD_MODEM_TXD_PIN);


  modem.restart();

  if (!modem.init()) {
    Serial.println("Failed to restart modem, delaying 10s and retrying");
  }

  int retry = 0;
  while (!modem.testAT(1000)) {
    Serial.print(".");
    if (retry++ > 6) {
      // Pull down PWRKEY for more than 1 second according to manual requirements
      digitalWrite(BOARD_MODEM_PWR_PIN, LOW);
      delay(100);
      digitalWrite(BOARD_MODEM_PWR_PIN, HIGH);
      delay(1000);
      digitalWrite(BOARD_MODEM_PWR_PIN, LOW);
      retry = 0;
      Serial.println("Retry start modem .");
    }
  }
  Serial.println();

  /*********************************
     * step 3 : start modem gps function
    ***********************************/
  //When configuring GNSS, you need to stop GPS first
  modem.disableGPS();
  delay(500);

  //GNSS Work Mode Set GPS+BEIDOU
  modem.sendAT("+CGNSMOD=1,1,0,0,0");
  modem.waitResponse();

  /*
        GNSS Command,For more parameters, see <SIM7070_SIM7080_SIM7090 Series_AT Command Manual> 212 page.
    <minInterval> range: 1000-60000 ms
     minInterval is the minimum time interval in milliseconds that must elapse between position reports. default value is 1000.
    <minDistance> range: 0-1000
     Minimum distance in meters that must be traversed between position reports. Setting this interval to 0 will be a pure time-based tracking/batching.
    <accuracy>:
        0  Accuracy is not specified, use default.
        1  Low Accuracy for location is acceptable.
        2 Medium Accuracy for location is acceptable.
        3 Only High Accuracy for location is acceptable.
    */

  // minInterval = 1000,minDistance = 0,accuracy = 0
  modem.sendAT("+SGNSCMD=2,1000,0,0");
  modem.waitResponse();

  // Turn off GNSS.
  modem.sendAT("+SGNSCMD=0");
  modem.waitResponse();

  delay(500);
  modem.enableGPS();
  // GPS function needs to be enabled for the first use
  if (modem.enableGPS() == false) {
    Serial.print("Modem enable gps function failed!!");
    while (1) {
      delay(5000);
    }
  }
  delay(20000);
}


String getLocation() {
  float lat, lon;

  if (modem.enableGPS() == false) {
    modem.enableGPS();
    delay(10000);
  }

  modem.getGPS(&lat, &lon);
  return String(lat, 6) + ", " + String(lon, 6);
}

String getIMEI() {
  return modem.getIMEI();
}

void connectToWiFi() {
  WiFi.begin(ssid, password);

  if (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}





// Modem is working but in combination with the API Post there are some problems that arent fixed till now.


// void initModem(){


//     Serial.begin(115200);

//     //Start while waiting for Serial monitoring
//     while (!Serial);

//     delay(3000);

//     Serial.println("*********************************");
//     Serial.println("Initialize power chip.....");


//     /*********************************
//      *  step 1 : Initialize power chip,
//      *  turn on modem and gps antenna power channel
//     ***********************************/
//     if (!PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, I2C_SDA, I2C_SCL)) {
//         Serial.println("Failed to initialize power.....");
//         while (1) {
//             delay(5000);
//         }
//     }
//     //Set the working voltage of the modem, please do not modify the parameters
//     PMU.setDC3Voltage(3000);    //SIM7080 Modem main power channel 2700~ 3400V
//     PMU.enableDC3();

//     //Modem GPS Power channel
//     PMU.setBLDO2Voltage(3300);
//     PMU.enableBLDO2();      //The antenna power must be turned on to use the GPS function

//     // TS Pin detection must be disable, otherwise it cannot be charged
//     PMU.disableTSPinMeasure();


//     /*********************************
//      * step 2 : start modem
//     ***********************************/
//     Serial.println("*********************************");
//     Serial.println("Start modem....");
//     SerialAT.begin(115200, SERIAL_8N1, BOARD_MODEM_RXD_PIN, BOARD_MODEM_TXD_PIN);


//   modem.restart();

//   if (!modem.init()) {
//     Serial.println("Failed to restart modem, delaying 10s and retrying");
//   }

//     int retry = 0;
//     while (!modem.testAT(1000)) {
//         Serial.print(".");
//         if (retry++ > 6) {
//             // Pull down PWRKEY for more than 1 second according to manual requirements
//             digitalWrite(BOARD_MODEM_PWR_PIN, LOW);
//             delay(100);
//             digitalWrite(BOARD_MODEM_PWR_PIN, HIGH);
//             delay(1000);
//             digitalWrite(BOARD_MODEM_PWR_PIN, LOW);
//             retry = 0;
//             Serial.println("Retry start modem .");
//         }
//     }
//     Serial.println();



//     /*********************************
//      * step 3 : Check if the SIM card is inserted
//     ***********************************/
//     String result ;


//     if (modem.getSimStatus() == SIM_ERROR) {
//         Serial.println("SIM Error!!!");
//         return  ;
//     }

//     if (modem.getSimStatus() == SIM_LOCKED) {
//         modem.simUnlock(GSM_PIN);
//         return  ;
//     }

//     if (modem.getSimStatus() == SIM_ANTITHEFT_LOCKED) {
//         Serial.println("SIM PUK needed!!!");
//         return  ;
//     }

//     if (modem.getSimStatus() != SIM_READY) {
//         Serial.println("SIM Card is not insert!!!");
//         return  ;
//     }

//     // Disable RF
//     modem.sendAT("+CFUN=0");

//     if (modem.waitResponse(20000UL) != 1) {
//         Serial.println("Disable RF Failed!");
//     }

//     /*********************************
//      * step 4 : Set the network mode
//     ***********************************/

//     modem.setNetworkMode(2);    //use automatic

//     modem.setPreferredMode(MODEM_CATM_NBIOT);

//     uint8_t pre = modem.getPreferredMode();

//     uint8_t mode = modem.getNetworkMode();

//     Serial.printf("getNetworkMode:%u getPreferredMode:%u\n", mode, pre);


//     //Set the APN manually. Some operators need to set APN first when registering the network.
//     modem.sendAT("+CGDCONT=1,\"IP\",\"", APN, "\"");
//     if (modem.waitResponse() != 1) {
//         Serial.println("Set operators APN Failed!");
//         return ;
//     }

//     //!! Set the APN manually. Some operators need to set APN first when registering the network.
//     modem.sendAT("+CNCFG=0,1,\"", APN, "\"");
//     if (modem.waitResponse() != 1) {
//         Serial.println("Config APN Failed!");
//         return ;
//     }

//     // Enable RF
//     modem.sendAT("+CFUN=1");
//     if (modem.waitResponse(20000UL) != 1) {
//         Serial.println("Enable RF Failed!");
//     }

//     /*********************************
//     * step 5 : Wait for the network registration to succeed
//     ***********************************/

//     Serial.println("*********************************");
//     Serial.println("Waiting for network registration.....");

//     RegStatus s;
//     do {
//         s = modem.getRegistrationStatus();
//         if (s != REG_OK_HOME && s != REG_OK_ROAMING) {
//             Serial.print(".");
//             PMU.setChargingLedMode(level ? XPOWERS_CHG_LED_ON : XPOWERS_CHG_LED_OFF);
//             level ^= 1;
//             delay(1000);
//         }

//     } while (s != REG_OK_HOME && s != REG_OK_ROAMING) ;

//     Serial.println();
//     Serial.print("Network register info:");
//     Serial.println(register_info[s]);





//     // Activate network bearer, APN can not be configured by default,
//     // if the SIM card is locked, please configure the correct APN and user password, use the gprsConnect() method
//     modem.sendAT("+CNACT=0,1");
//     if (modem.waitResponse() != 1) {
//         Serial.println("Activate network bearer Failed!");
//         return ;
//     }

//     // if (!modem.gprsConnect(APN, gprsUser, gprsPass)) {
//     //     return  ;
//     // }

//         if (modem.isGprsConnected()) {
//         Serial.println("GPRS is connected.");
//     } else {
//         Serial.println("GPRS is not connected.");
//     }
//     String ccid = modem.getSimCCID();
//     Serial.print("CCID:");
//     Serial.println(ccid);

//     imei = modem.getIMEI();
//     Serial.print("IMEI:");
//     Serial.println(imei);

//     String imsi = modem.getIMSI();
//     Serial.print("IMSI:");
//     Serial.println(imsi);

//     String cop = modem.getOperator();
//     Serial.print("Operator:");
//     Serial.println(cop);

//     IPAddress local = modem.localIP();
//     Serial.print("Local IP:");
//     Serial.println(local);

//     int csq = modem.getSignalQuality();
//     Serial.print("Signal quality:");
//     Serial.println(csq);

//       // modem.gprsDisconnect();
//      // Serial.println(F("GPRS disconnected"));

//     HTTPClient http;
//     const char* serverName = "https://personal-sl7oglgl.outsystemscloud.com/asd/rest/RESTAPI1/RESTAPIMethod1";
//     http.begin(serverName);

//     http.addHeader("Content-Type", "text/plain");

//     int httpResponseCode = http.POST("Ihr Text hier");

//     if (httpResponseCode>0) {
//       Serial.print("HTTP Response code: ");
//       Serial.println(httpResponseCode);

//     }

//     else {
//       Serial.print("Fehler beim Senden des POST-Requests: ");
//       Serial.println(http.errorToString(httpResponseCode).c_str());
//     }

//     http.end();

//   }
