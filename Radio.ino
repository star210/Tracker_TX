// include the library
#include <RadioLib.h>

#define PACKETLEN 255

// Change 'SX1278' in the line below to 'SX1276' if you have a SX1276 module.
SX1278 radio = new Module(PIN_NSS, PIN_DIO0, PIN_DIO1);

// create RTTY client instance using the radio module
RTTYClient rtty(&radio);



//===============================================================================
void SetupRTTY()
{
  // First setup FSK
  SetupFSK();
#if defined(DEVMODE)
  Serial.print(F("[RTTY] Initializing"));
#endif
  int16_t state = rtty.begin(RTTYSettings.Frequency,
                             RTTYSettings.Shift,
                             RTTYSettings.Baud,
                             RTTYSettings.Encoding,
                             RTTYSettings.StopBits  );

  if (state != RADIOLIB_ERR_NONE) {
#if defined(DEVMODE)
    Serial.print(F("failed, code "));
    Serial.println(state);
#endif
   // while (true);
  }
}


//===============================================================================
void SetupFSK()
{
  // Initialize the SX1278
#if defined(DEVMODE)
  Serial.print(F("[SX1278] Initializing"));
#endif

  // int16_t state = radio.beginFSK();

  int16_t state = radio.beginFSK(FSKSettings.Frequency,
                                 FSKSettings.BitRate,
                                 FSKSettings.FreqDev,
                                 FSKSettings.RXBandwidth,
                                 FSKSettings.Power,
                                 FSKSettings.PreambleLength,
                                 FSKSettings.EnableOOK);

  if (state != RADIOLIB_ERR_NONE) {
#if defined(DEVMODE)
    Serial.print(F("failed, code "));
    Serial.println(state);
#endif
   // while (true);
  }
}
//===============================================================================
void SetupLoRa()
{
  // Initialize the SX1278
#if defined(DEVMODE)
  Serial.println(F("[LoRA] Initializing"));
#endif

  //ResetRadio();

  switch (LORA_MODE)
  {
    case 0:
      LoRaSettings.CodeRate = 8;
      LoRaSettings.Bandwidth = 20.8;
      LoRaSettings.SpreadFactor = 11;
      break;

    case 1:
      LoRaSettings.CodeRate = 5;
      LoRaSettings.Bandwidth = 20.8;
      LoRaSettings.SpreadFactor = 6;
      break;

    case 2:
      LoRaSettings.CodeRate = 8;
      LoRaSettings.Bandwidth = 62.5;
      LoRaSettings.SpreadFactor = 8;
      break;

    case 3:
      LoRaSettings.CodeRate = 6;
      LoRaSettings.Bandwidth = 250;
      LoRaSettings.SpreadFactor = 7;
      break;
    case 5:
      LoRaSettings.CodeRate = 8;
      LoRaSettings.Bandwidth = 41.7;
      LoRaSettings.SpreadFactor = 11;
      break;

  }

  int16_t state = radio.begin
                  (
                    LoRaSettings.Frequency,
                    LoRaSettings.Bandwidth,
                    LoRaSettings.SpreadFactor,
                    LoRaSettings.CodeRate,
                    LoRaSettings.SyncWord,
                    LoRaSettings.Power,
                    LoRaSettings.PreambleLength,
                    LoRaSettings.Gain
                  );

  switch (LORA_MODE)
  {
    case 0:
      radio.forceLDRO(true);
      radio.setCRC(true);
      break;
    case 1:
      radio.implicitHeader(PACKETLEN);
      //radio.forceLDRO(true);
      radio.setCRC(true);
      break;
    default:
      radio.explicitHeader();
      //radio.autoLDRO();
      radio.setCRC(true);
      break;
  }

  if (state != RADIOLIB_ERR_NONE) {// Change this to (state == ERR_NONE) if you use an older radiolib library
#if defined(DEVMODE)
    Serial.print(F("[LORA] Failed Code: "));
    Serial.println(state);
#endif
   // while (true);
  }
}


//===============================================================================
//===============================================================================
void StandbyRadio()
{
#if defined(DEVMODE)
  Serial.println(F("[Radio] Standby"));
#endif
  // Sends command to standby
  int state = radio.standby();
}
//===============================================================================
void SleepRadio()
{
#if defined(DEVMODE)
  Serial.println(F("[Radio] Sleep"));
#endif
  // Sends command to sleep
  int state = radio.sleep();
}

//===============================================================================
void SetupRadio()
{
  // Setting up the radio
  if (RTTY_ENABLED) {
    SetupRTTY();
  }
  if (LORA_ENABLED) {
    SetupLoRa();
  }
}

//===============================================================================
void sendRTTY(String TxLine) {
  // Setup Radio
  SetupRTTY();
  // Send RTTY as many times as needed
  for (int i = 1; i <= RTTY_REPEATS; i++) {
#if defined(DEVMODE)
    Serial.print(F("[RTTY] Sending: "));
    Serial.println(TxLine);
#endif
    // Send only idle carrier to let people get their tuning right
    rtty.idle();
    delay(RTTY_IDLE_TIME);
    // Send the payload to the radio
    int state = rtty.println(TxLine);
  }
  // Put Radio to sleep to save energy
  SleepRadio();
}

//===============================================================================
void sendLoRa(String TxLine) {

  int state;
  SetupLoRa();

#if defined(DEVMODE)
  Serial.print(F("[LORA] Sending: "));
  Serial.println(TxLine);
#endif

  switch (LORA_MODE)
  {
    case 1:
      int i;
      int j;
      // Send the string
      char buf[PACKETLEN];
      for (j = 0; j < PACKETLEN; j++) {
        buf[j] = '\0';
      }
      for (i = 0; i < TxLine.length(); i++) {
        buf[i] = TxLine[i];
      }
      state = radio.transmit((uint8_t*)buf, PACKETLEN);
      break;
    default:
      // Send the string
      state = radio.transmit(TxLine);
      break;
  }

  if (state != RADIOLIB_ERR_NONE) { // Change this to (state == ERR_NONE) if you use an older radiolib library
#if defined(DEVMODE)
    Serial.print(F("[LORA] Failed Code: "));
    Serial.println(state);
#endif
  }
  // Put Radio to sleep to save energy
  SleepRadio();
}
