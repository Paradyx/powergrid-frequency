/**
 * This project is loosely based on the project orithena/Lamp_Simple1Button (https://github.com/orithena/Lamp_Simple1Button)
 */
#define DEBUG
#undef RENDERTIME // Define, if you want to measure the rendering time for the LED strip

#include "FastLED.h"
#include <Button.h>
#include "ESP8266WiFi.h"

// How many leds are there in your strip?
#define NUM_LEDS 15

// Where is the LED data line connected?
#define LED_DATA D4

//Number periods to time for frequency calculation.
#define DELTA_N 50

//What frequency should the powergrid have?
#define GREEN_FREQ 50.0

//The range to display on the LED-Strip
#define RANGE_FREQ 0.10

// variables for frequency measurment
// ESP8266 returns values between 1024 (=3.3V) and 0 (=0V) when measuring voltage.
#define THRESHOLD_HIGH 600
#define THRESHOLD_LOW 100 


bool state = 0; //high = 1, low = 0
unsigned long strt = 0;
unsigned int count = 0;
short mx = 0;
float freq = GREEN_FREQ;

//colors for gradients
CHSV low_hsv = CHSV(0, 255, 255); //red
CHSV mid_hsv = CHSV(90, 255, 255); //yellowish green

// the array of leds
CRGB leds[NUM_LEDS];

void resetColors() {
  fill_gradient(leds, 0, low_hsv, 6, mid_hsv, FORWARD_HUES);
  leds[7] = CRGB(0,255,0);
  fill_gradient(leds, 8, mid_hsv, 14, low_hsv, BACKWARD_HUES);
}


void setup() { 
  // make sure the Wifi chip is off, we don't need it.
  WiFi.forceSleepBegin();

  #ifdef DEBUG
  Serial.begin(230400);
  Serial.println("\n\n === powergridFrequency.ino ===\n\n");
  #endif

  // Initialize the LEDs
  pinMode(LED_DATA, OUTPUT); 
  FastLED.addLeds<WS2812B, LED_DATA, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(64);

  resetColors();
  FastLED.show();

  //Initialize Pins
  pinMode(A0, INPUT);

  //Start timer
  strt = micros();
}

static void countHighs() {
  short voltage = analogRead(A0);
  mx = max(voltage, mx);
  if (state && voltage < THRESHOLD_LOW) { //high state
    state = false;
    count++; //count falling edges
  } else if (!state && voltage > THRESHOLD_HIGH) { //low state
    state = true;
  }
}

static double scaleToLeds(
    float measured /*measured frequency*/, 
    float middle /*green center*/, 
    float range /*distance between red and green*/
  ) {
  double scaled = ((measured-middle) / range * 0.5) + 0.5; //[0, 1]
  double ranged = min(max(scaled, 0.0), 1.0);
  return ranged;
}

const float slotting = NUM_LEDS-1;
static void fillLeds(float highlight /*[0, 1]*/) {
  resetColors();
  
  for(int x=0; x < NUM_LEDS; x++) {
    float s = x/slotting - highlight;
    float factor = (255 * max(0.05, -100* (s-0.1) * (s+0.1)));
    leds[x] %= (int) factor;
  }
}

static void timeHighs() {
  short voltage = analogRead(A0);
  mx = max(voltage, mx);
  if (state && (voltage < THRESHOLD_LOW)) { //falling edge
    unsigned long now = micros();
    state = false;
    count++; //count falling edges
    
    if (count >= DELTA_N) {
      freq = ((float) DELTA_N * 1000000.) / (now - strt);

      #ifdef DEBUG
      Serial.print("Netzfrequenz: ");
      Serial.println(freq, 4);
   
      Serial.print("Maximum Voltage: ");
      Serial.println(mx * 3.3/1024);
      #endif

      //Reset counters and timer
      mx = 0;
      count = 0;
      strt = micros();
    }
  } else if (!state && (voltage > THRESHOLD_HIGH)) { //rising edge
    state = true;
  }
  
}

void loop() {  
  
  // frequency measurements
  timeHighs();
  
  //byte real_bright = 4;
  //FastLED.setBrightness(real_bright);
  EVERY_N_MILLISECONDS(50) {    
    
    #ifdef RENDERTIME
    long x = micros();
    #endif
    
    fillLeds(scaleToLeds(freq, GREEN_FREQ, RANGE_FREQ));
    // push the leds array out to the LED strip
    FastLED.show();

    #ifdef RENDERTIME
    long y = micros();
    Serial.print("Render time: ");
    Serial.println(y-x);
    #endif
  }

  // reset the watchdog (normally, the Wifi library does this, but we have disabled it).
  wdt_reset();
}
