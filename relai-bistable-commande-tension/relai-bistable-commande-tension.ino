/*

COMMUTATION TEMPORISÉE D’UN RELAI BISTABLE
==========================================

Pour la gestion des interruptions, voir
https://sites.google.com/site/qeewiki/books/avr-guide/external-interrupts-on-the-atmega328

Pour la gestion de l’énergie, voir
http://playground.arduino.cc/Learning/ArduinoSleepCode

avril 2017, ouilogique.com

*/

#include <avr/sleep.h>

const long dTbeforeOPEN     = 60000;
const long dTtoggle         = 50;

const int buttonPin         = PD2; // PORT D2 ⇒ pin 2
const int COIL1Pin          = PB0; // PORT B0 ⇒ pin 8
const int COIL2Pin          = PB1; // PORT B1 ⇒ pin 9
const int LEDboardPin       = PB5; // PORT B5 ⇒ pin 13

// #define COIL1HIGH       PORTB |=  ( 1<<COIL1Pin )
// #define COIL1LOW        PORTB &= ~( 1<<COIL1Pin )
// #define COIL2HIGH       PORTB |=  ( 1<<COIL2Pin )
// #define COIL2LOW        PORTB &= ~( 1<<COIL2Pin )
#define LEDboardHIGH    PORTB |=  ( 1<<LEDboardPin )
#define LEDboardLOW     PORTB &= ~( 1<<LEDboardPin )
#define COILsCLOSE      PORTB =  PORTB   |  ( 1<<COIL1Pin )   & ~( 1<<COIL2Pin )
#define COILsOPEN       PORTB =  PORTB   & ~( 1<<COIL1Pin )   |  ( 1<<COIL2Pin )
#define COILsRELEASE    PORTB =  PORTB   & ~( 1<<COIL1Pin )   & ~( 1<<COIL2Pin )
#define buttonREAD      ! ( PIND & ( 1<<buttonPin ) )

long T1                     = 0;
bool signalOuvrirRelai      = false;
bool signalResetTimer       = false;
bool relaiState             = false;

void ouvrirRelai()
{
  if( ! relaiState )
  {
    COILsOPEN;
    _delay_ms( dTtoggle );
    COILsRELEASE;
    relaiState = true;
  }
}

void fermerRelai()
{
  if( relaiState )
  {
    COILsCLOSE;
    _delay_ms( dTtoggle );
    COILsRELEASE;
    relaiState = false;
  }
}

void setup()
{
  // Désactivation de l’ADC pour diminuer la consommation d’énergie.
  ADCSRA = 0;

  // Configuration des ports :
  // - Le bouton est en INPUT.
  // - Les sorties en OUTPUT.
  // - Toutes les broches non utilisées sont en INPUT_PULLUP,
  //   car c’est la configuration qui consomme le moins d’énergie.
  DDRD &= ~( 1<<buttonPin );
  DDRB |= ( 1<<COIL1Pin ) | ( 1<<COIL2Pin ) | ( 1<<LEDboardPin );
  DDRC  = B00000000;
  PORTD = B11111111 & ~( 1<<buttonPin );
  PORTB = B11111111 & ~( 1<<COIL1Pin ) & ~( 1<<COIL2Pin ) & ~( 1<<LEDboardPin );
  PORTC = B11111111;

  // On indique que le setup à commencé en allumant la LED du board.
  LEDboardHIGH;

  // Initialisation du relai.
  ouvrirRelai();

  // Initialisation de l’interruption du bouton.
  // Configure PD2 pour générer une interruption
  // sur les changements d’état.
  EICRA = EICRA | ( 1<<ISC00 ) & ~( 1<<ISC01 );
  EIMSK |=  ( 1<<INT0 );
  sei();

  // On indique que le setup est terminé en éteignant la LED du board.
  LEDboardLOW;

  // Dodo
  sleepNow();
}

void sleepNow()
{
  set_sleep_mode( SLEEP_MODE_PWR_DOWN );
  sleep_enable();
  sleep_mode();     // Dodo à partir d’ici.
  sleep_disable();  // Et quand on se réveille, on inhibe le sommeil.
}

void loop()
{
  // Machine d’états.
  if( signalOuvrirRelai )
  {
    // État 1.
    if( signalResetTimer )
    {
      signalResetTimer = false;
      LEDboardHIGH;
      T1 = millis();
      fermerRelai();
    }
    // État 2.
    if( buttonREAD )
    {
      while( buttonREAD );;
      T1 = millis();
    }
    // État 3.
    if( millis() - T1 >= dTbeforeOPEN )
    {
      signalOuvrirRelai = false;
      ouvrirRelai();
      LEDboardLOW;
      sleepNow();
    }
  }
}

ISR( INT0_vect )
{
  signalResetTimer  = true;
  signalOuvrirRelai = true;
}
