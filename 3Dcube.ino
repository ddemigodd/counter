#include <GyverOLED.h>
//OLED
//SDA - A4
//SCL - A5
//VCC - 5v
//GND - GND
// LAZER
//D2 - VCC
//D3 - DATA/OUT
//GND - GND


GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;

uint32_t counter = 0;
byte lastButtonState = LOW;
unsigned long debounceDuration = 50; // millis
unsigned long lastTimeButtonStateChanged = 0;


void setup() {
  Serial.begin(9600);

  pinMode(2, OUTPUT);

  digitalWrite(2, HIGH);
  
  oled.init();
  oled.update();
  Wire.setClock(800000L);
  oled.clear();
  oled.update();
  oled.home();
  
  oled.print("Инициализация...");
  oled.update();
  delay(2000);
}

void dispUpd() {
  static uint32_t tmr;
  if (millis() - tmr >= 30) {
    tmr = millis();
    oled.clear();
    oled.setCursor(4,0);
    oled.setScale(4);
    oled.print(String(counter));
    oled.setScale(3);
    oled.setCursor(4,5);
    oled.print(String(counter));
    oled.update();
  }
}

void loop() {
  dispUpd();
  if (millis() - lastTimeButtonStateChanged > debounceDuration) {
      byte buttonState = digitalRead(3);
      if (buttonState != lastButtonState) {
      lastTimeButtonStateChanged = millis();
      lastButtonState = buttonState;
      if (buttonState == LOW) {
        counter = counter + 999;
      }
    }
  }
}