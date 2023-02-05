#include <TinyGPS++.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps;

/*********************************************************************************************************************************/
void CheckGPS()
{
  processGPSData();
  printGPSData();
}

/*********************************************************************************************************************************/
// This custom version of delay() ensures that the gps object is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (SerialGPS.available())
      gps.encode(SerialGPS.read());
  } while (millis() - start < ms);
}

/*********************************************************************************************************************************/
static void processGPSData()
{
  byte DesiredMode;

  // Number of Satellites
  if (gps.satellites.isValid())
    UGPS.Satellites = gps.satellites.value();

  else
    UGPS.Satellites = 0;

  // Time
  if (gps.time.isValid())
  {
    UGPS.Hours = gps.time.hour();
    UGPS.Minutes = gps.time.minute();
    UGPS.Seconds = gps.time.second();
  }
  else
  {
    UGPS.Hours = 0;
    UGPS.Minutes = 0;
    UGPS.Seconds = 0;
  }

  // Position
  if (gps.location.isValid())
  {
    UGPS.Longitude = gps.location.lng();
    UGPS.Latitude = gps.location.lat();
    UGPS.Heading = gps.course.deg();
    UGPS.Age = gps.location.age();
  }
  else
  {
    UGPS.Longitude = 0;
    UGPS.Latitude = 0;
    UGPS.Heading = 0;
    UGPS.Age = 0;
  }

  // HDOP
  if (gps.hdop.isUpdated())
    UGPS.Hdop = gps.hdop.value();
  else
    UGPS.Hdop = 999;

  // Altitude
  if (gps.altitude.isValid())
    UGPS.Altitude = abs(gps.altitude.meters());
  else
    UGPS.Altitude = 0;

  if (UGPS.Altitude > 1000)
    DesiredMode = AIRBORNE;
  else
    DesiredMode = PEDESTRIAN;

  // Set the correct flight mode for high altitude
  if (UGPS.FlightMode != DesiredMode)
    setDesiredMode(DesiredMode);


}

/*********************************************************************************************************************************/
void printGPSData()
{
#if defined(DEVMODE)
  Serial.print(F("         Time: ")); Serial.print(UGPS.Hours); Serial.print(F(":")); Serial.print(UGPS.Minutes); Serial.print(F(":")); Serial.println(UGPS.Seconds);
  Serial.print(F("     Latitude: ")); Serial.println(UGPS.Latitude, 6);
  Serial.print(F("    Longitude: ")); Serial.println(UGPS.Longitude, 6);
  Serial.print(F(" Altitude (m): ")); Serial.println(UGPS.Altitude);
  Serial.print(F("   Satellites: ")); Serial.println(UGPS.Satellites);
  Serial.print(F("         HDOP: ")); Serial.println(UGPS.Hdop);
  Serial.print(F("      Fix Age: ")); Serial.println(UGPS.Age);
  Serial.print(F("  Temperature: ")); Serial.println(ReadTemp());
  Serial.print(F(" Batt Voltage: ")); Serial.println(ReadVCC());
  Serial.print(F("      Seconds: ")); Serial.println(millis() / 1000);
  Serial.println();
  Serial.println(F("-------------------------"));
#endif
}


/*********************************************************************************************************************************/
void SendUBX(unsigned char *Message, int Length)
{
  int i;

  for (i = 0; i < Length; i++)
  {
    SerialGPS.write(Message[i]);
  }
}


/*********************************************************************************************************************************/
void setDesiredMode(byte aDesiredMode)
{
  if (aDesiredMode == PEDESTRIAN)
    setGPS_DynamicModel3();
  else if (aDesiredMode == AIRBORNE)
    setGPS_DynamicModel6();

  UGPS.FlightMode = aDesiredMode;
}

/*********************************************************************************************************************************/
// Pedestrian mode
void setGPS_DynamicModel3()
{
  unsigned char setNav[] =
  {
    0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA,
    0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4A, 0x75
  };

#if defined(DEVMODE)
  Serial.println(F("[GPS] Set Pedestrian Mode"));
#endif
  SendUBX(setNav, sizeof(setNav));
}

/*********************************************************************************************************************************/
// High altitude flightmode
void setGPS_DynamicModel6()
{
  unsigned char setNav[] =
  {
    0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA,
    0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4D, 0xDB
  };

#if defined(DEVMODE)
  Serial.println(F("[GPS] Set Airborne Mode"));
#endif
  SendUBX(setNav, sizeof(setNav));
}
