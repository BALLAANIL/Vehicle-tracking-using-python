#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

// GPS
SoftwareSerial gpsSerial(4, 3); // RX, TX
Adafruit_GPS GPS(&gpsSerial);

// GSM
SoftwareSerial gsmSerial(7, 8); // RX, TX

// Fuel Sensor
int fuelSensorPin = A0;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  gsmSerial.begin(9600);
  pinMode(fuelSensorPin, INPUT);

  // Initialize GPS module
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate

  // Initialize GSM module
  gsmSerial.println("AT");
  delay(1000);
  gsmSerial.println("AT+CPIN?");
  delay(1000);
  gsmSerial.println("AT+CREG?");
  delay(1000);
  gsmSerial.println("AT+CGATT?");
  delay(1000);
  gsmSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  delay(1000);
  gsmSerial.println("AT+SAPBR=3,1,\"APN\",\"your_apn\""); // Replace with your APN
  delay(1000);
  gsmSerial.println("AT+SAPBR=1,1");
  delay(1000);
  gsmSerial.println("AT+SAPBR=2,1");
  delay(1000);
  gsmSerial.println("AT+HTTPINIT");
  delay(1000);
}

void loop() {
  char c = GPS.read();
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA())) {
      return;
    }
  }

  if (GPS.fix) {
    float latitude = GPS.latitudeDegrees;
    float longitude = GPS.longitudeDegrees;
    int fuelLevel = analogRead(fuelSensorPin);

    String data = "latitude=" + String(latitude, 6) + "&longitude=" + String(longitude, 6) + "&fuel_level=" + String(fuelLevel);

    // Send data to server
    gsmSerial.println("AT+HTTPPARA=\"URL\",\"http://your_server_ip:5000/data\""); // Replace with your server IP
    delay(1000);
    gsmSerial.println("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"");
    delay(1000);
    gsmSerial.println("AT+HTTPDATA=" + String(data.length()) + ",10000");
    delay(1000);
    gsmSerial.print(data);
    delay(1000);
    gsmSerial.println("AT+HTTPACTION=1");
    delay(1000);
  }

  delay(10000);  // Send data every 10 seconds
}
