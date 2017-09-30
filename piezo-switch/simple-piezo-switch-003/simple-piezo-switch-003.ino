/*

TEST D’UN BOUTON PIÉZOÉLECTRIQUE À DEUX FILS.
simple-piezo-switch-003.ino

# DESCRIPTION DU PROGRAMME
Chaque fois que l’on pèse sur le bouton piézoélectrique, la led de
l’Arduino change d’état. Un anti-rebond est intégré au système. Les
changements d’états du bouton sont détectés à l’aide d’une
interruption sur la broche 9.

Voir aussi :
http://ouilogique.com/piezo-switch/

Pour la gestion des interruptions, voir
https://sites.google.com/site/qeewiki/books/avr-guide/external-interrupts-on-the-atmega328

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
const int buttonPin = 9; // ! Si on change de broche, il faut aussi changer l’initialisation de l’interruption dans le setup et le traitement de l’interruption dans l’ISR !
const long delayBounce = 25; // Nb de millisecondes entre deux pressions pour supprimer les rebonds.

bool buttonPressed = false;

void setup()
{
  Serial.begin( 115200 );
  Serial.println( "\n\nSTART\n\n" );
  pinMode( ledPin, OUTPUT );

  // Initialisation de la broche et de l’interruption du bouton.
  pinMode( buttonPin, INPUT_PULLUP );
  PCICR  |= ( 1<<PCIE0 );  // set PCIE0 to enable PCMSK0 scan
  PCMSK0 |= ( 1<<PCINT1 ); // set PCINT0 to trigger an interrupt on state change
  sei();                   // turn on interrupts
}

void loop()
{
  if( buttonPressed )
  {
    buttonPressed = false;
    static long prevMillis = millis();
    if( millis() - prevMillis > delayBounce )
    {
      Serial.println( millis() );
      static bool ledVal = false;
      ledVal = ! ledVal;
      digitalWrite( ledPin, ledVal );
      prevMillis = millis();
    }
  }
}

// ISR = Interrupt Service Routine
// Traitement de l’interruption.
ISR( PCINT0_vect )
{
  static uint8_t portbhistory = 0xFF;     // default is high because the pull-up
  uint8_t changedbits;

  changedbits = PINB ^ portbhistory;
  portbhistory = PINB;

  if( changedbits & (1<<PINB1) && PINB & (1<<PINB1) )
    buttonPressed = true;
}
