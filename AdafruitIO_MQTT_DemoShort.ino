/
 *  
 *  Original Author: Timothy Woo (www.botletics.com)
 *  Github: https://github.com/botletics/SIM7000-LTE-Shield
 *  Last Updated: 1/7/2021
 *  License: GNU GPL v3.0
 *  Changes made by William Burk to enable use with Arduino Uno.
 *  Code focused to work with SIM7000, on Verizon, and with Adafruit IO.
 */

#include "Adafruit_FONA.h" // https://github.com/botletics/SIM7000-LTE-Shield/tree/master/Code

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_FONA.h"

#define SIMCOM_7000

/************************* PIN DEFINITIONS *********************************/
#define FONA_PWRKEY 6
#define FONA_RST 7
#define FONA_TX 10 // Microcontroller RX
#define FONA_RX 11 // Microcontroller TX

#define samplingRate 60 // The time we want to delay after each post (in seconds)

#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);

SoftwareSerial *fonaSerial = &fonaSS;

  Adafruit_FONA_LTE fona = Adafruit_FONA_LTE();

/************************* MQTT SETUP *********************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "YourUserName"
#define AIO_KEY         "YourAdafruitKey"

Adafruit_MQTT_FONA mqtt(&fona, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

uint8_t txfailures = 0;  

Adafruit_MQTT_Publish feed_charging = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/charging");

#include <Wire.h>

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
char imei[16] = {0}; // Use this for device ID
uint8_t type;
uint16_t battLevel = 0; // Battery level (percentage)
uint8_t counter = 0;
char battBuff[6] = "3.345"; // Was 12

void setup() {
  Serial.begin(9600);
  Serial.println(F("*** SIMComExample ***"));
 
  pinMode(FONA_RST, OUTPUT);
  digitalWrite(FONA_RST, HIGH); // Default state

  fona.powerOn(FONA_PWRKEY); // Power on the module
  moduleSetup(); // Establishes first-time serial comm and prints IMEI
  fona.setFunctionality(1); // AT+CFUN=1
  fona.setNetworkSettings(F("vzwinternet")); // For Verizon SIM card

    if (!fona.enableGPRS(false)) Serial.println(F("Faildisdata"));
    while (!fona.enableGPRS(true)) {
      Serial.println(F("F en data"));
      delay(2000); // Retry every 2s
    }
    Serial.println(F("En data!"));

}

void loop() {
  while (!netStatus()) {
    Serial.println(F("Failed"));
    delay(2000); // Retry every 2s
  }
  Serial.println(F("Conn cell net"));

  battLevel = readVcc();
  dtostrf(battLevel,4,0,battBuff);

  MQTT_connect();

  MQTT_publish_checkSuccess(feed_charging, battBuff);

  Serial.print(F("Wafor ")); Serial.print(samplingRate); Serial.println(F(" ses\r\n"));
  delay(samplingRate * 1000UL); // Delay
}

void moduleSetup() {
  fonaSS.begin(115200); // Default SIM7000 shield baud rate
  Serial.println(F("C96"));
  fonaSS.println("AT+IPR=9600"); // Set baud rate
  delay(100); // Short pause to let the command run
  fonaSS.begin(9600);
  if (!fona.begin(fonaSS)) {
    Serial.println(F("t find FONA"));
    while (1); // Don't proceed if it couldn't find the device
  }
  type = fona.type();
  Serial.println(F("FONA is OK"));
  Serial.print(F("Found "));
  switch (type) {
    case SIM7000:
      Serial.println(F("SIM7000")); break;
    default:
      Serial.println(F("???")); break;
  }
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("Module IMEI: "); Serial.println(imei);
  }
}

float readVcc() {
  // Read battery voltage
  if (!fona.getBattVoltage(&battLevel)) Serial.println(F("Failbatt"));
  else Serial.print(F("b = ")); Serial.print(battLevel); Serial.println(F(" mV"));
  return battLevel;
}

bool netStatus() {
  int n = fona.getNetworkStatus();
  Serial.print(F("Net stat ")); Serial.print(n); Serial.print(F(": "));
  if (!(n == 1 || n == 5)) return false;
  else return true;
}

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }
  Serial.println("Con MQTT ");
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retg MQTTn 5 ses...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}

void MQTT_publish_checkSuccess(Adafruit_MQTT_Publish &feed, const char *feedContent) {
  Serial.println(F("Sending data..."));
  if (! feed.publish(feedContent)) {
    Serial.println(F("Failed"));
    txfailures++;
  }
  else {
    Serial.println(F("OK!"));
    txfailures = 0;
  }
}
