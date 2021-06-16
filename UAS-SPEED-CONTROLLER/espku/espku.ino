#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include "pinku.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer mysensor;

DHT dht(pin_suhu, type_suhu);
int mySpeed = 0; //Motor Speed Now
float mySuhu;

//Milis Setting
const long interval = 2000;
unsigned long before = 0;

BLYNK_WRITE(V15) {
  int slider = param.asInt();
  mySpeed = slider;
  Serial.print("Slider Value: "); Serial.println(slider);
  runMotor(slider);
}
void sendSensor() {
   Blynk.virtualWrite(13, mySuhu);
   Blynk.virtualWrite(14, mySpeed);
}
void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  dht.begin();

  pinMode(motorpin, OUTPUT);
  digitalWrite(motorpin, HIGH);

  ledcSetup(channel, freq, bits);
  ledcAttachPin(motorpwm, 0);

  WiFi.begin(ssid, pass);
  String statusWifi = "Connected";
  int retry = 1;
  while (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0, 0);
    lcd.print("Connecting WiFi");
    lcd.setCursor(1, 1);
    lcd.print(retry);

    if (retry > 100) {
      statusWifi = "Error";
      break;
    }
    else {
      retry++;
    }
    delay(500);
  }

  Blynk.begin(myblynk, ssid, pass);
  mysensor.setInterval(3000L, sendSensor);
}
void loop() {
  mySuhu = getSuhu();
  mySpeed = getSpeed();

  unsigned long now = millis();
  if (now - before >= interval) {
    tampil(mySuhu, mySpeed);
    before = now;
  }

  Blynk.run();
  mysensor.run();
}
void runMotor(int mspeed) {
  mySpeed = mspeed;
  int convert = map(mspeed, 0, 100, motorSlow, motorFast);
  ledcWrite(0, convert);
}
void tampil(float temp, int kecepatan) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Speed :");
  lcd.print(kecepatan);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Temp  :");
  lcd.print(temp);
  lcd.print((char)223);
  lcd.print("C");
}
float getSuhu() {
  float val = dht.readTemperature();
  return val;
}
int getSpeed() {
  return mySpeed;
}
