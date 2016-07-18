/*

LECTURE D’UN ENCODEUR ROTATIF KY-040
====================================

# DESCRIPTION DU PROGRAMME
Lecture d’un encodeur rotatif KY-040 pour affichage dans le traceur
série de l’IDE Arduino (CMD-SHIFT-L).
La rotation du bouton est détecté avec des interruptions sur les
broches 2 et 3 du port D.

Ce programme implémente un filtrage des rebond en deux étapes (voir
la procédure “lectureEncodeur”).

Ce programme est basé sur celui d’Oleg Mazurov
https://www.circuitsathome.com/mcu/reading-rotary-encoder-on-arduino

Pour la gestion des interruptions, voir
https://sites.google.com/site/qeewiki/books/avr-guide/external-interrupts-on-the-atmega328

# RÉFÉRENCE DE L’ENCODEUR
http://www.banggood.com/5Pcs-5V-KY-040-Rotary-Encoder-Module-For-Arduino-AVR-PIC-p-951151.html

# CONNEXIONS DE L’ENCODEUR KY-040 SUR ARDUINO NANO
    GND            ⇒   GND
    +              ⇒   +5V
    SW  (bouton)   ⇒   pin D4 (PORT D4)
    DT  (encodeur) ⇒   pin D3 (PORT D3)
    CLK (encodeur) ⇒   pin D2 (PORT D2)

juin 2016, ouilogique.com

*/

#define ENC_A 2
#define ENC_B 3
#define ENC_C 4
#define ENC_PORT PIND
#define BtnRead  ! bitRead( ENC_PORT, ENC_C )

bool encodeurTourne = false;
byte ENC_PORT_VAL = 0;

// Modifier ici la sensibilité de l’encodeur pour qu’il ne réagisse
// pas trop vite. Cette valeur représente le nombre de millisecondes
// avant un changement sur la sortie.
static const long sensibiliteEncodeur = 90;

// Gamme de sortie de l’encodeur. À modifier en fonction de l’application.
static const int8_t minVal = 0;
static const int8_t maxVal = 10;


void setup()
{
  pinMode( ENC_A, INPUT_PULLUP );
  pinMode( ENC_B, INPUT_PULLUP );
  pinMode( ENC_C, INPUT_PULLUP );

  PCICR  |= ( 1<<PCIE2 );
  PCMSK2 |= ( 1<<PCINT18 );
  PCMSK2 |= ( 1<<PCINT19 );
  PCMSK2 |= ( 1<<PCINT20 );
  PCMSK2 |= ( 1<<PCINT21 );

  encodeurTourne = true;

  Serial.begin( 115200 );
}


void loop()
{
  static int8_t compteur = 0;

  // Lecture et affichage des valeurs de l’encodeur
  // dans le traceur série de l’IDE Arduino (CMD-SHIFT-L)
  if( encodeurTourne )
  {
    int8_t encodeurVal = lectureEncodeur();

    // Si le bouton est pressé, on remet le compteur à 0.
    if( encodeurVal == 2 )
    {
      compteur = 0;
      Serial.println( 0 );
      while( BtnRead ){ _delay_ms( 1 ); }
      _delay_ms( 20 );
    }

    // Sinon on met le compteur à jour en le gardant dans la gamme minVal..maxVal.
    else if( encodeurVal != 0 )
    {
      if( compteur < minVal || ( compteur == minVal && encodeurVal < 0 ) )
        { compteur = minVal; }
      else if( compteur > maxVal || ( compteur == maxVal && encodeurVal > 0 ) )
        { compteur = maxVal; }
      else
        { compteur = compteur + encodeurVal; }

      Serial.println( compteur );
    }
    encodeurTourne = false;
  }
}

/*
  lectureEncodeur() retourne

  -1 pour une rotation dans le sens anti-horaire
   0 pour une valeur impossible (rebond)
  +1 pour une rotation dans le sens horaire
  +2 si le bouton de l’encodeur est pressé
*/
int8_t lectureEncodeur()
{
  // Si le bouton est pressé, on retourne 2.
  if( BtnRead )
  {
    return( 2 );
  }

  // Lecture des signaux de l’encodeur et comparaison
  // avec les valeurs possibles dans le code de Gray.
  // Ceci permet de filtrer la plupart des rebonds de l’encodeur.
  // Les valeurs à 0 représentent les transitions impossibles
  // et qui doivent être supprimées (resultat = 0).
  // Les valeurs à 1 représentent les transitions dans le sens horaire.
  // Les valeurs à -1 représentent les transitions dans le sens anti-horaire.
  static const int8_t enc_states[ 16 ] PROGMEM =
    { 0,-1, 1, 0, 1, 0, 0,-1,-1, 0, 0, 1, 0, 1,-1, 0 };
  static uint8_t old_AB = 0;

  old_AB <<= 2;
  bitWrite( old_AB, 0, bitRead( ENC_PORT_VAL, ENC_A ) );
  bitWrite( old_AB, 1, bitRead( ENC_PORT_VAL, ENC_B ) );
  uint8_t resultat = pgm_read_byte( &enc_states[ ( old_AB & 0x0F ) ] );

  // On accumule “maxVal” résulats dans un sens ou dans l’autre
  // avant de bouger. Ceci permet de supprimer les derniers rebonds
  // de l’encodeur qui n’ont pas été filtrés ci-dessus.
  if( resultat != 0 )
  {
    static int8_t resultatCumul = 0;
    const int8_t maxVal = 2;
    resultatCumul += resultat;
    if( resultatCumul >= maxVal )
    {
      resultatCumul = maxVal;
      resultat = 1;
    }
    else if( resultatCumul <= -maxVal )
    {
      resultatCumul = -maxVal;
      resultat = -1;
    }
    else
      { resultat = 0; }
  }

  // On rejete quelques lectures pour limiter la sensibilité de l’encodeur
  // et éviter ainsi qu’il ne tourne trop vite.
  static long lastT = millis();
  if( millis() - lastT < sensibiliteEncodeur )
    { resultat = 0; }
  else
    { lastT = millis(); }

  // On retourne le résultat.
  return( resultat );
}


ISR( PCINT2_vect )
{
  ENC_PORT_VAL = ENC_PORT;
  encodeurTourne = true;
}
