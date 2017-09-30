/*

TEST D’UN BOUTON PIÉZOÉLECTRIQUE À DEUX FILS.
simple-piezo-switch-002.ino

# DESCRIPTION DU PROGRAMME
Chaque fois que l’on pèse sur le bouton piézoélectrique,
la led de l’Arduino change d’état. Un anti-rebond de
250 ms est intégré au système.

Voir aussi :
http://ouilogique.com/piezo-switch/

# BROCHAGE
La polarité est sans importance.
fil 1 : GND
fil 2 : pin 9
Note : La résistance de pullup interne de l’Arduino est activée,
donc il n’y a pas besoin de pullup externe.

# MICROCONTRÔLEUR
Testé avec un Arduino Nano, ATmega328P

septembre 2017, ouilogique.com

*/

const int ledPin = LED_BUILTIN;
const int buttonPin = 9;
#define buttonRead ! digitalRead( buttonPin )

void setup()
{
  Serial.begin( 115200 );
  Serial.println( "\n\nSTART\n\n" );
  pinMode( ledPin, OUTPUT );
  pinMode( buttonPin, INPUT_PULLUP );
}

void loop()
{
  static long T1 = millis();
  static bool ledVal = false;
  if( buttonRead && (millis()-T1>250) )
  {
    Serial.println( millis() );
    ledVal = ! ledVal;
    digitalWrite( ledPin, ledVal );
    T1 = millis();
  }
}
