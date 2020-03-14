// WebServer stuff
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
// LCD & Sensor stuff
#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Define PIN & TYPE for temp sensor
#define DHTPIN 13
#define DHTTYPE DHT22

// WiFi creds
#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

DHT dht(DHTPIN, DHTTYPE);

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;

unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 10000; 

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 14, en = 12, d4 = 5, d5 = 4, d6 = 0, d7 = 2; // WeMos D1 Mini
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  // Start WiFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // Init LED as OUTPUT
  pinMode(LED_BUILTIN, OUTPUT);
  dht.begin();
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print some stuff to the LCD.
  lcd.print("Temp:");
  lcd.setCursor(14,0);
  lcd.print((char)223+String("C")); // degree sign followed by C
  lcd.setCursor(0, 1);
  lcd.print("Humidity:");
  lcd.setCursor(15,1);
  lcd.print("%");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/temp", []() {
    server.send(200, "application/json", "{\"Temp\":"+String(t)+",\"Humidity\":"+String(h)+"}");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {

  // Turn off onboard LED
  digitalWrite(LED_BUILTIN, HIGH);
  
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
       Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      lcd.setCursor(10, 0);
      lcd.print(t, 1);
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
       Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      lcd.setCursor(10, 1);
      lcd.print(h, 1);
    }
  }

  server.handleClient();

}
