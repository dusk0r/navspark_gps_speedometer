#include "TwoWire.h"
#include "sti_gnss_lib.h"

short sevenSegmentCodes[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67 };
short pointCode = 0x80;

void writeSpeed(float speed) {
  int hundred = (int)speed / 100;
  int tens = ((int)speed / 10) % 10;
  int single = (int)speed % 10;
  int decimal = (int)(speed*10) % 10;

  byte first = (hundred > 0) ? sevenSegmentCodes[hundred] : 0x00;
  byte second = (hundred > 0 || tens > 0) ? sevenSegmentCodes[tens] : 0x00;
  byte third = sevenSegmentCodes[single] + pointCode;
  byte forth = sevenSegmentCodes[decimal];

  twMaster.beginTransmission(0x70);
  twMaster.write(0x00);
  twMaster.write(first); // First segment
  twMaster.write(0x00);
  twMaster.write(second); // Second segment
  twMaster.write(0x00);
  twMaster.write(0x00); // 0x02 activates :
  twMaster.write(0x00);
  twMaster.write(third); // Third segment
  twMaster.write(0x00);
  twMaster.write(forth); // Fourth segment
  twMaster.write(0x00);
  twMaster.write(0x00);
  twMaster.write(0x00);
  twMaster.write(0x00);
  twMaster.write(0x00);
  twMaster.write(0x00);
  twMaster.write(0x00);
  twMaster.endTransmission();
}

void clearScreen() {
  twMaster.beginTransmission(0x70);
  twMaster.write(0x00);
  twMaster.write(0x40); // First segment
  twMaster.write(0x00);
  twMaster.write(0x40); // Second segment
  twMaster.write(0x00);
  twMaster.write(0x00); // 0x02 activates :
  twMaster.write(0x00);
  twMaster.write(0x40); // Third segment
  twMaster.write(0x00);
  twMaster.write(0x40); // Fourth segment
  twMaster.write(0x00);
  twMaster.write(0x00);
  twMaster.write(0x00);
  twMaster.write(0x00);
  twMaster.write(0x00);
  twMaster.write(0x00);
  twMaster.write(0x00);
  twMaster.endTransmission();
}

void setup(){
  GnssConf.init();

  Serial1.config(STGNSS_UART_8BITS_WORD_LENGTH, STGNSS_UART_1STOP_BITS, STGNSS_UART_NOPARITY);
  Serial1.begin(115200);
  
  twMaster.config(100000); // 100 kHz
  twMaster.begin();

  // Init for Display
  twMaster.beginTransmission(0x70);
  twMaster.write(0x21); // Power-On
  twMaster.endTransmission();
  twMaster.beginTransmission(0x70);
  twMaster.write(0x81); // Display on, no blinking
  twMaster.endTransmission();
  twMaster.beginTransmission(0x70);
  twMaster.write(0xEF); // 100% duty
  twMaster.endTransmission();

  delay(500);
}

float speedKmh;
bool hasFix;

void loop() {
  if (hasFix) {
    writeSpeed(speedKmh);
  } else {
    clearScreen();
  }
  //digitalWrite(31, hasFix);
  //digitalWrite(28, hasFix);
}

void task_called_after_GNSS_update(void) {
  GnssInfo.update();
  if (GnssInfo.isUpdated()) {
    hasFix = GnssInfo.fixMode() >= 2;
    speedKmh = GnssInfo.speed.kph();
  }
}

