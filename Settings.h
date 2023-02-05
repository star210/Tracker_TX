/***********************************************************************************
   LoRa and RTTY tracker for Arduino and SX1278

    1 x Atmega328PU
    1 x SX1278 LoRa chip (or compatible LoRa chip)
    1 x ATGM336H GPS (Ublox compatible / 9600 Baud) or any compatible GPS device)

    ATGM336H <> Arduino
    ----------------------
    VCC -> 3.3V
    GND -> GND
    RX -> D8
    TX -> D9
    ON/OFF -> D5 (Shutdown pin)
    BAUD RATE -> 9600

    SX1278 <-> Arduino
    ----------------------
    MISO   -> D12
    SCK    -> D13
    RST    -> D4 (SSX1276)
    GND    -> GND
    DI00   -> D2
    MOSI   -> D11
    NSS    -> D10
    3V3    -> 3V3

    External Voltage <-> Arduino
    ----------------------
    AnalogRead Pin -> A2
    R1 -> 2.8K
    R2 -> 10K
    Voltage divider ->  1.28 (R1+R2)/R2)

    Extra librairies needed:
    https://github.com/jgromes/RadioLib (Radiolib)
    https://github.com/mikalhart/TinyGPSPlus (tinyGPS++)

    For payload information and how to get your Payload on the map, see the file Misc.ini from this sketch

/************************************************************************
  PIN NUMBERS for SX1278

  Change if needed
************************************************************************************/
#define PIN_NSS   10
#define PIN_DIO0  2
#define PIN_BUSY  -1  // Not used in this sketch for sx1278
#define PIN_RESET  4  // Not used in this sketch for sx1278
#define PIN_DIO1   3  // Not used in this sketch for sx1278

/***********************************************************************************
  DEFAULT FSK SETTINGS

  Normally needs no change
************************************************************************************/
#define FSK_FREQUENCY 434.113
#define FSK_BITRATE 100.0
#define FSK_FREQDEV 50.0
#define FSK_RXBANDWIDTH 125.0
#define FSK_POWER 17   // in dBm between 2 and 17. 10 = 10mW (recommended). Sets also RTTY power
#define FSK_PREAMBLELENGTH 16
#define FSK_ENABLEOOK false
#define FSK_DATASHAPING 0.5

/***********************************************************************************
  RTTY SETTINGS

  Change when needed
************************************************************************************/
#define RTTY_ENABLED            false         // Set to true if you want RTTY transmissions (You can use Both LoRa and RTTY or only one of the two) 
#define RTTY_PAYLOAD_ID         "M7ONS"       // Payload ID for RTTY protocol
#define RTTY_FREQUENCY          434.113       // Can be different from LoRa frequency
#define RTTY_SHIFT              610
#define RTTY_BAUD               100        // Baud rate
#define RTTY_STOPBITS           2
#define RTTY_PREFIX             "$$$$$$"
#define RTTY_ENCODING           0            // 0 - ASCII 7 bits, 1 ASCII EXTENDED 8 bits, 2 - BAUDOT 5 Bits
#define RTTY_REPEATS            1           // number of RTTY transmits during a cycle

// Idle carrier in ms before sending actual RTTY string.
// Set to a low value (i.e. 1000 or lower) if you have a very frequency stable signal
// Set to a high value (i.e. 5000 or even higher) if you have a hard time to tune the signal
#define RTTY_IDLE_TIME          5000

/***********************************************************************************
* LORA SETTINGS
*  
* Change when needed
************************************************************************************/
#define LORA_ENABLED true            // Set to true if you want LoRa transmissions (You can use Both LoRa and RTTY or only one of the two)
#define LORA_PAYLOAD_ID  "M7ONS"   // Payload ID for LoRa protocol
#define LORA_FREQUENCY  434.126      // Can be different from RTTY frequency
#define LORA_BANDWIDTH 125.0         // Do not change, change LORA_MODE instead
#define LORA_SPREADFACTOR 9          // Do not change, change LORA_MODE instead
#define LORA_CODERATE 7              // Do not change, change LORA_MODE instead
#define LORA_PREFIX "$$"             // Prefix for "Telemetry". Some older LoRa software does not accept a prefix of more than 2x "$"
#define LORA_SYNCWORD 0x12           // Default syncword
#define LORA_POWER 17                // in dBm between 2 and 17. 10 = 10mW (recommended)
#define LORA_CURRENTLIMIT 100
#define LORA_PREAMBLELENGTH 8
#define LORA_GAIN 0
// HAB modes
// 0 = (normal for telemetry)  Explicit mode, Error coding 4:8, Bandwidth 20.8kHz, SF 11, Low data rate optimize on  - SUPPORTED
// 1 = (normal for SSDV)       Implicit mode, Error coding 4:5, Bandwidth 20.8kHz,  SF 6, Low data rate optimize off - SUPPORTED
// 2 = (normal for repeater)   Explicit mode, Error coding 4:8, Bandwidth 62.5kHz,  SF 8, Low data rate optimize off - SUPPORTED
// 3 = (normal for fast SSDV)  Explicit mode, Error coding 4:6, Bandwidth 250kHz,   SF 7, Low data rate optimize off - SUPPORTED
// 4 = Test mode not for normal use. - NOT SUPPORTED
// 5 = (normal for calling mode)   Explicit mode, Error coding 4:8, Bandwidth 41.7kHz, SF 11, Low data rate optimize off - SUPPORTED
#define LORA_MODE 2  // Mode 2 is usually used for simple telemetry data
#define LORA_REPEATS 1 // number of LoRa transmits during a cycle
/***********************************************************************************
  TRANSMISSIONS SETTINGS

  Change if needed
************************************************************************************/
#define SENTENCE_LENGTH 100     // Maximum length of telemetry line to send

/// Allow time for the GPS to re-acquire a fix when using sleep mode!
// Currently deep sleep is only enabled for ATMEGA328
#define TIME_TO_SLEEP  30       // This is the number in seconds out of TX_LOOP_TIME that the CPU is in sleep. Only valid when USE_DEEP_SLEEP = true

                                
// Define up to 5 pins to power sensors from (for example your GPS). Each Arduino pin can source up to 40mA. All together, the pins can source 150-200 mA
// Use a transistor or FET as a switch if you need more power. Or use multiple pins in parallel.
// This will only work when USE_DEEP_SLEEP=true and there is a valid GPS lock.
// Comment out the pins you use for your sensors or leds.
// Set pin value to a valid value.
#define POWER_PIN1     5
// #define POWER_PIN2     -1
// #define POWER_PIN3     -1
// #define POWER_PIN4     -1
// #define POWER_PIN5     -1

#define DEVMODE // Development mode. Uncomment to enable for debugging and see debug info on the serial monitor

/***********************************************************************************/
//  GPS SETTINGS

#define   SS_RX        9  
#define   SS_TX        8
#define   GPS_BAUD     9600
#define   PEDESTRIAN   3
#define   AIRBORNE     6

/***********************************************************************************
  SENSOR SETTINGS

  Change if needed

   You can connect an external voltage directly to the EXTERNALVOLTAGE_PIN as long as the the pin is rated for that voltage
   Alteratively, you can use a voltage divider so you can connect a higher voltage, but then you have to calculate the DIVIDER_RATIO yourself

   Voltage divider schema:

                           |-----------------
                           |                |
                           |               R1
                           |                |
                     +/+ ---                |
     to external voltage                    |------ To EXTERNALVOLTAGE_PIN
                     -/- ---                |
                           |                |
                           |               R2
                           |                |
                           |----------------------- To Arduino GND

    DIVIDER_RATIO can be calculated by (R1+R2) / R2

    Optionally add a 100nF capacitor between A1 and GND if the measured voltage seems unstable
************************************************************************************/
#define USE_EXTERNAL_VOLTAGE true // Set to true if you want to measure an external voltage on the EXTERNALVOLTAGE_PIN 
#define VCC_OFFSET 0.00            // Offset for error correction in Volts for the internal voltage. Ideally this should be 0.0 but usually is between -0.1 and +0.1 and is chip specific. 
#define EXT_OFFSET 0.00            // Offset for error correction in Volts for the external voltage. Use it to correct errors when necessary.
#define EXTERNALVOLTAGE_PIN A2     // Pin to read the external voltage from
#define SAMPLE_RES 1024            // 1024 for Uno, Mini, Nano, Mega, Micro. Leonardo. 4096 for Zero, Due and MKR  
#define DIVIDER_RATIO 1.28         // Leave at 1.00 when using no voltage divider. Set to (R1+R2)/R2 when using a voltage divider.

/***********************************************************************************
* SONDEHUB EXTRA FIELDS SETTINGS
*  
* For displaying extra fields at sondehub, we need to define which fields are
* in the telemetry after the lat, lon, alt fields
* This can be done by adding a specific string after the last telemetry field
* This is supported by the various receivers made by Dave Akerman,
* See: https://www.daveakerman.com/?page_id=2410
* 
* 0  PayloadID
* 1 Counter
* 2 Time
* 3 Latitude
* 4 Longitude
* 5 Altitude
* 6 Satellites
* 7 Speed
* 8 Heading
* 9 Battery Voltage
* A InternalTemperature
* B ExternalTemperature
* C PredictedLatitude
* D PredictedLongitude
* E CutdownStatus
* F LastPacketSNR
* G LastPacketRSSI
* H ReceivedCommandCount
* I-N ExtraFields
* O MaximumAltitude
* P Battery Current
* Q External Temperature 2
* R Pressure
* S Humidity
* T CDA
* U Predicted Landing Speed
* V Time Till Landing
* W Last Command Received
* 
* Our string would be: "01234568A9"
* You can disable FIELDSTR by undefining it, if you want.
************************************************************************************/
#define USE_FIELDSTR
#define FIELDSTR "01234568A9"


/***********************************************************************************
  TELEMETRY COUNTERS

  Uncomment this if you want to reset the counters for LoRa and RTTY set back to 0.
************************************************************************************/
// #define RESET_TRANS_COUNTERS
