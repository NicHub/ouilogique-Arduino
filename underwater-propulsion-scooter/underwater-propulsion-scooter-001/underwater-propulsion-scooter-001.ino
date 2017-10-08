/*

UNDERWATER PROPULSION SCOOTER
underwater-propulsion-scooter-001.ino

# DESCRIPTION DU PROGRAMME
Programme de base pour un propulseur de plongée.
- 1 bouton pour augmenter la vitesse (broche 8)
- 1 bouton pour diminuer la vitesse (broche 9)
- 1 bouton pour mettre la vitesse à 0 (broche 10)
La vitesse augmente et diminue progressivement.
Dans cet exemple, la vitesse est simplement affichée dans le terminal série.
Dans le programme réel, il suffit de modifier la procédure “sendSpeed”
pour envoyer les commandes de vitesse au propulseur.

Pour la gestion des interruptions, voir
https://sites.google.com/site/qeewiki/books/avr-guide/external-interrupts-on-the-atmega328

# BROCHAGE
Connecter trois boutons sur les broches 8, 9 et 10.
Pour les boutons piezo, la polarité est sans importance.
Note : La résistance de pullup interne de l’Arduino est activée,
donc il n’y a pas besoin de pullup externe.

# MICROCONTRÔLEUR
Testé avec un Arduino UNO, ATmega328P

octobre 2017, ouilogique.com

*/

const int ledPin = LED_BUILTIN;

// Si on change de broche pour les boutons, il faut aussi changer
// l’initialisation de l’interruption dans le setup, le traitement
// de l’interruption dans l’ISR et la lecture des boutons dans
// la procédure “debounce()” !
const int buttonPin1 = 8;
const int buttonPin2 = 9;
const int buttonPin3 = 10;

// Nb de millisecondes entre deux pressions pour supprimer les rebonds.
const unsigned long delayBounce = 25;

bool buttonPressed = false;
bool button1Read = false;
bool button2Read = false;
bool button3Read = false;

const int speedSetupIncrement = 100;
const int speedChangeIncrement = 10;
const int speedChangeWait = 50;
const int speedMax = 2000;
const int speedMin = 1500;
const int speedIdle = 1500;
int speed = speedIdle;

void setup()
{
  Serial.begin( 115200 );
  Serial.println( "\n\nSTART\n\n" );
  pinMode( ledPin, OUTPUT );

  // Initialisation de la broche et de l’interruption des boutons.
  pinMode( buttonPin1, INPUT_PULLUP );
  pinMode( buttonPin2, INPUT_PULLUP );
  pinMode( buttonPin3, INPUT_PULLUP );
  PCICR  |= ( 1<<PCIE0 );  // set PCIE0 to enable PCMSK0 scan
  PCMSK0 |= ( 1<<PCINT0 ) // set PCINT0, PCINT1, PCINT2 to trigger an interrupt on state change
         |  ( 1<<PCINT1 )
         |  ( 1<<PCINT2 );
  sei();                   // turn on interrupts
}

void loop()
{
  if( buttonPressed )
  {
    buttonPressed = false;
    bool buttonPressedDebounced = debounce();
    digitalWrite( ledPin, buttonPressedDebounced );
    if( buttonPressedDebounced )
    {
      if( button1Read ) Serial.print( " X" ); else Serial.print( " _" );
      if( button2Read ) Serial.print( " X" ); else Serial.print( " _" );
      if( button3Read ) Serial.print( " X" ); else Serial.print( " _" );

      if( button1Read ) incrementSpeed();
      if( button2Read ) decrementSpeed();
      if( button3Read ) setIdleSpeed();
      setSpeed();
    }
  }
}

void setIdleSpeed()
{
  speed = speedIdle;
}

void incrementSpeed()
{
  int newSpeed = speed + speedSetupIncrement;
  if( newSpeed <= speedMax )
    speed = newSpeed;
}

void decrementSpeed()
{
  int newSpeed = speed - speedSetupIncrement;
  if( newSpeed >= speedMin )
    speed = newSpeed;
}

void setSpeed()
{
  static int prevSpeed = speedIdle;
  bool speedChanged = false;

  if( prevSpeed == speedMax && speed == speedMax )
    Serial.print( "\tSPEED = SPEED MAX" );
  else if( prevSpeed == speedMin && speed == speedMin )
    Serial.print( "\tSPEED = SPEED MIN" );
  else
  {
    Serial.print( "\tSPEED " );
    Serial.print( prevSpeed );
    Serial.print( " -> " );
    Serial.print( speed );
    speedChanged = true;
  }

  if( speedChanged )
  {
    Serial.print( "\t[ " );
    if( speed > prevSpeed )
    {
      for( int i=prevSpeed+speedChangeIncrement; i<=speed; i+=speedChangeIncrement )
      {
        sendSpeed( i );
      }
    }
    else
    {
      for( int i=prevSpeed-speedChangeIncrement; i>=speed; i-=speedChangeIncrement )
      {
        sendSpeed( i );
      }
    }
    Serial.print( "]\n" );
  }

  prevSpeed = speed;
}

void sendSpeed( int curSpeed )
{
  Serial.print( curSpeed );
  Serial.print( " " );
  delay( speedChangeWait );
}

bool debounce()
{
  // Rejette les pressions de bouton trop rapprochées dans le temps.
  static unsigned long prevT = millis();
  bool timeout = ( millis() - prevT < delayBounce );
  if( timeout ) return false;
  prevT = millis();

  // Intègre les valeurs de lecture des boutons pour éliminer les rebonds.
  uint8_t pulseCount[ 3 ] = { 0, 0, 0 };
  for( uint8_t i=0; i<255; i++ )
  {
    uint8_t PINBval = PINB;
    pulseCount[ 0 ] = ! ( PINBval & (1<<PINB0) ) + pulseCount[ 0 ];
    pulseCount[ 1 ] = ! ( PINBval & (1<<PINB1) ) + pulseCount[ 1 ];
    pulseCount[ 2 ] = ! ( PINBval & (1<<PINB2) ) + pulseCount[ 2 ];
    _delay_ms( 0.2 ); // Temps d’intégration : 0.2 * 255 = 51 ms (+ temps de traitement des instructions de la boucle for)
  }
  bool button1ReadInternal = ( pulseCount[ 0 ] > 204 );
  bool button2ReadInternal = ( pulseCount[ 1 ] > 204 );
  bool button3ReadInternal = ( pulseCount[ 2 ] > 204 );

  // Debug
  Serial.print( "\n" );
  for( int i=0; i<3; i++ )
  {
    Serial.print( pulseCount[ i ] );
    Serial.print( "\t" );
  }

  // On ne met à jour que s’il y a au moins un bouton pressé
  // et si l’état précédent était sans aucun bouton pressé.
  // Si on voulait exclure des cas particuliers, par exemple
  // si deux boutons sont pressés en même temps, on pourrait
  // le faire ici.
  static bool buttonPressedInternal = false;
  buttonPressedInternal = ( (button1ReadInternal || button2ReadInternal || button3ReadInternal) && (! buttonPressedInternal) );
  if( buttonPressedInternal )
  {
    button1Read = button1ReadInternal;
    button2Read = button2ReadInternal;
    button3Read = button3ReadInternal;
  }
  return buttonPressedInternal;
}

// ISR = Interrupt Service Routine
// Traitement de l’interruption.
ISR( PCINT0_vect )
{
  buttonPressed = true;
}
