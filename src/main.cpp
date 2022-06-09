#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#define LOG_PERIOD 15000 // Logging period in milliseconds, recommended value 15000-60000.
#define MAX_PERIOD 60000 // Maximum logging period without modifying this sketch

LiquidCrystal_I2C lcd(0x27, 20, 4);

unsigned long counts; // variable for GM Tube events
float cpm;            // variable for CPM
float uSv;
float multiplier; // variable for calculation CPM in this sketch
float maxPer = 60000.0;
int charge;
const float conFactor = 0.008120370;
unsigned long previousMillis; // variable for time measurement
unsigned long currentMillis;
bool lastbtn = false;

void tube_impulse()
{ // subprocedure for capturing events from Geiger Kit
  counts++;
}

void getCharge()
{
  int value = analogRead(2);
  float voltage = value * 5.2 / 1023.0;
  if (voltage > 4)
  {
    lcd.print((char)0xFF);
    lcd.print((char)0xFF);
    lcd.print((char)0xFF);
    lcd.print((char)0xFF);
    lcd.print((char)0x7D);
  }
  else if (voltage > 3.7)
  {
    lcd.print((char)0xFF);
    lcd.print((char)0xFF);
    lcd.print((char)0xFF);
    lcd.print((char)0xDB);
    lcd.print((char)0x7D);
  }
  else if (voltage > 3.4)
  {
    lcd.print((char)0xFF);
    lcd.print((char)0xFF);
    lcd.print((char)0xDB);
    lcd.print((char)0xDB);
    lcd.print((char)0x7D);
  }
  else if (voltage > 3)
  {
    lcd.print((char)0xFF);
    lcd.print((char)0xDB);
    lcd.print((char)0xDB);
    lcd.print((char)0xDB);
    lcd.print((char)0x7D);
  }
  else
  {
    lcd.print((char)0xDB);
    lcd.print((char)0xDB);
    lcd.print((char)0xDB);
    lcd.print((char)0xDB);
    lcd.print((char)0x7D);
  }
}

void setupLogger()
{
  lcd.clear();
  lcd.setCursor(11, 1);
  getCharge();
  lcd.setCursor(0, 0);
  currentMillis = millis();
  lcd.print("---- uSv/h");
  while (millis() - currentMillis < LOG_PERIOD)
  {
  }
  cpm = counts * multiplier;
}

void updateLcd()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(uSv, 2);
  lcd.print(" uSv/h");
  lcd.setCursor(11, 1);
  getCharge();
}

void update()
{
  multiplier = (millis() - currentMillis) / maxPer;
  cpm = counts / multiplier;
  uSv = cpm * conFactor;

  updateLcd();

  Serial.print("multiplier = ");
  Serial.println(multiplier);
  Serial.print("cpm = ");
  Serial.println(cpm);
  Serial.print("counts = ");
  Serial.println(counts);
  Serial.print("millis = ");
  Serial.println(millis());
  Serial.print("current millis = ");
  Serial.println(currentMillis);
  Serial.println();
}

void setup()
{             // setup subprocedure
  lcd.init(); // initialize the lcd
  lcd.backlight();
  counts = 0;
  cpm = 0;
  multiplier = MAX_PERIOD / LOG_PERIOD; // calculating multiplier, depend on your log period
  Serial.begin(9600);
  attachInterrupt(0, tube_impulse, FALLING); // define external interrupts

  setupLogger();
}

void loop()
{ // main cycle

  if (millis() - previousMillis > 1000)
  {
    previousMillis = millis();
    update();
  }

  if (digitalRead(4) == true && lastbtn == false)
  {
    counts = 0;
    setupLogger();
  }
  lastbtn = digitalRead(4);
}