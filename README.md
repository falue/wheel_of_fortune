# WHEEL OF FORTUNE
## Whats this
If you have scanned the QR Code on the back of the wheel and do not know what this is:

This site contains the code for making the LEDs blink.

If you need my contact for changing or setting up this wheel, feel free to contact me here:

info@fluescher.ch / <a href="tel:0787424834">0787424834</a>


## Instructions for set up
[Here](https://raw.githubusercontent.com/falue/wheel_of_fortune/main/MANUAL.pdf) you'll find the instructions for setup / deconstruct this wheel.
A **very** important read to not break anything.

# Customizing
Change these values to your liking to customize the experience. Upload it to the Arduino on the back. Have fun.

```
// Basic colors. Note as CRGB::Colorname or CRGB(255,255,255)
CRGB color = CRGB::White;

// min speed for symmetric lights (another LED is lit up, symmetrically to first one)
int minSpeedForSymmetry = 33;

// if not symmetric light effect, reverse=true makes light go counter clockwise
bool reverse = false;

// how fast light effect dies down. bigger number, faster stopping effect
int decayPerLoop = 2;

// how fast the light effect speeds up. bigger number, faster turning
int bonusForRotation = 20;

// max speed of light effect
int speedMax = 45;

// Threshold for detecting a turning wheel. A little dependent on room light, so change it if it behaves erratically.
int photoThreshold = 245;
```

Check out [all the official color names here](https://github.com/FastLED/FastLED/wiki/Pixel-reference#predefined-colors-list).

With little effort you can make the rolling lights go faster or slower, change the symmetry mode, change the main color. Not much else. If you need custom effects, I'd recommend checking out the [FastLED library examples](https://github.com/FastLED/FastLED/tree/master/examples) and program your own.