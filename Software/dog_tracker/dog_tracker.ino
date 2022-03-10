#include <SPI.h>
#include <RH_RF95.h>
#include <Adafruit_GPS.h>
#define GPSSerial Serial1
#define ALARM_PIN A0
// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);
uint32_t timer = millis();
uint32_t timer2 = millis();
#define GPSECHO false
/* for ESP w/featherwing */
//#define RFM95_CS  14    // "E"
//#define RFM95_RST 32   // "D"
//#define RFM95_INT 33   // "B"
// feather m4
#define RFM95_CS   5//9 //32    // "E"
#define RFM95_RST  6//10 //15   // "D"
#define RFM95_INT  10//19 //14   // "B"
#define RF95_FREQ 915.0
#define LED 13 // Blinky on receipt

#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
int accum = 0;
String kylee_data = "3746.4940N, 12225.1640W, 6/6/6666 66:66:66.000";
RH_RF95 rf95(RFM95_CS, RFM95_INT);
Adafruit_MMA8451 mma = Adafruit_MMA8451();

void setup() {
  // -------------------------------------- TX SET UP -----------------------------------------------
  pinMode(LED, OUTPUT);
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
  Serial.println("Adafruit GPS library basic parsing test!");

  // -------------------------------------- GPS SET UP -----------------------------------------------
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);

  // -------------------------------------- Accelerometer SET UP -------------------------------------
  Serial.println("Adafruit MMA8451 test!");
  if (! mma.begin()) {
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("MMA8451 found!");
  mma.setRange(MMA8451_RANGE_2_G);
  Serial.print("Range = "); Serial.print(2 << mma.getRange());
  Serial.println("G");
}

void loop() {
  Serial.println("------------Reading GPS...------------"); 
  readGPSData();
  Serial.println("------------Step Count...------------");   
  pedometerCount();
//  if (millis() - timer2 < 5000) {
//    return;
//  }
//   timer2 = millis();
 delay(1000); // Wait 1 second between transmits, could also 'sleep' here!
  Serial.println("------------Transmitting...------------"); // Send a message to rf95_server
  char tx_buf[60];
  memset(tx_buf, 0, sizeof(tx_buf));
  kylee_data.toCharArray(tx_buf, 60);
  Serial.println(tx_buf);
  Serial.println("Sending...");
  delay(10);
  rf95.send((uint8_t *)tx_buf, 60);
  Serial.println("Waiting for packet to complete...");
  delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t rx_buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(rx_buf);
  Serial.println("Waiting for reply...");
  if (rf95.waitAvailableTimeout(1000)) {
    // Should be a reply message for us now
    if (rf95.recv(rx_buf, &len)) {
      Serial.print("Got reply: ");
      Serial.println((char*)rx_buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      if (rx_buf[0] == 'N') {
        playAlarm();
      }
    }
    else {
      Serial.println("Receive failed");
    }
  }
  else {
    Serial.println("No reply, is there a listener around?");
  }
}
void readGPSData() {
//  // read data from the GPS in the 'main loop'
  char c;
  for (int i = 0; i <= 100; i++) {
//    if(GPS.available()){
      c = GPS.read();
//    }
  }  
//  char c = GPS.read();
  if (GPSECHO)
    if (c) Serial.print(c);
  if (GPS.newNMEAreceived()) {
    Serial.print(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    Serial.print("\nTime: ");
    if (GPS.hour < 10) {
      Serial.print('0');
    }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) {
      Serial.print('0');
    }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) {
      Serial.print('0');
    }
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
    if (GPS.fix) {
      Serial.println("Example code");
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      float gps_latitude = GPS.latitude;
      char gps_lat_dir = GPS.lat;
      float gps_longitude = GPS.longitude;
      char gps_lon_dir = GPS.lon;
      kylee_data = String(gps_latitude, 6) + gps_lat_dir + ", " + String(gps_longitude, 6) + gps_lon_dir + ",  " + String(GPS.day) + "/" + String(GPS.month) + "/" + String(GPS.year) + " " + String(GPS.hour) + ":" + String(GPS.minute) + ":" + String(GPS.seconds) + ", " + String(accum);
      digitalWrite(LED, HIGH);    
      delay(1000);
      digitalWrite(LED, LOW); 
    }
    Serial.println(kylee_data);
  }
}

void playAlarm() {
  tone(ALARM_PIN, 20000, 3000); // play for 3 seconds
}

void pedometerCount() {
  // Read the 'raw' data in 14-bit counts
  mma.read();
//  Serial.print("X:\t"); Serial.println(mma.x);
//  Serial.print("Y:\t"); Serial.println(mma.y);
//  Serial.print("Z:\t"); Serial.println(mma.z);
//  Serial.println();

  /* Get a new sensor event */
  sensors_event_t event;
  mma.getEvent(&event);

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("X: "); Serial.println(event.acceleration.x);
  Serial.print("Y: "); Serial.println(event.acceleration.y);
  Serial.print("Z: "); Serial.println(event.acceleration.z);
  Serial.println("m/s^2 ");

  if (event.acceleration.x >= abs(1)) {
    Serial.println("Update Step");
    accum = accum + 1;
  }
  Serial.print("Step Counter: "); Serial.println(accum);
  uint8_t o = mma.getOrientation();
  Serial.println();
  delay(500);
}
