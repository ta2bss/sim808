/* If you send an SMS by typing "Location" to the phone line connected to the device, the device will send you its location.
Additionally, it will send you its location via SMS at the time intervals you enter below.
Don't forget to put gsm number to send sms. */


#include <SoftwareSerial.h>

int minutesinterval = 60;  // How often should SMS be sent? MINUTES (0 for non-repetitive SMS)
//String GSMNo = "+12345678"; // Phone number to send SMS - with international code (starts with +) 

int counter = 0;
String incomingsms;
int period = 10000;
unsigned long myTime = 0;
String Output;
String startString = "CGPSINF:";
String endString = "OK";
int latitudeDegrees, longitudeDegrees;
float latitudeMinutes, longitudeMinutes;
SoftwareSerial GPSmodule(3, 4); //rx tx , It is recommended to divide the voltage with 3.3 KOhm and 5.6 KOhm resistors
int lastcount = (minutesinterval * 5);

void setup() {
  delay(1000);
  Serial.begin(9600);
  GPSmodule.begin(9600);
  delay(2000);
  Serial.println("GPSmodule ready...");
  GPSmodule.println("AT");
  delay(50);
  GPSmodule.println("AT+CMEE=1");
  delay(50);
  GPSmodule.println("AT+CMGF=1");
  delay(50);
  GPSmodule.println("AT+CNMI=2,2,0,0,0");
  delay(50);
  GPSmodule.println("AT+CGPSPWR=1");
  delay(50);
}

void loop() {
  incomingsms = GPSmodule.readString();
  GPSmodule.println("AT+CGPSINF=0");
  delay(50);
  if (Serial.available() > 0)
    GPSmodule.write(Serial.read());
  if (GPSmodule.available() > 0)
    Serial.write(GPSmodule.read());
  if (GPSmodule.available() > 0) {
    Output = GPSmodule.readString();
  }
  Output.replace("\n", "");
  Output.replace(" ", "");
  int startIndex = Output.indexOf(startString);
  if (startIndex != -1) {
    startIndex += startString.length();
    int endIndex = Output.indexOf(endString, startIndex);
    if (endIndex != -1) {
      String extractedOutput = Output.substring(startIndex, endIndex);
      Serial.println("Extracted Output: " + extractedOutput);
      parseAndPrint(extractedOutput);
    } else {
      Serial.println("GPS is not ready");
    }
  } else {
    Serial.println("GPS is not ready");
  }
  delay(50);
  myTime = millis();
  while (millis() < myTime + period) {
  }
  counter = counter + 1;
  Serial.println(counter);
  if (counter >= lastcount) { counter = 0; }
}

void parseAndPrint(String extractedOutput) {
  char *token = strtok(const_cast<char *>(extractedOutput.c_str()), ",");

  for (int i = 0; token != NULL; i++) {
    switch (i) {
      case 1:
        {
          int degrees = atoi(token) / 100;
          double minutes = fmod(atof(token), 100);
          latitudeDegrees = degrees;
          latitudeMinutes = minutes;
          break;
        }

      case 2:
        {
          int degrees = atoi(token) / 100;
          double minutes = fmod(atof(token), 100);
          longitudeDegrees = degrees;
          longitudeMinutes = minutes;
          break;
        }
    }
    token = strtok(NULL, ",");
  }
  float latitude = latitudeDegrees + (latitudeMinutes / 60);
  float longitude = longitudeDegrees + (longitudeMinutes / 60);
  Serial.print("Google Maps Link: ");
  Serial.print("https://www.google.com/maps?q=");
  Serial.print(latitude, 6);
  Serial.print(",");
  Serial.println(longitude, 6);
  String googleMapsLink = "https://www.google.com/maps?q=" + String(latitude, 6) + "," + String(longitude, 6);
  if (incomingsms.indexOf("Location") >= 0) { sendSMS(googleMapsLink); }
  if (counter == 4) { sendSMS(googleMapsLink); }
}

void sendSMS(String message) {
  GPSmodule.println("AT+CMGS=\"" + GSMNo + "\"");
  delay(100);
  GPSmodule.print(message);
  delay(100);
  Serial.println(message);
  GPSmodule.println((char)26);
  delay(50);
  GPSmodule.println();
  delay(5000);
}