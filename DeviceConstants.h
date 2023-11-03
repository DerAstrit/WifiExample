#pragma once

// Constants related to HTTP / REST API
static const char* const REST_API_URL = "https://personal-sl7oglgl.outsystemscloud.com/TNetVision_API/rest/Lilygo_7080g/Post_Data";

// Constants related to GSM / Network
#define TINY_GSM_MODEM_SIM7080

static const char* GSM_PIN = "8671";
static const char* APN = "gprs.swisscom.ch";
static const char* GPRS_USER = "";
static const char* GPRS_PASS = "";

static const char* ssid = "Der Gerät";
static const char* password = "12345678!";

// Beacon RSSI Threshold
static const int RSSI_THRESHOLD = -50;

static const uint32_t SCAN_TIME = 40;  //50 Sekunden

static const long int TENANT_ID = 145;


