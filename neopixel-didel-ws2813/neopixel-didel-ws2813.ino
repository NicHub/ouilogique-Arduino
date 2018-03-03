/*
  NeoPixel Test pour Didel WS2813

  Programmé avec la bibliothèque NeoPixel d’Adafruit
  https://github.com/adafruit/Adafruit_NeoPixel

  février-mars 2018
  ouilogique.com
*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN            A0
#define NUMPIXELS      2
#define buttonPin      PORTD2
#define ledPin         LED_BUILTIN
#define maxBrightness  255 // 64

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
const uint32_t C_RED     = pixels.Color(255,   0,   0);
const uint32_t C_GREEN   = pixels.Color(  0, 255,   0);
const uint32_t C_BLUE    = pixels.Color(  0,   0, 255);
const uint32_t C_WHITE   = pixels.Color(128, 255, 255);
const uint32_t C_ORANGE  = pixels.Color(255, 128,   0);

const uint32_t COLORS[] =
{
  C_ORANGE,
  C_RED,
  C_GREEN,
  C_BLUE,
  C_WHITE
};
const uint8_t NUMCOLOR = sizeof(COLORS) / sizeof(COLORS[0]);

bool wasButtonPressedOnce = false;
bool buttonToggle = false;
long T1 = millis();

void buttonPressed()
{
  if(millis() - T1 < 400)
    return;
  T1 = millis();
  wasButtonPressedOnce = true;
  buttonToggle = true;
}

bool semaphore()
{
  if(! wasButtonPressedOnce)
  {
    return false;
  }
  digitalWrite(ledPin, HIGH);
  for(int colorid=0; colorid<NUMCOLOR; colorid++)
  {
    Serial.print("COLORS[colorid] = ");
    Serial.println(COLORS[colorid]);
    uint32_t couleur = COLORS[colorid];
    pixels.setBrightness(maxBrightness);
    for(int pxid=0; pxid<NUMPIXELS; pxid++)
    {
      pixels.setPixelColor(pxid, couleur);
    }
    pixels.show();
    buttonToggle = false;
    while(! buttonToggle)
    {
      _delay_ms(10);
    }
  }
  wasButtonPressedOnce = false;
  digitalWrite(ledPin, LOW);
  uint32_t couleur = pixels.Color(0, 0, 0);
  pixels.setBrightness(0);
  for(int pxid=0; pxid<NUMPIXELS; pxid++)
  {
    pixels.setPixelColor(pxid, couleur);
  }
  pixels.show();
  return true;
}

void setOrange()
{
  uint32_t couleur = COLORS[0];
  for(int pxid=0; pxid<NUMPIXELS; pxid++)
  {
    pixels.setPixelColor(pxid, couleur);
  }
  pixels.setBrightness(maxBrightness);
  pixels.show();
}

void setup()
{
  Serial.begin(115200);
  Serial.print("\n\n\nSTART TEST WS2813\n");
  pixels.begin();
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonPressed, FALLING);
  digitalWrite(ledPin, LOW);
}

void loop()
{
  for(int colorid=0; colorid<NUMCOLOR; colorid++)
  {
    uint32_t couleur = COLORS[colorid];

    for(int pxid=0; pxid<NUMPIXELS; pxid++)
    {
      for(int brightness=0; brightness<256; brightness++)
      {
        pixels.setBrightness(brightness);
        pixels.setPixelColor(pxid, couleur);
        pixels.show();
        if(semaphore()) return;
        _delay_ms(10);
      }
      _delay_ms(1000);
    }

    pixels.setBrightness(maxBrightness);
    for(int pxid=0; pxid<NUMPIXELS; pxid++)
    {
      pixels.setPixelColor(pxid, couleur);
    }
    pixels.show();
    if(semaphore()) return;
    _delay_ms(3000);
  }
}
