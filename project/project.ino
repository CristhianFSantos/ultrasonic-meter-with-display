/*****************************************************[ LIBRARIES ]*****************************************************/
#include <LiquidCrystal_I2C.h>

/*****************************************************[ DEFINITIONS ]*****************************************************/
#define DISPLAY_ADDRESS 0x27
#define TRIGGER_PIN 5
#define ECHO_PIN 4

/*****************************************************[ PROPERTIES ]*****************************************************/
const int DISPLAY_SIZE[2] = { 16, 2 };
const int MAX_DISTANCE_HCSR04 = 30;
const LiquidCrystal_I2C LCD(DISPLAY_ADDRESS, DISPLAY_SIZE[0], DISPLAY_SIZE[1]);

float DISTANCE_RAW = 0;

/*****************************************************[ STANDARD ARDUINO METHODS ]*****************************************************/
void setup() {
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  LCD.begin(DISPLAY_SIZE[0], DISPLAY_SIZE[1]);
  LCD.backlight();
}

void loop() {
  DISTANCE_RAW = getDistanceHCSR04(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE_HCSR04);
  String result = formatingFloat(DISTANCE_RAW, 2, 1);

  LCD.clear();
  if (DISTANCE_RAW > 0) {
    printLcdDistanceInCM(LCD, result, 0);
    printProgressBar(LCD, DISTANCE_RAW, 1, MAX_DISTANCE_HCSR04, DISPLAY_SIZE[0]);
  } else {
    printLcdDistanceError(LCD, MAX_DISTANCE_HCSR04);
  }

  delay(500);
}

/*****************************************************[ METHODS ]*****************************************************/
float getDistanceHCSR04(int triggerPin, int echoPin, int maxDistance) {
  const int delays[2] = { 2, 10 };

  digitalWrite(triggerPin, LOW);
  delayMicroseconds(delays[0]);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(delays[1]);
  digitalWrite(triggerPin, LOW);

  const float inchToCM = 0.0343;
  float result = (pulseIn(echoPin, HIGH) * inchToCM) / 2;
  return (result >= 0 && result <= maxDistance) ? result : 0;
}

String formatingFloat(float value, int firstCase, int lastCase) {
  char text[6] = "";
  String result = "";
  dtostrf(value, firstCase, lastCase, text);
  return result += text;
}

void printLcdDistanceInCM(LiquidCrystal_I2C lcd, String text, int columnPrint) {
  lcd.setCursor(0, 0);
  lcd.print("DISTANCE: " + text + "CM");
}

void printLcdDistanceError(LiquidCrystal_I2C lcd, int maxDistance) {
  LCD.setCursor(0, 0);
  LCD.print("OUT OF REACH");
  LCD.setCursor(0, 1);
  LCD.print("MAX RANGE: " + String(maxDistance) + "CM");
}

void printProgressBar(LiquidCrystal_I2C lcd, float value, int columnPrint, int maxValue, int maxColumnsDisplay) {
  const int maxPercent = 100;
  int progress = ceil((ceil(value) * maxPercent) / maxValue) * (maxColumnsDisplay - 1) / maxPercent;

  lcd.setCursor(0, columnPrint);
  for (int i = 0; i < progress; i++) {
    lcd.write(255);
  }
}