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
  // char* visual = '0000000000000000';
  // visual[index] = '1';

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
    // visual[overflow(NUM_LEDS-index-2, 0, NUM_LEDS-1)] = '1';
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

  if(readingPhotoDiode < min) min = readingPhotoDiode;
  if(readingPhotoDiode > max) max = readingPhotoDiode;


  
  FastLED.show();

  // hrmmmgh
  // maybe do something not blocking?
  // what happens if speed==0?
  int wait = 1000/speed;
  if(wait < 0) wait = 0;
  Serial.print("\twait:\t");
  Serial.print(wait);

  Serial.println("");

  ///// TO DO: IF SPEED IS HIGH, MAKE wait SHORT
  ///// SPLIT DELAY IN 20MS CHUNKS AND CHECK FOR ROTATION.
  bool hasNotTurned = true;
  while(wait > 0 && hasNotTurned) {
    delay(20);
    wait -= 20;
    // if new reading of photoresistor is different than what was before, exit while loop
    // because wheel has turned while waiting
    int difference = readingPhotoDiode - analogRead(photoResistorPin);
    hasNotTurned =  difference < 10 && difference > -10;
    delay(wait);
  }
  
  delay(20);
}

int clamp(int value, int min, int max) {
  return value < min ? min : value > max ? max : value;
}

int overflow(int value, int min, int max) {
  return value < min ? max : value > max ? min : value;
}
