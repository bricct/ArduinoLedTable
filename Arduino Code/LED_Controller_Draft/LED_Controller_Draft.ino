#include <FastLED.h>

// ****************** //
// **** CONSTANTS *** //
// ****************** //

#define LED_PIN_1 10
#define LED_PIN_2 12

#define BUTTON_PIN_1 A0
#define BUTTON_PIN_2 A2

#define NUM_LEDS 60
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define LED_ON_BRIGHTNESS 60
#define LED_OFF_BRIGHTNESS 0

#define BUTTON_NONE 0
#define BUTTON_POWER 1
#define BUTTON_NEXT 2
#define BUTTON_PREV 3
#define BUTTON_ANIMATE 4

#define BUFFER_SIZE 75


// ****************** //
// ***** STRUCTS **** //
// ****************** //

struct sensor_buffer {
  int buffer[BUFFER_SIZE];
  int index;
  int size;
};


struct led_strip {
  uint8_t buttonPin;
  uint8_t outputPin;
  bool on;
  bool rainbow;
  uint8_t animation;
  int color;
  uint8_t brightness;
  uint8_t lastButton;
  struct sensor_buffer sensor;
  struct CRGB *leds;
};

typedef led_strip LED_STRIP;




// ****************** //
// ***** GLOBALS **** //
// ****************** //

LED_STRIP strip_1;
LED_STRIP strip_2;

CRGB leds_1[NUM_LEDS];
CRGB leds_2[NUM_LEDS];

// ****************** //
// ****** SETUP ***** //
// ****************** //

void setup() {

  setupLedStrips();

  delay(1000);
  Serial.begin(9600);

  pinMode(BUTTON_PIN_1, INPUT);
  pinMode(BUTTON_PIN_2, INPUT);
  
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  
  fill_solid(strip_1.leds, NUM_LEDS, CRGB::Black);
  fill_solid(strip_2.leds, NUM_LEDS, CRGB::Black);

  FastLED.show();

  setupSensorBuffers();
}

void setupLedStrips() {

  strip_1.buttonPin = BUTTON_PIN_1;
  strip_1.outputPin = LED_PIN_1;
  strip_1.on = false;
  strip_1.animation = 0;
  strip_1.color = 0;
  strip_1.lastButton = BUTTON_NONE;
  strip_1.sensor.index = 0;
  strip_1.sensor.size = BUFFER_SIZE;
  strip_1.leds = leds_1;
  strip_1.rainbow = false;

  strip_2.buttonPin = BUTTON_PIN_2;
  strip_2.outputPin = LED_PIN_2;
  strip_2.on = false;
  strip_2.animation = 0;
  strip_2.color = 0;
  strip_2.lastButton = BUTTON_NONE;
  strip_2.sensor.index = 0;
  strip_2.sensor.size = BUFFER_SIZE;
  strip_2.leds = leds_2;
  strip_2.rainbow = false;

  FastLED.setBrightness(LED_ON_BRIGHTNESS);
  LEDS.addLeds<LED_TYPE, LED_PIN_1, COLOR_ORDER>(leds_1, NUM_LEDS);
  LEDS.addLeds<LED_TYPE, LED_PIN_2, COLOR_ORDER>(leds_2, NUM_LEDS); 
}

void setupSensorBuffers() {
  for (int i = 0; i < strip_1.sensor.size; i++) {
    strip_1.sensor.buffer[i] = 0;
  }
  for (int i = 0; i < strip_2.sensor.size; i++) {
    strip_2.sensor.buffer[i] = 0;
  }
}



// ****************** //
// ****** LOOP ****** //
// ****************** //

void loop() {

  update(&strip_1);
  update(&strip_2);
  
  FastLED.show();
  delay(1);
}

void update(led_strip *strip) {
  int button = readButtonPress(strip);

  handleButtonPress(strip, button);

  animate(strip);
}


int readButtonPress(led_strip *strip) {
  strip->sensor.index = (strip->sensor.index + 1) % strip->sensor.size;
  strip->sensor.buffer[strip->sensor.index] = analogRead(strip->buttonPin);

  int button = calcPressedButton(&(strip->sensor));
  
  if (button == strip->lastButton) return BUTTON_NONE;
  strip->lastButton = button;
  return button;
}

int calcPressedButton(sensor_buffer *sensor) {
  int button = -1;
  for (int i = 0; i < sensor->size; i++) {
    if (button == -1) {
      button = translateSignalToButton(sensor->buffer[i]);
    }

    if (button != translateSignalToButton(sensor->buffer[i])) return BUTTON_NONE;

    return button;
  }
}

int translateSignalToButton(int sensorValue) {
  if (sensorValue > 900) {
    return BUTTON_POWER;
  } else if (sensorValue < 900 && sensorValue >= 675) {
    return BUTTON_NEXT;
  } else if (sensorValue < 675 && sensorValue >= 400) {
    return BUTTON_PREV;
  } else if (sensorValue < 400  && sensorValue >= 200 ) {
    return BUTTON_ANIMATE;
  } else if (sensorValue < 200 ) {
    return BUTTON_NONE;
  }  
}


void handleButtonPress(led_strip* strip, uint8_t button) {
  switch(button) {
    case BUTTON_NONE:
      return;

    case BUTTON_POWER:
      strip->on = !(strip->on);
      if (strip->on) { 
        strip->brightness = LED_ON_BRIGHTNESS;
      }
      else {
        strip->brightness = LED_OFF_BRIGHTNESS;
      }
      return;

    case BUTTON_NEXT:
      if (strip->color == 7 && !(strip->rainbow)) {
        strip->rainbow = true;
      }
      else if (strip->rainbow) {
        strip->rainbow = false;
      }
      else {
        strip->color = (strip->color + 1) % 8;
      }
      return;

    case BUTTON_PREV:
      if (strip->color == 0 && !(strip->rainbow)) {
        strip->rainbow = true;
      }
      else if (strip->rainbow) {
        strip->rainbow = false;
      }
      else {
        strip->color = (strip->color - 1) % 8;
      }
      return;

    case BUTTON_ANIMATE:
       strip->animation = (strip->animation) + 1 % 5;
       if (strip->rainbow && strip->animation == 4) strip->animation = 0;
      return;

    default:
      return;
  }
}


void animate(led_strip *strip) {
//  Serial.print("aState: ");
//  Serial.print(aState);
//  Serial.print("\n");
  if (strip->rainbow) {
    switch(strip->animation) {
      case 0:
        rainbow_wave(strip);
        return;
    
      case 1:
        rainbow_bounce(strip);
        return;
      
      case 2:
        rainbow_trace(strip);
        return;
      
      case 3:
        rainbow_build(strip);
        return;

      case 200:
        destroy(strip);
        return;

      default:
        return;  
    }
  }

  else {
    switch(strip->animation) {
      case 0:
        solid(strip);
        return;
      
      case 1:
        every_other(strip);
        return;
      
      case 2:
        wave(strip);
        return;
      
      case 3:
        trace(strip);
        return;
      
      case 4:
        build(strip);
        return;   
      
      case 200:
        destroy(strip);
        return;

      default:
        return;   
    }
  }
}



// ****************** //
// *** ANIMATIONS *** //
// ****************** //

int getColor(int color) {
  return color << 5;
}


void solid(led_strip *strip) {
  //Serial.print("solid\n");
  fill_solid(strip->leds, NUM_LEDS, CHSV(getColor(strip->color), 255, strip->brightness));
}

void wave(led_strip *strip) {
  fadeToBlackBy(strip->leds, NUM_LEDS, 32);
  int pos = beatsin8(12, 0, NUM_LEDS-1);

  strip->leds[pos] += CHSV(getColor(strip->color), 255, strip->brightness);
}


void trace(led_strip *strip) {
  fadeToBlackBy(strip->leds, NUM_LEDS, 32);
  int pos = beatsin8(16, 0, NUM_LEDS/2 - 1);
  strip->leds[pos] = CHSV(getColor(strip->color), 255, strip->brightness);
  strip->leds[NUM_LEDS-1-pos] = CHSV(getColor(strip->color), 255, strip->brightness);
}

void build(led_strip *strip) {
  int pos = beatsin8(16, 0, NUM_LEDS/2);
  strip->leds[pos] = CHSV(getColor(strip->color), 255, strip->brightness);
  strip->leds[NUM_LEDS-1-pos] = CHSV(getColor(strip->color), 255, strip->brightness);
  if (pos == 30) {
    delay(500);
    strip->animation = 200;
  }
}

void destroy(led_strip *strip) {
  int pos = beatsin8(16, 0, NUM_LEDS/2);
  strip->leds[pos] = CHSV(getColor(strip->color), 255, LED_OFF_BRIGHTNESS);
  strip->leds[NUM_LEDS-1-pos] = CHSV(getColor(strip->color), 255, LED_OFF_BRIGHTNESS);
  if (pos == 0) {
    if (strip->rainbow) {
      strip->animation = 3;
    } else {
      strip->animation = 4;
    }
  }
}


void every_other(led_strip *strip) {
  uint8_t  brt = beat8(1,255);
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i % 2 == brt % 2) {
      strip->leds[i] = CHSV(getColor(strip->color), 255, LED_OFF_BRIGHTNESS);
    } else {
      strip->leds[i] = CHSV(getColor(strip->color), 255, strip->brightness);
    }
  }
}


void rainbow_bounce(led_strip *strip) {
  uint8_t thisHue = beatsin8(12,0,255);
  fill_rainbow(strip->leds, NUM_LEDS, thisHue, 10);
  for (int i = 0; i < NUM_LEDS; i++) {
    strip->leds[i].fadeToBlackBy(205);
  }
}


void rainbow_wave(led_strip *strip) {
  uint8_t thisHue = beat8(64, 255);
  fill_rainbow(strip->leds, NUM_LEDS, thisHue, 10);
  for (int i = 0; i < NUM_LEDS; i++) {
    strip->leds[i].fadeToBlackBy(205);
  }
}

void rainbow_trace(led_strip *strip) {
  fadeToBlackBy(strip->leds, NUM_LEDS, 32); 
  uint8_t thisHue = beat8(64, 255);
  int pos = beatsin8(16, 0, NUM_LEDS/2 - 1);
  strip->leds[pos] = CHSV(thisHue, 255, strip->brightness);
  strip->leds[NUM_LEDS-1-pos] = CHSV(thisHue, 255, strip->brightness);
}


void rainbow_build(led_strip *strip) {
  uint8_t thisHue = beat8(64, 255);
  int pos = beatsin8(16, 0, NUM_LEDS/2);
  strip->leds[pos] = CHSV(thisHue, 255, strip->brightness);
  strip->leds[NUM_LEDS-1-pos] = CHSV(thisHue, 255, strip->brightness);
  if (pos == 30) {
    delay(500);
    strip->animation = 200;
  }
}
