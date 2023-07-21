//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 433E6

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// Initialize variables to get and save LoRa data
int rssi;
String loRaMessage;
String readingID;

// Replaces placeholder with RSSI value
String processor(const String& var){
  if(var == "RRSI"){
    return String(rssi);
  }
  return String();
}

//Initialize OLED display
void startOLED(){
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA SENDER");
}

//Initialize LoRa module
void startLoRA(){
  int counter;
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    Serial.println("Starting LoRa failed!"); 
  }
  Serial.println("LoRa Initialization OK!");
  display.setCursor(0,10);
  display.clearDisplay();
  display.println("LoRa Receiver");
  display.println("Initialized");
  display.display();
}

//Function to get LoRa packet and extract the sensor readings
void getLoRaData() {
  Serial.print("Lora packet received: ");
  // Read packet
  while (LoRa.available()) {
    String LoRaData = LoRa.readString();
    // LoRaData format: readingID/sensorData
    // String example: 1/27.43
    Serial.print(LoRaData); 
    
    // Get readingID and sensor data
    int pos = LoRaData.indexOf('/');
    readingID = LoRaData.substring(0, pos);
    loRaMessage = LoRaData.substring(pos +1);
  }
  // Get RSSI
  rssi = LoRa.packetRssi();
  Serial.print(" with RSSI ");    
  Serial.println(rssi);
}

//LED Pin & Buzzer
int ledPin1 = LED_BUILTIN;
int ledPin2 = 25; // GPIO 25
int ledPin3 = 32; // GPIO 32
int buzzerPin = 33; // GPIO 33

void setup() {
  //Led & buzzer set
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  digitalWrite(buzzerPin, LOW);

  LoRa.begin(433E6);

  // Initialize Serial Monitor
  Serial.begin(115200);
  startOLED();
  startLoRA();
}

void loop() {
  // Check if there are LoRa packets available
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String waterLevel = "";
    while (LoRa.available()) {
      waterLevel += LoRa.readString();
    getLoRaData();

    // Update OLED display with received data
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Received Data:");
    display.print("ID: ");
    display.println(readingID);//{{75}};
    display.print("Message: ");
    display.println(loRaMessage);
    display.print("RSSI: ");
    display.println(rssi);
    display.display();
  }
  //LED & Buzzer Kondisi
  //LoRa.display->clear();
  //LoRa.display->drawString(0, 0, waterLevel);
  //LoRa.display->display();

  if (waterLevel.equals("Level 1 meter")) {
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, HIGH);
      digitalWrite(ledPin3, HIGH);
      digitalWrite(buzzerPin, HIGH);
    } else if (waterLevel.equals("Level 2 meter")) {
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, HIGH);
      digitalWrite(ledPin3, LOW);
      digitalWrite(buzzerPin, LOW);
    } else if (waterLevel.equals("Level 3 meter")) {
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, LOW);
      digitalWrite(ledPin3, LOW);
      digitalWrite(buzzerPin, LOW);
    } else {
      digitalWrite(ledPin1, LOW);
      digitalWrite(ledPin2, LOW);
      digitalWrite(ledPin3, LOW);
      digitalWrite(buzzerPin, LOW);
    }
  }
  //delay(1000);
}

