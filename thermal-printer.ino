// Include the Arduino Stepper Library
#include <Stepper.h>
#include "fonts.h"

#define CLK 10
#define DATA  8
#define LATCH 7
#define STROBE1 12
#define STROBE2 11

#define FONT_WIDTH 24
#define FONT_HEIGHT 32

#define HEAD_LENGTH_BYTES 48
#define MAX_PRINT_CHAR HEAD_LENGTH_BYTES/(FONT_WIDTH/8)

const int dataPin = 8;
const int clkPin = 10;
const int latchPin = 7;

#define HEAT_TIME 2800//us
const int stbPins[2] = {11, 12};

const int stepsPerRevolution = 400;

Stepper myStepper(stepsPerRevolution, 3, 4, 5, 6);

uint8_t buffer [48];

void pushLine(uint8_t *buff, uint16_t len) { //send current line buffer to the printer head
  for (uint16_t i = 0; i < len; i++) {
    // shiftOut(dataPin,clkPin,MSBFIRST,pgm_read_byte(&buff[i])); //FOR bitmaps in PROGMEM
    shiftOut(dataPin, clkPin, MSBFIRST, buff[i]); //Normal mode, buffer in RAM
    delayMicroseconds(10);
    /*for(int a=0;a<8;a++){
         digitalWrite(dataPin,(buff[i]<<a)&128);
         digitalWrite(clkPin,HIGH);
         delayMicroseconds(1);
         digitalWrite(clkPin,LOW);
      }*/
  }
  digitalWrite(latchPin, LOW); //activate LATCH
  delayMicroseconds(1);
  digitalWrite(latchPin, HIGH);
  ;
}

void printLine(uint8_t *buff, uint16_t len, uint16_t printTime, bool push) {

  if (push) {
    pushLine(buff, len);
  }
  for (int i = 0; i < 2; i++) {
    digitalWrite(stbPins[i], HIGH);
    delayMicroseconds(printTime);
    digitalWrite(stbPins[i], LOW);
  }
  myStepper.step(-2);
  delay(1);
}

void nprintLine(uint8_t *buff, uint16_t len, uint16_t printTime, bool push, uint8_t lines) {
  for (int a = 0; a < lines; a++) {
    unsigned long time = micros();
    if (push)
      printLine((buff + a * len), len, printTime, true);
    else
      printLine((buff + a * len), len, printTime, false);
  }
}

void printText(uint8_t *str, const uint8_t font[]) {
  uint8_t temp[MAX_PRINT_CHAR + 1];
  //print line for (n*width)
  uint8_t print_lines = ((strlen(str) - 1) * FONT_WIDTH) / (HEAD_LENGTH_BYTES * 8);

  for (uint8_t line_index = 0; line_index <= print_lines; line_index++) {



    if (strlen(str) - line_index * MAX_PRINT_CHAR < MAX_PRINT_CHAR && line_index == print_lines) {
      strcpy(temp, str + (MAX_PRINT_CHAR)*line_index);
      for (uint8_t i = strlen(str) - line_index * MAX_PRINT_CHAR; i < MAX_PRINT_CHAR; i++) {

        temp[i] = ' ';

      }

    }
    else
    {
      strncpy(temp, str + (MAX_PRINT_CHAR)*line_index, MAX_PRINT_CHAR);
    }
    //print each line for buffer
    for (uint8_t y = 0; y < FONT_HEIGHT; y++) {
      //print each character
      for (uint8_t x = 0; x < HEAD_LENGTH_BYTES; x = x + FONT_WIDTH / 8) {
        //print the required number of bytes for each character
        for (uint8_t z = 0; z < FONT_WIDTH / 8; z++) {
         //subtract 32 as ASCII offset in array addressing
         //4 extra bytes in the font need to be avoided so addressing is incremented with 4
         //font array implies that each character uses a font width multiple of 8(8, 16, 24, 32, etc)
         //each line of the character has a total of FONT_WIDTH/8 bytes in length and has exactly FONT_HEIGHT lines which can be any number
          buffer[x + z] = pgm_read_byte(&font[(temp[x / (FONT_WIDTH / 8)] - 32) * FONT_HEIGHT * (FONT_WIDTH / 8) + ((FONT_WIDTH / 8) * y) + z + 4]);

        }
      }
      printLine(buffer, HEAD_LENGTH_BYTES, HEAT_TIME, true);
    }
  }

}

void setup()
{
  // set the speed at 60 rpm:
  myStepper.setSpeed(60);
  pinMode(dataPin, OUTPUT);
  pinMode(clkPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  for (int i = 0; i < 2; i++)
  {
    pinMode(stbPins[i], OUTPUT);
  }
  
  digitalWrite(latchPin, HIGH);

  delayMicroseconds(100);
/*
 //Test code to empty the shift register
 //900 chosen as arbitrary value if actual number of pixels is not known
  for (uint16_t i = 0; i < 900; i++)
  {
    digitalWrite(CLK, LOW);
    delayMicroseconds(10);
    digitalWrite(CLK, HIGH);
    delayMicroseconds(10);
  }
  */
  Serial.begin(9600);
 
 

}
void loop()
{ 
  static String readString;
  static uint8_t serial_string[500];
  
  if (Serial.available()) {


    readString = Serial.readStringUntil('\r'); 
    strcpy(serial_string, (char*)readString.c_str());
    printText(serial_string, arial_normal);
  }
}
