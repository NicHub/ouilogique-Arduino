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
const unsigned long delayBounce = 25; // Nb de millisecondes entre deux pressions pour supprimer les rebonds.
const unsigned long delayBump = 50; // Si l’impulsion est plus courte que delayBump, on considère que c’est un choc et on ne tient pas compte du signal.

bool buttonPressed = false;
unsigned long prevMillis;

#define buttonRead ! ( PINB & (1<<PINB1) )

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

  prevMillis = millis();
}

void loop()
{
  if( buttonPressed )
  {
    buttonPressed = false;
    if( millis() - prevMillis > delayBounce )
    {
      unsigned long T1 = millis();
      while( buttonRead ){}
      unsigned long dT = millis() - T1;
      if( dT > delayBump )
      {
        static bool ledVal = false;
        ledVal = ! ledVal;
        digitalWrite( ledPin, ledVal );
        Serial.print( "\nledVal = " );
        Serial.println( ledVal );
      }
      else
      {
        Serial.println( "Choc détecté" );
        delay( 500 );
      }
      Serial.print( "dT = " );
      Serial.println( dT );
      prevMillis = millis();
    }
    else
      Serial.println( "Rebond détecté" );
  }
}

// ISR = Interrupt Service Routine
// Traitement de l’interruption.
ISR( PCINT0_vect )
{
  static bool prevButtonRead = 1;
  bool curButtonRead = buttonRead;

  // On ne réagit qu’au flanc descendant (bouton pressé)
  // et pas au flanc montant (bouton relâché).
  if( curButtonRead == 1 && prevButtonRead == 0 )
    buttonPressed = true;

  prevButtonRead = curButtonRead;
}
