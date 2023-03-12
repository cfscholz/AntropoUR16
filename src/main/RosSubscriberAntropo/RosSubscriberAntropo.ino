#include <Adafruit_DotStar.h>
#include <SPI.h>
#include <ros.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Float32.h>

#define NUM_LEDS 40 // total number of LEDs in the strip
#define DATAPIN    4 // green wire connected to this pin
#define CLOCKPIN   5 // yellow wire connected to this pin

// Global variables for feedback values
int ergoFeedback = 0; // Default value for ergo feedback
float pressureFeedback = 1; // Default value for pressure feedback

// Create an object for controlling the LED strip
Adafruit_DotStar strip(NUM_LEDS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

// Define some color values used in the code
uint32_t white = strip.Color(255, 255, 255);
uint32_t blue = strip.Color(140, 0, 233);
uint32_t green = strip.Color(255, 0, 0);
uint32_t red = strip.Color(0, 255, 0);
uint32_t none = strip.Color(0, 0, 0);
uint32_t orange = strip.Color(255, 128, 0);
uint32_t purple = strip.Color(0, 148, 211);

// Global variable for state, initially set to 0
int state = 0;

// Initialize the ROS node handle object
ros::NodeHandle nh;

// Callback function for STATE subscriber
void stateCallback(const std_msgs::Int32& msg){
  state = msg.data;
}

// Callback function for ergofeedback subscriber
void ergoFeedbackCallback(const std_msgs::Int32& msg){
  ergoFeedback = msg.data;
}

// Callback function for pressurefeedback subscriber
void pressureFeedbackCallback(const std_msgs::Float32& msg){
  pressureFeedback = msg.data;
}

// Create ROS subscribers for STATE, ergofeedback, and pressurefeedback messages
ros::Subscriber<std_msgs::Int32> stateSub("state", &stateCallback);
ros::Subscriber<std_msgs::Int32> ergoFeedbackSub("ergo_feedback", &ergoFeedbackCallback);
ros::Subscriber<std_msgs::Float32> pressureFeedbackSub("pressure_feedback", &pressureFeedbackCallback);

// Global variables for timing control
unsigned long prevTimeLeft = 0; // Used in turningBlueLeft() function
unsigned long prevTimeRight = 0; // Used in turningBlueRight() function

void setup() {
  Serial.begin(9600); // Initialize the serial communication
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
    clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
  #endif

  strip.begin(); // Initialize the LED strip
  strip.show();  // Turn off all LEDs
  
  nh.initNode(); // Initialize the ROS node
  nh.subscribe(stateSub); // Subscribe to state messages
  nh.subscribe(ergoFeedbackSub); // Subscribe to ergo feedback messages
  nh.subscribe(pressureFeedbackSub); // Subscribe to pressure feedback messages
}


void loop() {
  nh.spinOnce(); // process ROS callbacks

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
  int oldState = state; // Save the current state
  unsigned long lastTime = 0; // Used for timing control
  int interval = 100; // Time interval between brightness changes in milliseconds

  strip.fill(white, 0, 40); // Set the color of all LEDs to white
  strip.setBrightness(0); // Set the brightness to 0
  strip.show(); // Update the LED strip

  // Increase brightness gradually
  for (int i = 0; i < 255; i = i + 1) {
    strip.setBrightness(i);
    strip.show();

    // Check if the time interval has passed and state has not changed
    if (millis() - lastTime >= interval && state == oldState) {
      lastTime = millis();
    } else if (state != oldState || Serial.available()) { // Check if the state has changed or new serial is available
      return; // Exit the function early if state has changed or new serial is available
    }
  }

  // Decrease brightness gradually
  for (int j = 0; j < 255; j = j + 3) {
    strip.setBrightness(255 - j);
    strip.show();

    // Check if the time interval has passed and state has not changed
    if (millis() - lastTime >= interval && state == oldState) {
      lastTime = millis();
    } else if (state != oldState || Serial.available()) { // Check if the state has changed or new serial is available
      return; // Exit the function early if state has changed or new serial is available
    }
  }

  // Check if new serial is available and doesn't match the current state
  if (Serial.available() && Serial.parseInt() != state) {
    state = 0; // Switch to the main loop
  }
}

void slowBlinkGreen() {
  // Blink the LEDs green
  for (int i = 0; i < 1; i++) { // Repeat the pattern once
    strip.setBrightness(255); // Set the brightness to maximum
    strip.fill(green, 0, 40); // Set the color of all LEDs to green
    strip.show(); // Update the LED strip
    delay(1000); // Wait for 1 second
    strip.setBrightness(0); // Set the brightness to 0
    strip.show(); // Update the LED strip
    delay(400); // Wait for 0.4 seconds
    strip.setBrightness(255); // Set the brightness to maximum
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
