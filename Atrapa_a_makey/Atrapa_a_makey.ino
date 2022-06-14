#include"LIS3DHTR.h"
#include"TFT_eSPI.h"
#include"Free_Fonts.h"

LIS3DHTR<TwoWire> lis;
TFT_eSPI tft;

float x_values, y_values, z_values;
bool isOnTarget = false;
int MAKEYpos = 0;
unsigned long lastChangeTime = 0;
int MAKEYwaitTime = 0;
int level = 0;

char cadena_1[10];

uint32_t color_1 = TFT_RED;
uint32_t color_2 = 0xD85C5D;

int operation_x = 0;
int operation_y = 0;

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
  tft.fillScreen(TFT_WHITE);

  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  pinMode(WIO_BUZZER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);

  restar();
}

void loop() {

  if (MAKEYwaitTime  == 0) {
    menu();
  } else {
    game();
  }
}

void menu()
{
  tft.setTextColor(TFT_BLACK);
  tft.setFreeFont(FF22);
  tft.drawString("Select the level", 75, 80);

  tft.setFreeFont(FF21);
  tft.drawString("Easy", 40, 130);
  tft.drawString("Normal", 130, 130);
  tft.drawString("Hard", 240, 130);

  tft.drawString("C", 55, 170);
  tft.drawString("Button", 30, 190);
  tft.drawRect( 20, 160, 80, 60, TFT_BLACK);

  tft.drawString("B", 155, 170);
  tft.drawString("Button", 130, 190);
  tft.drawRect( 120, 160, 80, 60, TFT_BLACK);

  tft.drawString("A", 260, 170);
  tft.drawString("Button", 235, 190);
  tft.drawRect( 225, 160, 80, 60, TFT_BLACK);

  if (digitalRead(WIO_KEY_A) == LOW) {
    Serial.println("A Key pressed");
    onButton(260, 170, "A");
    MAKEYwaitTime = 1000;
    delay(2000);
    restar();
  }
  else if (digitalRead(WIO_KEY_B) == LOW) {
    Serial.println("B Key pressed");
    onButton(155, 170, "B");
    MAKEYwaitTime = 2000;
    delay(2000);
    restar();
  }
  else if (digitalRead(WIO_KEY_C) == LOW) {
    Serial.println("C Key pressed");
    onButton(55, 170, "C");
    MAKEYwaitTime = 4000;
    delay(2000);
    restar();
  }
}

void  onButton(int x, int y, char *b)
{
  tft.fillRect( x - 35, y - 10, 80, 60, color_2);
  tft.drawString(b, x, y);
  tft.drawString("Button", x - 25, y + 20);
}

void restar()
{
  tft.fillScreen(TFT_WHITE);
  tft.fillRect( 0, 0, 320, 60, color_1);
  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(FF23);
  tft.drawString("Maker Camp", 10, 15);
}

void game()
{
  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(FF21);

  tft.fillRect( 240, 10, 75, 17, color_1);
  sprintf(cadena_1, "%d", level);

  tft.drawString(cadena_1, 270, 12);
  tft.drawString("Points", 250, 32);

  x_values = lis.getAccelerationX();
  y_values = lis.getAccelerationY();

  if (lastChangeTime + MAKEYwaitTime < millis()) {
    generateRandomMAKEY();
    level = 0;
  }

  drawHoles();

  if (digitalRead(WIO_5S_PRESS) == LOW) {
    if (isOnTarget) {
      Serial.println("Hit!");
      digitalWrite(LED_BUILTIN, HIGH);
      analogWrite(WIO_BUZZER, 128);
      delay(300);
      digitalWrite(LED_BUILTIN, LOW);
      analogWrite(WIO_BUZZER, 0);
      level++;
    }
  }

  if (digitalRead(WIO_5S_PRESS) == LOW) {
    if (isOnTarget) {
      Serial.println("Hit!");
      digitalWrite(LED_BUILTIN, HIGH);
      analogWrite(WIO_BUZZER, 128);
      delay(300);
      digitalWrite(LED_BUILTIN, LOW);
      analogWrite(WIO_BUZZER, 0);
      level++;
    } else {
      Serial.println("Miss!");
      level = 0;
      delay(300);
    }
    generateRandomMAKEY();
  }
}

void generateRandomMAKEY() {
  MAKEYpos = random(0, 9);
  lastChangeTime = millis();
}

void drawHoles() {
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      bool isShaded = (((int)(y_values * -5) + operation_y) == i) && (((int)(x_values * 5) - operation_x) == j);

      calibrate_game();

      int index = (i + 1) + (j + 1) * 3;
      tft.fillRect(111 + i * 108, 125 + j * 60, 100, 52,  isShaded ? color_2 : TFT_DARKGREY);

      if (index == MAKEYpos) {
        isOnTarget = isShaded;
        if (isShaded == false) {
          drawMAKEY(111 + i * 108, 125 + j * 60);
        }
      }

    }
  }
}

void calibrate_game() {
  if (digitalRead(WIO_KEY_C) == LOW) {
    Serial.println("C Key pressed");
    operation_x = -((int)(x_values * -5));
    operation_y = -((int)(y_values * -5));
  }
}

void drawMAKEY(int x, int y) {
  tft.fillRect(x, y, 100, 52, TFT_RED);
  tft.fillCircle(x + 50, y + 26, 20, TFT_WHITE);
  tft.setTextColor(TFT_RED);
  tft.setFreeFont(FF22);
  tft.drawString("M", x + 40, y + 17);
  tft.fillRect(x, y + 26 - 4, 30, 8, TFT_WHITE);
  tft.fillRect(x + 60, y + 26 - 4, 40, 8, TFT_WHITE);
}
