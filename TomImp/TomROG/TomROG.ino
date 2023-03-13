#include <Adafruit_DotStar.h>
#include <SPI.h>

#define NUM_LEDS 40 //strip length
#define DATAPIN    4 //green
#define CLOCKPIN   5 //yellow

Adafruit_DotStar strip(NUM_LEDS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

uint32_t white = strip.Color(255, 255, 255);
uint32_t blue = strip.Color(140, 0, 233);
uint32_t green = strip.Color(255, 0, 0);
uint32_t red = strip.Color(0, 255, 0);
uint32_t none = strip.Color(0, 0, 0);
uint32_t orange = strip.Color(69, 225, 0);
uint32_t purple = strip.Color(0, 148, 211);

int state = 0;
int input;

void setup() {

  Serial.begin(9600);

  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
    clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
  #endif

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP

  // Connect binary input pins
  pinMode(9, INPUT_PULLUP); // Connect binary input MSB (most significant bit) to pin 9 with internal pull-up resistor enabled
  pinMode(8, INPUT_PULLUP); // Connect binary input LSB (least significant bit) to pin 8 with internal pull-up resistor enabled

}

void loop() {

  // read binary input from pins 9 and 8
  input = digitalRead(9) * 2 + digitalRead(8);

  switch (input) { // check the binary input
    case 0b01: // if input is '01', change the state to 1
      state = 1;
      Serial.println("State 1");
      break;
    case 0b10: // if input is '10', change the state to 2
      state = 2;
      Serial.println("State 2");
      break;
    case 0b11: // if input is '11', change the state to 3
      state = 3;
      Serial.println("State 3");
      break;
    case 0b00: // if input is '00', change the state to 0 (default state)
      state = 0;
      Serial.println("Default State");
      break;
    default: // if input is not valid, do nothing
      break;
  }

  // check the current state and perform the corresponding actions
  switch (state) {
    case 0: // default state, do nothing
      strip.fill(none, 0, 40);
      strip.setBrightness(0);
      strip.show();
      break;
    case 1: // green
      strip.fill(green, 0, 40);
      strip.setBrightness(255);
      strip.show();
      break;
    case 2: // orange
      strip.fill(orange, 0, 40);
      strip.setBrightness(255);
      strip.show();
      break;
    case 3: // red
      strip.fill(red, 0, 40);
      strip.setBrightness(255);
      strip.show();
      break;
    default: // invalid state, reset to default state
      state = 0;
      break;
  }

}
