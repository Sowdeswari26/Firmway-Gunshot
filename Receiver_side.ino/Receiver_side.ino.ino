#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define VIBRATION_PIN  4     // Vibration motor (via transistor)
#define LED_PIN        2     // Built-in LED

#define LORA_SS        5
#define LORA_RST       14
#define LORA_DIO0      2
#define LORA_FREQ      433E6   // Must match sender

// ── SETUP ─────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  pinMode(VIBRATION_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(VIBRATION_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 failed"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(12, 20);
  display.println(F("SOLDIER ALERT"));
  display.setCursor(25, 35);
  display.println(F("STANDBY"));
  display.display();

  // LoRa init
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa init failed!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("LoRa FAILED"));
    display.display();
    while(1);
  }

  Serial.println("LoRa Receiver Ready @ 433 MHz");
  display.clearDisplay();
  display.setCursor(10, 25);
  display.println(F("LINK ACTIVE"));
  display.display();
}

// ── LOOP ──────────────────────────────────────────────────
void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String packet = "";
    while (LoRa.available()) {
      packet += (char)LoRa.read();
    }

    Serial.print("Received: ");
    Serial.println(packet);

    if (packet.startsWith("DETECT:")) {
      parseAndShowAlert(packet);
    }
  }
}

// ── Parse & Display Alert ─────────────────────────────────
void parseAndShowAlert(String pkt) {
  // Remove "DETECT:"
  pkt = pkt.substring(7);

  // Split by ':'
  int idx1 = pkt.indexOf(':');
  int idx2 = pkt.indexOf(':', idx1 + 1);
  int idx3 = pkt.indexOf(':', idx2 + 1);
  int idx4 = pkt.indexOf(':', idx3 + 1);
  int idx5 = pkt.indexOf(':', idx4 + 1);

  String direction    = (idx1 != -1) ? pkt.substring(0, idx1) : "UNKNOWN";
  String distance     = (idx2 != -1) ? pkt.substring(idx1 + 1, idx2) : "?";
  String obsLat       = (idx3 != -1) ? pkt.substring(idx2 + 1, idx3) : "N/A";
  String obsLon       = (idx4 != -1) ? pkt.substring(idx3 + 1, idx4) : "N/A";
  String shooterLat   = (idx5 != -1) ? pkt.substring(idx4 + 1, idx5) : "N/A";
  String shooterLon   = (idx5 != -1) ? pkt.substring(idx5 + 1) : "N/A";

  // ── Serial Monitor Output ───────────────────────────────
  Serial.println("\n=== THREAT DETECTED ===");
  Serial.print("Direction: "); Serial.println(direction);
  Serial.print("Distance : "); Serial.print(distance); Serial.println(" m");
  Serial.print("Observer : "); Serial.print(obsLat); Serial.print(", "); Serial.println(obsLon);
  Serial.print("Shooter  : "); Serial.print(shooterLat); Serial.print(", "); Serial.println(shooterLon);
  Serial.print("RSSI     : "); Serial.print(LoRa.packetRssi()); Serial.println(" dBm");
  Serial.println("===================\n");

  // ── OLED Display ────────────────────────────────────────
  display.clearDisplay();

  // Top warning bar (inverted)
  display.fillRect(0, 0, SCREEN_WIDTH, 18, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(2);
  display.setCursor(8, 2);
  display.println(F("THREAT"));

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // Directional arrow
  bool isLeft = (direction == "LEFT");
  int arrowX = isLeft ? 8 : (SCREEN_WIDTH - 60);
  drawBigArrow(arrowX, 22, isLeft);

  // Hazard icon
  drawHazardIcon(SCREEN_WIDTH - 38, 20);

  // Main info
  display.setTextSize(2);
  display.setCursor(4, 42);
  display.print(distance);
  display.setTextSize(1);
  display.print(F(" m"));

  display.setTextSize(1);
  display.setCursor(4, 28);
  display.print(F("DIR: "));
  display.print(direction);

  // GPS info
  display.setCursor(0, 56);
  display.print(obsLat.substring(0, 8));
  display.print(F(","));
  display.print(obsLon.substring(0, 8));

  display.setCursor(SCREEN_WIDTH - 55, 56);
  display.print(F("RSSI "));
  display.print(LoRa.packetRssi());
  display.print(F("dB"));

  display.display();

  // Alert: vibration + LED
  vibrateAlert(4);
}

// ── Draw large directional arrow ─────────────────────────
void drawBigArrow(int x, int y, bool left) {
  int len = 48;
  int hgt = 18;

  if (left) {
    display.fillRect(x + 12, y + 4, len, 10, SSD1306_WHITE);
    display.fillTriangle(x, y + 9, x + 12, y, x + 12, y + 18, SSD1306_WHITE);
  } else {
    display.fillRect(x, y + 4, len, 10, SSD1306_WHITE);
    display.fillTriangle(x + len, y + 9, x + len - 12, y, x + len - 12, y + 18, SSD1306_WHITE);
  }
}

// ── Draw hazard warning icon ─────────────────────────────
void drawHazardIcon(int x, int y) {
  for (int i = 0; i < 4; i++) {
    display.drawTriangle(x + i, y + 26 + i,
                         x + 34 - i, y + i,
                         x + 34 - i, y + 26 + i,
                         SSD1306_WHITE);
  }
  display.fillRect(x + 14, y + 6, 7, 12, SSD1306_WHITE);
  display.fillRect(x + 14, y + 22, 7, 7, SSD1306_WHITE);
}

// ── Vibration & LED alert ────────────────────────────────
void vibrateAlert(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(VIBRATION_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    delay(180);
    digitalWrite(VIBRATION_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    delay(120);
  }
}
