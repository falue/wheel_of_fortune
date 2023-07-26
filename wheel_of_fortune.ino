#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 16

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define DATA_PIN 5
#define CLOCK_PIN 4

// Define the array of leds
CRGB leds[NUM_LEDS];

int ledPin = 6;
int photoResistorPin = A0;
int brightnessPin = A1;

int brightness = 255;

int speed = 0;
int index = 0;

int minSpeedForSymmetry = 8;
int minSpeed = 1;
int decayPerLoop = 1;
int bonusForRotation = 19;
int speedMax = 400;
bool triggeredPhoto = false;
int photoThreshold = 245;

int min = 1024;
int max = 0;
int maxSpeed = 0;

// CRGB color = CRGB::White;
////// TO DO: check different whites; test if ::Red Works because of color ordering?
CRGB color = CRGB::White;

void setup() { 
    Serial.begin(9600); // Initialize serial communication for printing potentiometer values
    pinMode(ledPin, OUTPUT);
    pinMode(photoResistorPin, INPUT);
    pinMode(brightnessPin, INPUT);

    FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    FastLED.setBrightness(brightness);

    // Black out all leds
    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));
    FastLED.show();
    Serial.println("START");
    blink(0, 4, 1200);

    digitalWrite(ledPin, HIGH);
  }

void loop() { 
  brightness = map(analogRead(brightnessPin), 0,1024, 0, 255);
  FastLED.setBrightness(brightness);

  // Black out all leds
  fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));

  // Light up one LED at a time
  index = index+1 > NUM_LEDS-1 ? 0 : index+1;
  leds[index] = color;

  int readingPhotoDiode = analogRead(photoResistorPin);

  // If photoResistorPin detects white AND was not triggered already
  if(!triggeredPhoto && readingPhotoDiode > photoThreshold) {
    // Get faster
    speed += bonusForRotation;
    triggeredPhoto = true;
    // Serial.print("\twhite trigger");

  } else if(triggeredPhoto && readingPhotoDiode < photoThreshold) {
    // if reading "black", be open for next trigger
    triggeredPhoto = false;
    // Serial.print("\tblack trigger");
  }
  
  // get slower whith each loop anyways
  speed -= decayPerLoop;
  speed = clamp(speed, 0, speedMax);


  // Turn on symmetry effect when fast turning
  if(speed > minSpeedForSymmetry) {
    Serial.print("\tSymmetry!");
    leds[overflow(NUM_LEDS-index-1, 0, NUM_LEDS-1)] = color;
  }

  if(speed <= minSpeed) {
    Serial.print("\tStand still");
    // wheel is standing still, show only top most
    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));
    leds[0] = color;
  } else {
    Serial.print("\tMovement!");
  }

  Serial.print("\tindex:\t");
  Serial.print(index);
  Serial.print("\tspeed:\t");
  Serial.print(speed);
  Serial.print("\tbrightness:\t");
  Serial.print(brightness);
  Serial.print("\tphotoResistorPin:\t");
  Serial.print(readingPhotoDiode);
  Serial.print(" = ");
  Serial.print(readingPhotoDiode > photoThreshold);
  Serial.print("\tmin/max:\t");
  Serial.print(min);
  Serial.print("/");
  Serial.print(max);
  Serial.print("\tmaxSpeed:\t");
  Serial.print(maxSpeed);

  if(readingPhotoDiode < min) min = readingPhotoDiode;
  if(readingPhotoDiode > max) max = readingPhotoDiode;
  if(speed > maxSpeed) maxSpeed = speed;

  FastLED.show();

  // maybe do something not blocking?
  int wait = 1000/speed;
  if(wait < 0) wait = 0;
  Serial.print("\twait:\t");
  Serial.print(wait);

  Serial.println("");
  delay(wait+20);
}

int clamp(int value, int min, int max) {
  return value < min ? min : value > max ? max : value;
}

int overflow(int value, int min, int max) {
  return value < min ? max : value > max ? min : value;
}

void blink(int ledNr, int times, int duration) {
  if(times > 0 ) {
    for (int i = 0; i <= times; i++) {
      //digitalWrite(pin, HIGH);
      leds[ledNr] = color;
      FastLED.show();
      delay(duration/(times*2));
      //digitalWrite(pin, LOW);
      leds[ledNr] = CRGB::Black;
      FastLED.show();
      delay(duration/(times*2));
    }
  }
}
