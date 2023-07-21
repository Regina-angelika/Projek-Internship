// Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

// Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Libraries for Ultrasonic Sensor
#include <NewPing.h>

// Define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

// 433E6 for Asia
// 866E6 for Europe
// 915E6 for North America
#define BAND 433E6

// OLED pins
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Ultrasonic Sensor Pins
#define TRIGGER_PIN 21
#define ECHO_PIN 13
#define MAX_DISTANCE 400 // Maksimum jarak pengukuran (dalam cm)

NewPing ultrasonicSensor(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// Packet counter
int readingID = 0;

int counter = 0;
String LoRaMessage = "";

float distance = 0;

// Initialize OLED display
void startOLED()
{
  // Reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  // Initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false))  // Address 0x3C for 128x32
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LORA SENDER");
}

// Initialize LoRa module
void startLoRA()
{
  // SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  // Setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  while (!LoRa.begin(BAND) && counter < 10)
  {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10)
  {
    // Increment readingID on every new reading
    readingID++;
    Serial.println("Starting LoRa failed!");
  }
  Serial.println("LoRa Initialization OK!");
  display.setCursor(0, 10);
  display.clearDisplay();
  display.print("LoRa Initializing OK!");
  display.display();
  delay(2000);
}

void getUltrasonicReading()
{
  unsigned int uS = ultrasonicSensor.ping(); // Send ultrasonic pulse and get the response time in microseconds
  distance = uS / US_ROUNDTRIP_CM; // Calculate the distance in centimeters
}

void sendReadings()
{
  LoRaMessage = String(readingID) + "/" + String(distance);
  // Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("LoRa packet sent!");
  display.setCursor(0, 20);
  display.print("Distance:");
  display.setCursor(72, 20);
  display.print(distance);
  display.setCursor(0, 50);
  display.print("Reading ID:");
  display.setCursor(66, 50);
  display.print(readingID);
  display.display();
  Serial.print("Sending packet: ");
  Serial.println(readingID);
  readingID++;
}

void setup(){
  // Initialize Serial Monitor
  Serial.begin(115200);
  startOLED();
  startLoRA();
}

void loop(){
  getUltrasonicReading();
  sendReadings();
  delay(10000);
  unsigned int distance = ultrasonicSensor.ping_cm(); // Mengukur jarak dalam cm

    Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  String waterLevel;
  if (distance <= 100) {
    waterLevel = "Level 1 meter";
  } else if (distance <= 200) {
    waterLevel = "Level 2 meter";
  } else if (distance >= 300) {
    waterLevel = "Level 3 meter";
  } else {
    waterLevel = "Ketinggian normal";
  }
  LoRa.beginPacket();
  LoRa.print(waterLevel);
  LoRa.endPacket();
}

