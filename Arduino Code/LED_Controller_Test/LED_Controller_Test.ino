#include <FastLED.h>

#define NUM_LEDS 60
#define LED_PIN 10
#define COLOR_ORDER GRB
#define LED_TYPE WS2812B

#define B_PIN A0

int sensorValue = 0;

uint8_t brightness = 100;
uint8_t aState = 0;
uint8_t mState = 0;
uint8_t cState = 0;
uint8_t bState = 0;
uint8_t color = 0;
bool on = false;
uint8_t on_bright = 100;
uint8_t anim_pos = 0;



CRGB leds[NUM_LEDS];

void setup() {
  // put your setup code here, to run once:
  FastLED.setBrightness(brightness);
  LEDS.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS); 
  delay(1000);
  Serial.begin(9600);
  pinMode(B_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void loop() {
  // put your main code here, to run repeatedly:
  bState = readSensor(B_PIN);
  debounceState(B_PIN);
  //getColor();
//  if (bState != 0) { 
//    Serial.print("bState: ");
//    Serial.print(bState);
//    Serial.print("\n");
//  }
  if (bState != 0)
    stateChange();
  animate();
  
  FastLED.show();

}





//reads the analog pin and returns which button is pressed
int readSensor(int buttonPin) {
  sensorValue = analogRead(buttonPin);
  //Serial.print(sensorValue);
  //Serial.print("\n");
  if (sensorValue > 900) {
    return 1;
  } else if (sensorValue > 750 && sensorValue < 1000) {
    return 2;
  } else if (sensorValue < 750 && sensorValue > 500) {
    return 3;
  } else if (sensorValue < 500  && sensorValue > 250 ) {
    return 4;
  } else if (sensorValue < 250 ) {
    return 0;
  }
  
}


//debounces button press to verify signal
void debounceState(int buttonPin) {
  int currentState = readSensor(buttonPin);

   if (bState != currentState) {
      delay(50);
      bState = readSensor(buttonPin);
   }

  if (bState != 0) {
   while(bState == readSensor(buttonPin)) {
    delay(1);
    //Serial.print("delaying...\n");
   }
  }

}


//sets the color palette based on color state
void getColor() {
  //Serial.print("Colors Changed\n");
  if (cState < 8) {color = cState * 32;}
  else {color = 0;}
  
}


//performs an operation based on which button is pressed
void stateChange() {
  switch(bState) {
    case 1:
      on = !on;
      if (on) { 
        cState = 0;
        getColor();
        brightness = on_bright;
      }
      else {
        brightness = 0;
        aState = 0;
      }
      return;
    case 2:
      if (cState == 8) {
          aState = 0;
        }
      cState++;
      if (cState > 8) cState = 0;
//      Serial.print("cState: ");
//      Serial.print(cState);
//      Serial.print("\n");
      else if (cState == 8) {
        aState = 66;
        return;
      }
      getColor();
      return;
    case 3:
      if (cState == 8) {
        aState = 0;
      }
      cState--;
      if (cState > 7) cState = 7;
//      Serial.print("cState: ");
//      Serial.print(cState);
//      Serial.print("\n");
      getColor();
      return;
    case 4:
       if (cState == 8) {
        aState++;
        if (aState > 69) aState = 66;
        return;
       }
       aState++;
//       Serial.print("Incrementing aState\n");
       if (aState > 4){ aState = 0;}
//       Serial.print("aState: ");
//       Serial.print(aState);
//       Serial.print("\n");
      return;
    default:
      return;
  }
}


void animate() {
//  Serial.print("aState: ");
//  Serial.print(aState);
//  Serial.print("\n");
  switch(aState) {
    case 0:
      solid();
      return;
     case 1:
      every_other();
      return;
     case 2:
      wave();
      return;
     case 3:
      trace();
      return;
     case 4:
       build();
       return;
     case 66:
        rainbow_wave();
        return;
     case 67:
        rainbow_bounce();
        return;
     case 68:
       rainbow_trace();
       return;
     case 69:
      rainbow_build();
      return;
     case 200:
      destroy();
      return;
     default:
      return;   
  }
}



void solid() {
  //Serial.print("solid\n");
  fill_solid(leds, NUM_LEDS, CHSV(color, 255, brightness));
}

void wave() {
  fadeToBlackBy(leds, NUM_LEDS, 32);
  int pos = beatsin8(12, 0, NUM_LEDS-1);
  //pos = map(pos, 0, 255, 0, NUM_LEDS-1);
  //leds[pos] = CRGB::Black;
  leds[pos] += CHSV(color, 255, brightness);
//  if (pos % 30 == 0) {
//    solid();
//  }

  
//  fill_solid(leds, NUM_LEDS, CRGB::Black);
//  Serial.print("wave\n");
//  for (int i = 0; i < NUM_LEDS; i++) {
//    leds[i].setRGB(colors[0], colors[1], colors[2]);
//    FastLED.show();
//    delay(10);
//    if (i - 2 > 0) {
//      leds[i-2] = CRGB::Black;
//    }
//    FastLED.show();
//  }
}


void trace() {
  fadeToBlackBy(leds, NUM_LEDS, 32);
  int pos = beatsin8(16, 0, NUM_LEDS/2 - 1);
  leds[pos] = CHSV(color, 255, brightness);
  leds[NUM_LEDS-1-pos] = CHSV(color, 255, brightness);
//  pos++;
//  if (pos > NUM_LEDS-1-pos) {
//    fill_solid(leds, NUM_LEDS, CHSV(color, 255, 0));
//    pos = 0;
//  }
}

void build() {
  int pos = beatsin8(16, 0, NUM_LEDS/2);
  leds[pos] = CHSV(color, 255, brightness);
  leds[NUM_LEDS-1-pos] = CHSV(color, 255, brightness);
  if (pos == 30) {
    delay(500);
    aState = 200;
  }
//  pos++;
//  if (pos > NUM_LEDS-1-pos) {
//    fill_solid(leds, NUM_LEDS, CHSV(color, 255, 0));
//    pos = 0;
//  }
}







void destroy() {
  int pos = beatsin8(16, 0, NUM_LEDS/2);
  leds[pos] = CHSV(color, 255, 0);
  leds[NUM_LEDS-1-pos] = CHSV(color, 255, 0);
  if (pos == 0) {
    if (cState == 8) {
      aState = 69;
    } else {
      aState = 4;
    }
  }
//  pos++;
//  if (pos > NUM_LEDS-1-pos) {
//    fill_solid(leds, NUM_LEDS, CHSV(color, 255, 0));
//    pos = 0;
//  }
}


void every_other() {
  uint8_t  brt = beat8(1,255);
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i % 2 == brt % 2) {
      leds[i] = CHSV(color, 255, 0);
    } else {
      leds[i] = CHSV(color, 255, brightness);
    }
  }
}


//void void_fill() {

//}



void rainbow_bounce() {
  uint8_t thisHue = beatsin8(12,0,255);
  fill_rainbow(leds, NUM_LEDS, thisHue, 10);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].fadeToBlackBy(205);
  }
}




void rainbow_wave() {
  uint8_t thisHue = beat8(64, 255);
  fill_rainbow(leds, NUM_LEDS, thisHue, 10);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].fadeToBlackBy(205);
  }
}

void rainbow_trace() {
  fadeToBlackBy(leds, NUM_LEDS, 32); 
  uint8_t thisHue = beat8(64, 255);
  int pos = beatsin8(16, 0, NUM_LEDS/2 - 1);
  leds[pos] = CHSV(thisHue, 255, brightness);
  leds[NUM_LEDS-1-pos] = CHSV(thisHue, 255, brightness);
  //fill_rainbow(leds, NUM_LEDS, thisHue, 10);
  

}


void rainbow_build() {
  uint8_t thisHue = beat8(64, 255);
  int pos = beatsin8(16, 0, NUM_LEDS/2);
  leds[pos] = CHSV(thisHue, 255, brightness);
  leds[NUM_LEDS-1-pos] = CHSV(thisHue, 255, brightness);
  if (pos == 30) {
    delay(500);
    aState = 200;
  }
}


//void blink_led() {
//  fill_solid(leds, NUM_LEDS, CHSV(color, 255, brightness));
//  delay(50);
//  FastLED.show();
//  fill_solid(leds, NUM_LEDS, CHSV(color, 255, 0));
//  delay(50);
//}

//void stack() {
//  int pos = beatsin8(16, 0, NUM_LEDS-anim_pos/2);
//  leds[pos] = CHSV(color, 255, brightness);
//  leds[NUM_LEDS-1-pos] = CHSV(color, 255, brightness);
//  if (pos != NUM_LEDS-anim_pos/2) {
//    leds[pos + 1] = CHSV(color, 255, 0);
//    leds[NUM_LEDS-1-pos] = CHSV(color, 255, 0);
//  }
//  if (pos == anim_pos + 1) {
//    anim_pos++;
//  }
////  pos++;
////  if (pos > NUM_LEDS-1-pos) {
////    fill_solid(leds, NUM_LEDS, CHSV(color, 255, 0));
////    pos = 0;
////  }
//}
