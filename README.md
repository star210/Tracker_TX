# Tracker_TX a LoRa and RTTY tracker for Arduino and SX1278

Based upon https://github.com/RoelKroes/TBTracker
Compiles for Arduino 328PU Pro Mini 8Mhz external Crystal 3.3v

Librarys:
- https://github.com/jgromes/RadioLib (Radiolib)
- https://github.com/mikalhart/TinyGPSPlus (tinyGPS++)

Components:
- Atmega328PU
- SX1278 LoRa chip (or compatible LoRa chip)
- ATGM336H GPS (Ublox compatible / 9600 Baud) or any compatible GPS device)
- AT2659 LNA (Optional, recommended for ATGM336H)

Voltages:
- Atmega328PU Min 2.4 max 3.6v
- SX1278 Min 1.8v Max 3.7v
- ATGM336H Min 2.7v max 3.6v
- AT2659 Min 1.4 max 3.6v

Battery:
- Saft Lithium Thionyl Chloride 3.6V, 2/3A Battery -> 
  
Connections:

    ATGM336H <> Arduino
    ----------------------
    VCC -> 3.3V
    GND -> GND
    RX -> D8
    TX -> D9
    ON/OFF -> D5 (Shutdown pin)
    BAUD RATE -> 9600
    
    AT2659 <> Arduino
    ----------------------
    VCC -> 3.3V
    GND -> GND
    _SHDN -> D5 (Shutdown pin)

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
    Voltage divider Ratio ->  1.28 (R1+R2)/R2


