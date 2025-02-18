#include <GyverOLED.h>
#include <EEPROM.h>
//OLED
//SDA - A4
//SCL - A5
//VCC - 5v
//GND - GND
// LAZER
//D2 - VCC
//D3 - DATA/OUT
//GND - GND
//BTN
//D9 - BTN
#define lazerVcc 2
#define lazerPin 3
#define btnPin 9


GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;

uint32_t counter = 0;                                           // Счетчик изделий
boolean currentLaserState, lastLaserState, countIsSet, isSave;  // Текущее состояние лазера, Последнее состояние лазера, текущий тик учитан, счетчик сохранен
boolean currentBtnState, lastBtnState;                          // Текущий и предыдущее значение кнопки
uint32_t btnTimer;                                              // Время зажатия кнопки
uint32_t btnPushTime = 3000;                                    // Продолжительность нажатия на кнопку
uint32_t debounceTimer;                                         // Таймер времени против "дребезга"
uint32_t debounceDuration = 80;                                 // Время "дребезга"
uint32_t timeWork;                                              // Рабочее время
uint32_t unactiveTimer;                                         // Счетчик бездействия
uint32_t unactiveDuration = 5000;                               // Время бездействия для сброса и сохранения

uint32_t startTime = 0;                                         // Время старта работы
uint32_t lastUpdateTime = 0;                                    // Последнее обновление
uint32_t interval = 1000;                                       // Интервал 1 минута (60000 мс)
uint32_t countPerHour = 0;                                      // Количество событий за минуту
float speedPerHour = 0;                                         // Средняя скорость событий в час
boolean initTime = false;                                       // Время старта выставлено


void setup() {
  Serial.begin(9600);
  pinMode(lazerVcc, OUTPUT);
  pinMode(lazerPin, INPUT_PULLUP);
  pinMode(btnPin, INPUT_PULLUP);
  digitalWrite(2, HIGH);
  oled.init();
  Wire.setClock(400000L);
  oled.update();
  digitalWrite(lazerVcc, HIGH);
  EEPROM.get(0, counter);                                         // Кол-во сохраненное в памяти
}

void dispUpd() {
  static uint32_t tmr;
  if (millis() - tmr >= 100) {
    tmr = millis();
    oled.clear();
    oled.setCursor(4, 0);
    oled.setScale(4);
    oled.print(counter);

    oled.setScale(3);
    oled.setCursor(4, 5);
    oled.print((int)speedPerHour);

    oled.setScale(2);
    oled.setCursor(94, 6);
    oled.print("час");
    oled.update();
  }
}

void loop() {
  dispUpd();

  currentBtnState = !digitalRead(btnPin);
  currentLaserState = !digitalRead(lazerPin);

  if (millis() - unactiveTimer > unactiveDuration && !isSave) {
    initTime = false;
    countPerHour = 0;
    isSave = true;
    EEPROM.put(0, counter);
  }

  if (currentLaserState == LOW || lastLaserState == LOW) {
    debounceTimer = millis();
    countIsSet = false;
  }

  if (currentBtnState == LOW || lastBtnState == LOW) {
    btnTimer = millis();
  }
  if (currentBtnState == HIGH && lastBtnState == HIGH && millis() - btnTimer > btnPushTime) {
    counter = 0;
    return;
  }

  if (!countIsSet && currentLaserState == HIGH && lastLaserState == HIGH && millis() - debounceTimer > debounceDuration) {
    if (!initTime) {
      startTime = millis();
      lastUpdateTime = millis();
      initTime = !initTime;
      isSave = false;
    }
    counter++;
    countPerHour++;
    countIsSet = true;
    unactiveTimer = millis();
  }

  if (millis() - lastUpdateTime >= interval) {
    timeWork = millis() - startTime;
    speedPerHour = 3600000 * countPerHour / timeWork;
    lastUpdateTime = millis();
  }

  lastLaserState = currentLaserState;
  lastBtnState = currentBtnState;
}
