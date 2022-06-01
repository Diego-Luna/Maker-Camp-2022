#include"LIS3DHTR.h"
LIS3DHTR<TwoWire> lis;
#include"TFT_eSPI.h"
TFT_eSPI tft;

void setup() {
  Serial.begin(115200);
  lis.begin(Wire1);

  if (!lis) {
    Serial.println("ERROR");
    while (1);
  }
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); //Data output rate
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); //Scale range set to 2g

  tft.begin();
  tft.setRotation(3);

  tft.fillScreen(0x2d40);

  pinMode(WIO_5S_PRESS, INPUT_PULLUP);

  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
}

int lastX = 0;
int lastY = 0;
float x_values, y_values, z_values;
bool isOnTarget = false;
int LEDpos = 0;
unsigned long lastChangeTime = 0;
const int LEDwaitTime = 2000;

void loop() {

  x_values = lis.getAccelerationX();
  y_values = lis.getAccelerationY();

  if (lastChangeTime + LEDwaitTime < millis()) {
    digitalWrite(D0, HIGH);
    delay(300);
    digitalWrite(D0, LOW);
    generateRandomLED();
  }

  drawHoles();
  if (digitalRead(WIO_5S_PRESS) == LOW) {
    if (isOnTarget) {
      Serial.println("Hit!");
      digitalWrite(D1, HIGH);
      delay(300);
      digitalWrite(D1, LOW);
    } else {
      Serial.println("Miss!");
      digitalWrite(D0, HIGH);
      delay(300);
      digitalWrite(D0, LOW);
    }
    generateRandomLED();
  }
}

void generateRandomLED() {
  LEDpos = random(0, 9);
  lastChangeTime = millis();
}

void drawHoles() {
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      bool isShaded = ((int)(y_values * -5) == i) && ((int)(x_values * 5) == j);
      int index = (i + 1) + (j + 1) * 3;
      tft.fillCircle(160 + i * 80, 120 + j * 80, 30, isShaded ? TFT_ORANGE : TFT_DARKGREY);
      if (index == LEDpos) {
        isOnTarget = isShaded;
        drawLED(160 + i * 80, 115 + j * 80);
      }
    }
  }
}

void drawLED(int x, int y) {
  tft.fillCircle(x, y, 12, TFT_RED);
  tft.fillRect(x - 12, y, 25, 19, TFT_RED);
  tft.fillRect(x - 8, y + 24, 4, 10, TFT_LIGHTGREY);
  tft.fillRect(x + 5, y + 24, 4, 10, TFT_LIGHTGREY);
  tft.fillRect(x - 14, y + 19, 29, 6, 0xa800);
  tft.fillRoundRect(x + 7, y - 1, 3, 10, 2, TFT_WHITE);
}
