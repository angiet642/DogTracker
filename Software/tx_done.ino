#include <SPI.h>
#include <RH_RF95.h>
#include <Adafruit_GPS.h>
#define GPSSerial Serial1
Adafruit_GPS GPS(&GPSSerial);
uint32_t timer1 = millis();
// Set GPSECHO to ‘false’ to turn off echoing the GPS data to the Serial console
// Set to ‘true’ if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

// feather m4
#define RFM95_CS   9 //32    // “E”
#define RFM95_RST  10 //15   // “D”
#define RFM95_INT  19 //14   // “B”
#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);
String kylee_data = "131.00, -25, 6/6/6666 66:66:66.000";

void setup(){
//  setUpLora();
  setUpGPS();
}

void loop()
{
  Serial.println("ReadingGPS");
  readGPSData();
  delay(1000); // Wait 1 second between transmits, could also ‘sleep’ here!
//  loraTransmit();
}

void loraTransmit() {
  char tx_buf[50];
  memset(tx_buf, 0, sizeof(tx_buf));
  kylee_data.toCharArray(tx_buf, 50);
  Serial.print("Sending "); 
  Serial.println(tx_buf);
  Serial.println("Sending...");
  delay(10);
  rf95.send((uint8_t *)tx_buf, 50);
  Serial.println("Waiting for packet to complete...");
  delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t rx_buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(rx_buf);
  Serial.println("Waiting for reply...");
  if (rf95.waitAvailableTimeout(1000)){
    // Should be a reply message for us now
    if (rf95.recv(rx_buf, &len)){
      Serial.print("Got reply: ");
      Serial.println((char*)rx_buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
    }
    else{
      Serial.println("Receive failed");
    }
  }
  else{
    Serial.println("No reply, is there a listener around?");
  }
}
void setUpLora() {
  Serial.println("Transmitting..."); // Send a message to rf95_server
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
  delay(100);
  Serial.println("Feather LoRa TX Test!");
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);  
}
void setUpGPS() {
  Serial.println("Adafruit GPS library basic parsing test!");
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);  
}

void readGPSData() // run over and over again
{
  // read data from the GPS in the ‘main loop’
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    Serial.print(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer1 > 2000) {
    timer1 = millis(); // reset the timer1
    Serial.print("\nTime: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
//    if (GPS.fix) {
//      Serial.print("Location: ");
//      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
//      Serial.print(", ");
//      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
//    }
  
    float gps_latitude = 0.0;
    float gps_longitude = 0.0;
    if (GPS.fix) {
      gps_latitude = GPS.latitude/100.0;
      gps_longitude = GPS.longitude/100.0;
      if (GPS.lat == 'S') {
        gps_latitude = gps_latitude * -1;
      }
      if (GPS.lon == 'W') {
        gps_longitude = gps_longitude * -1;
      }
      kylee_data = String(gps_latitude, 6) + ", " + String(gps_longitude, 6) + ",  "+ String(GPS.day) + "/" + String(GPS.month) + "/" +String(GPS.year) + " " + String(GPS.hour) + ":" +String(GPS.minute) + ":"+ String(GPS.seconds);
      Serial.println(kylee_data);
    }
  }
}
