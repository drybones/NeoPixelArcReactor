#include <Adafruit_NeoPixel.h>
 
#define PIN 6
 
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(7+16+24, PIN, NEO_GRB + NEO_KHZ800);

const float TAU = 2.0 * PI;
const int NUM_MODES = 4;
const float MODE_DURATION[NUM_MODES] = { 300, 0.3, 120, 0.3 };

int mode = 0;
unsigned long lastModeSwitch = millis();
float hue = 0.33;

const uint8_t PROGMEM gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}
 
void loop() {
  unsigned long time = millis();
  if(time - lastModeSwitch > MODE_DURATION[mode]*1000)
  {
    mode = (mode+1) % NUM_MODES;
    lastModeSwitch = time;
  }
  
  switch(mode) 
  {
    case 0:
      hue = 0.7;
      oscillate(0, 1, 40, 255, 0.2, 0.0);
      oscillate(1, 6, 10, 128, 0.2, 0.0);
      spin( 7, 16, 3, 1.0, 64, 255, 1);
      spin(23, 24, 3, 1.0, 64, 255, 0);
      break;
    case 2:
      hue = 0.7;
      oscillate(0, 1, 0, 255, .3, TAU/16);
      oscillate(1, 6, 0, 255, .3, -TAU/16);
      oscillate(7, 16, 0, 255, .3, -TAU/8);
      oscillate(23, 24, 0, 255, .3, -2 * TAU/8);
      break;
    case 1:
    case 3:
      hue = 0.3;
      flicker(0, 1, 32, 255, 40);
      flicker(7, 16, 32, 255, 40);
      setPixelRange(1, 6, strip.Color(0,0,0));
      setPixelRange(23, 24, strip.Color(0,0,0));
      break;
  }
  strip.show();
  delay(10);
}

void oscillate(int startPixel, int pixelLength, int minAlpha, int maxAlpha, float freq, float phase)
{
  unsigned long time = millis();
  int alpha = (maxAlpha-minAlpha)*0.5*(sin((time/1000.0)*freq*TAU + phase)+1.0)+minAlpha;
  setPixelRange(startPixel, pixelLength, getColor(hue, alpha));
}

void spin(int startPixel, int pixelLength, int numPeaks, float freq, int minAlpha, int maxAlpha, int dir) {
  unsigned long time = millis();
  int alpha;
  int pos;
  
  for(int i=0; i<pixelLength; i++)
  {
    alpha = (maxAlpha-minAlpha)*0.5*(1.0+sin(TAU*(numPeaks*(1.0*i/pixelLength)+(time/1000.0)*freq)))+minAlpha;
    if(dir == 0)
    {
      pos = startPixel+i;
    }
    else
    {
      pos = startPixel+pixelLength-i-1;
    }
      strip.setPixelColor(pos, getColor(hue, alpha));
  }  
}

void flicker(int startPixel, int pixelLength, int minAlpha, int maxAlpha, int alphaDelta)
{
  static int alpha;
  static int dir;
  
  int flip = random(32);
  if(flip > 20) {
    dir = 1 - dir;
  }
  if (dir == 1) {
    alpha += alphaDelta;
  }
  if (dir == 0) {
    alpha -= alphaDelta;
  }
  if (alpha < minAlpha) {
    alpha = minAlpha;
    dir = 1;
  }
  if (alpha > maxAlpha) {
    alpha = maxAlpha;
    dir = 0;
  }
  setPixelRange(startPixel, pixelLength, getColor(hue, alpha));
}

void setPixelRange(int startPixel, int pixelLength, uint32_t c) {
  for(int i=startPixel; i<(startPixel+pixelLength); i++) {
      strip.setPixelColor(i, c);
  }
}

uint32_t getColor(float hue, int alpha) {
  uint8_t w = 255*hue;
  float brightness = pgm_read_byte(&gamma[alpha]) / 255.0;
  if(w < 85) {
    return Adafruit_NeoPixel::Color(w*3*brightness, (255-w*3)*brightness, 0);
  } else if(w < 170) {
    w -= 85;
    return Adafruit_NeoPixel::Color((255-w*3)*brightness, 0, w*3*brightness);
  } else {
    w -= 170;
    return Adafruit_NeoPixel::Color(0, w*3*brightness, (255-w*3)*brightness);
  }  
}

  
