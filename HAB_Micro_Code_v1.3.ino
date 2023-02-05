/***********************************************************************************
  - Compiles for Arduino 328PU Pro Mini 8Mhz external Crystal 3.3v
  - SX1278 Min 1.8v Max 3.7v
  - ATGM336H Min 2.7v max 3.6v
  - AT2659 Min 1.4 max 3.6v
***********************************************************************************/
void(* resetFunc) (void) = 0;  // reset function


#include <SoftwareSerial.h>
#include "Settings.h"

/***********************************************************************************/
//  DATA STRUCTS

// Struct to hold GPS data
struct TGPS
{
  uint8_t Hours, Minutes, Seconds;
  float Longitude, Latitude;
  uint32_t Altitude;
  uint8_t Satellites;
  byte FlightMode;
  unsigned int Heading;
  uint16_t Hdop;
  uint32_t Age;
} UGPS;

// Struct to hold LoRA settings
struct TLoRaSettings
{
  float Frequency = LORA_FREQUENCY;
  float Bandwidth = LORA_BANDWIDTH;
  uint8_t SpreadFactor = LORA_SPREADFACTOR;
  uint8_t CodeRate = LORA_CODERATE;
  uint8_t SyncWord = LORA_SYNCWORD;
  uint8_t Power = LORA_POWER;
  uint8_t CurrentLimit = LORA_CURRENTLIMIT;
  uint16_t PreambleLength =  LORA_PREAMBLELENGTH;
  uint8_t Gain = LORA_GAIN;
} LoRaSettings;

// Struct to hold FSK settings
struct TFSKSettings
{
  float Frequency = FSK_FREQUENCY;
  float BitRate = FSK_BITRATE;
  float FreqDev = FSK_FREQDEV;
  float RXBandwidth = FSK_RXBANDWIDTH;
  int8_t  Power = FSK_POWER;                  // in dbM range 2 - 17
  uint16_t  PreambleLength = FSK_PREAMBLELENGTH;
  bool  EnableOOK = FSK_ENABLEOOK;
  float dataShaping = FSK_DATASHAPING;
} FSKSettings;

// Struct to hold RTTY settings
struct TRTTYSettings
{
  float Frequency = RTTY_FREQUENCY;   // Base frequency
  uint32_t Shift = RTTY_SHIFT;        // RTTY shift
  uint16_t Baud = RTTY_BAUD;          // Baud rate
  uint8_t Encoding = RTTY_ENCODING;   // Encoding (ASCII = 7 bits)
  uint8_t StopBits = RTTY_STOPBITS;   // Number of stopbits
} RTTYSettings;

/***********************************************************************************/
//  GLOBALS

SoftwareSerial SerialGPS(SS_RX, SS_TX);
char Sentence[SENTENCE_LENGTH];
uint16_t RTTYCounter = 0;
long LoRaCounter = 0;
unsigned long previousTX = 0;
volatile bool watchdogActivated = true;
volatile int sleepIterations = 0;
uint8_t resetCounter = 0;

//============================================================================
void setup()
{
  // Setup Serial for debugging
  Serial.begin(57600);
  //  Set clock prescaler for 4mhz @ 1.8v
  //  CLKPR = 0x80; // (1000 0000) enable change in clock frequency
  //  CLKPR = 0x01; // (0000 0001) use clock division factor 2 to reduce the frequency from 16 MHz to 8 MHz

  // Setup the Ublox GPS
  SerialGPS.begin(GPS_BAUD);  //TX, RX

#if defined(RESET_TRANS_COUNTERS)
  Reset_Transmission_Counters();
#endif

  // Setup the radio and send to sleep untill GPS lock
  SetupRadio();
  SleepRadio();
  
  // Setup power pins and switch on GPS power
  setup_PowerPins();
  enable_PowerPins();
}


//============================================================================
void loop() {
  // Watchdog should have been fired before doing anything
  if (watchdogActivated) {
    // Reset the watchdog and the sleep timer
    watchdogActivated = false;

    // Get data from the GPS
    CheckGPS();
    smartDelay(1000);
    // If GPS hdop is not received after 20 seconds reset GPS
    if (UGPS.Hdop == 999) {
      resetCounter ++;
      if (resetCounter > 20) {
        resetCounter = 0;
        resetPower();
      }
    }
    // If GPS last fix age is more than 20 seconds reset GPS
    if (UGPS.Age > 20000) {
      resetPower();
      resetFunc();
    }


    if (UGPS.Hdop < 400 && UGPS.Age < 500) {
      // Switch off GPS while transmitting
      disable_PowerPins();

      // Send RTTY
      if (RTTY_ENABLED == true) {
        RTTYCounter = EEPROMReadlong(0x00);
        CreateTXLine(RTTY_PAYLOAD_ID, RTTYCounter++, RTTY_PREFIX);
        // Write the RTTY counter to EEPROM at address 0x00
        EEPROMWritelong(0x00, RTTYCounter);
        sendRTTY(Sentence);
      }
      // Send LoRa
      if (LORA_ENABLED)
      {
        for (int i = 1; i <= LORA_REPEATS; i++)
        {
          LoRaCounter = EEPROMReadlong(0x04);
          CreateTXLine(LORA_PAYLOAD_ID, LoRaCounter++, LORA_PREFIX);
          // Write the LoRa counter to EEPROM at address 0x04
          EEPROMWritelong(0x04, LoRaCounter);
          sendLoRa(Sentence);
        }
      }

#if defined(DEVMODE)
      Serial.println(F("[Sleep] Start"));
#endif
      Serial.flush();
      sleepIterations = 0;
      while (sleepIterations < TIME_TO_SLEEP)
      {
        my_Sleep();
      }
#if defined(DEVMODE)
      Serial.println("[Sleep] Finish");
#endif
      previousTX = millis();
      // Set all defined power pins to high
      enable_PowerPins();
    }
  }
  watchdogActivated = true;
}
