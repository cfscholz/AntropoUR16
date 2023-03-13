
#include <Adafruit_DotStar.h>
#include <SPI.h>

#define NUM_LEDS 40 //strip length
#define DATAPIN    4 //green
#define CLOCKPIN   5 //yellow

int ergoFeedback = 0;
float pressureFeedback = 1;

Adafruit_DotStar strip(NUM_LEDS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

 uint32_t white = strip.Color(255, 255, 255);
 uint32_t blue = strip.Color(140, 0, 233);
 uint32_t green = strip.Color(255, 0, 0);
 uint32_t red = strip.Color(0, 255, 0);
 uint32_t none = strip.Color(0, 0, 0);
 uint32_t orange = strip.Color(255, 128, 0);
 uint32_t purple = strip.Color(0, 148, 211);


int state = 0; // initial state is 0 (default state)
char input;

unsigned long prevTimeLeft = 0;
unsigned long prevTimeRight = 0;


void setup() {

Serial.begin(9600);

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP

}

void loop() {
  if (Serial.available() > 0) { // check if there is serial data available
    input = Serial.read(); // read the input character from serial port
    switch (input) { // check the input character
      case '1': // if input is '1', change the state to 1
        state = 1;
        Serial.println("State 1");
        break;
      case '2': // if input is '2', change the state to 2
        state = 2;
        Serial.println("State 2");
        break;
      case '3': // if input is '3', change the state to 3
        state = 3;
        Serial.println("State 3");
        break;
      case '4': // if input is '4', change the state to 4
        state = 4;
        Serial.println("State 4");
        break;
      case '5': // if input is '5', change the state to 5
        state = 5;
        Serial.println("State 5");
        break;
      case '0': // if input is '0', change the state to 0 (default state)
        state = 0;
        Serial.println("Default State");
        break;
      default: // if input is not valid, do nothing
        break;
    }
  }

  // check the current state and perform the corresponding actions
  switch (state) {
    case 0: // default state, do nothing
      strip.fill(none, 0, 40);
      strip.setBrightness(0);
      strip.show();
      break;
    case 1: // state 1 actions
      kill();
      breathe();
      break;
    case 2: // state 2 actions
      kill();
      slowBlinkGreen();
      break;
    case 3: // state 3 actions
      feedback(ergoFeedback, pressureFeedback);
      break;
    case 4: // state 4 actions
      kill();
      turningBlueLeft();
      break;
    case 5: // state 5 actions
      kill();
      turningBlueRight();
      break;
    default: // invalid state, reset to default state
      state = 0;
      kill();
      break;
  }

}

void breathe() {
  int oldState = state; // save the current state
  unsigned long lastTime = 0;
  int interval = 100; // time interval between brightness changes in milliseconds
  
  strip.fill(white, 0, 40);
  strip.setBrightness(0);
  strip.show();
  
  for (int i = 0; i < 255; i = i + 1) {
    strip.setBrightness(i);
    strip.show();
    
    // check if the time interval has passed and state has not changed
    if (millis() - lastTime >= interval && state == oldState) {
      lastTime = millis();
    } else if (state != oldState || Serial.available()) { // check if the state has changed or new serial is available
      return; // exit the function early if state has changed or new serial is available
    }
  }
  
  for (int j = 0; j < 255; j = j + 3) {
    strip.setBrightness(255 - j);
    strip.show();
    
    // check if the time interval has passed and state has not changed
    if (millis() - lastTime >= interval && state == oldState) {
      lastTime = millis();
    } else if (state != oldState || Serial.available()) { // check if the state has changed or new serial is available
      return; // exit the function early if state has changed or new serial is available
    }
  }
  
  // check if new serial is available and doesn't match the current state
  if (Serial.available() && Serial.parseInt() != state) {
    state = 0; // switch to the main loop
  }
}

void slowBlinkGreen() {

   for (int i = 0; i < 1; i++) {
     strip.setBrightness(255);
     strip.fill(green, 0, 40);
     strip.show();
     delay(1000);
     strip.setBrightness(0);
     strip.show();
     delay(400);
     strip.setBrightness(255);
   }  
}

void turningBlueLeft() {
  strip.setBrightness(255);
  strip.show();
  int delayTime = 500; // adjust this value to change the speed of the rotation
  unsigned long currentTime = millis();
  if (currentTime - prevTimeLeft >= delayTime) {
    prevTimeLeft = currentTime;
    for (int i = NUM_LEDS / 2 - 4; i >= 0; i--) {
      strip.fill(none, 0, NUM_LEDS); // turn off all LEDs
      // set the LED colors on the bottom and top strips
      strip.setPixelColor(i, blue);
      strip.setPixelColor(i + 1, blue);
      strip.setPixelColor(i + 2, blue);
      strip.setPixelColor(NUM_LEDS - i - 1, blue);
      strip.setPixelColor(NUM_LEDS - i - 2, blue);
      strip.setPixelColor(NUM_LEDS - i - 3, blue);
      strip.show(); // update the LED strip
    }
  }
}

void turningBlueRight() {
  strip.setBrightness(255);
  strip.show();
  int delayTime = 500; // adjust this value to change the speed of the rotation
  unsigned long currentTime = millis();
  if (currentTime - prevTimeRight >= delayTime) {
    prevTimeRight = currentTime;
    for (int i = 0; i < NUM_LEDS / 2 - 4; i++) {
      strip.fill(none, 0, NUM_LEDS); // turn off all LEDs
      // set the LED colors on the bottom and top strips
      strip.setPixelColor(i, blue);
      strip.setPixelColor(i + 1, blue);
      strip.setPixelColor(i + 2, blue);
      strip.setPixelColor(NUM_LEDS - i - 1, blue);
      strip.setPixelColor(NUM_LEDS - i - 2, blue);
      strip.setPixelColor(NUM_LEDS - i - 3, blue);
      strip.show(); // update the LED strip
    }
  }
}

void feedback(int ergoFeedback, float pressureFeedback) {
  
  if (ergoFeedback == 1) { // flashing red turning on first and last 20 LEDs
    int delayTime = 200; // adjust this value to change the speed of the flashing
    for (int i = 0; i < 2; i++) { // repeat the pattern twice
      for (int j = 0; j < 20; j++) {
        strip.setPixelColor(j, red); // turn on first 20 LEDs
        strip.setPixelColor(NUM_LEDS - j - 1, red); // turn on last 20 LEDs
      }
      strip.show(); // update the LED strip
      delay(delayTime);
      strip.fill(none, 0, NUM_LEDS); // turn off all LEDs
      strip.setBrightness(250);
      strip.show(); // update the LED strip
      delay(delayTime);
    }
  } else if (ergoFeedback == 0) { // dimming purple based on pressureFeedback
    int brightness = (int)(pressureFeedback * 255.0); // calculate brightness value using float to int conversion
    brightness = constrain(brightness, 0, 255); // limit brightness value to [0, 255]
    strip.fill(purple, 0, NUM_LEDS); // set the purple color with mapped brightness value
    strip.setBrightness(brightness);
    strip.show(); // update the LED strip
  }
}

void kill(){
  strip.fill(none, 0, 40);
  strip.setBrightness(0);
  strip.show();
}
