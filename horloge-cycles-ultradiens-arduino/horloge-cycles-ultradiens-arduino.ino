/*

# HORLOGE À CYCLES ULTRADIENS

http://ouilogique.com/horloge_cycles_ultradiens/

## DESCRIPTION DU PROGRAMME
Ce programme affiche le pourcentage d’attention d’une personne en fonction
de l’heure. Il se base sur les hypothèses suivantes :

- Le corps humain est soumis à des cycles d’attention d’une durée d’une
  heure et demie, soit 16 cycles de 5400 secondes par jour.
- Lors de ces cycles, l’attention passe par un minimum et par un maximum et
  peut être représentée sous la forme d’un cosinus.
- Le seul paramètre qui change d’une personne à l’autre est le déphasage de
  la courbe.

Pour utiliser ce programme, il faut donc connaître une des 16 heures
d’attention maximum possibles lors d’une journée et de modifier la constante
“heureAttentionMax” en conséquence.

Pour mettre à jour l’heure de l’horloge, il faut changer la valeur de
“avecSerial” à “true” et recharger le programme sur le microcontrôleur.
Cette valeur est à “false” par défaut pour limiter l’utilisation de la RAM.

## CONNEXIONS Arduino Nano
    I²C       GND   ⇒   GND
    I²C       VCC   ⇒   +5V
    I²C       SDA   ⇒   pin A4 + pullup 4.7 kΩ
    I²C       SCL   ⇒   pin A5 + pullup 4.7 kΩ
    Buzzer    +     ⇒   pin A0 (PORT C0)
    Buzzer    -     ⇒   GND
    Bouton 1  +     ⇒   pin D2 (PORT D2)
    Bouton 1  -     ⇒   GND
    Bouton 2  +     ⇒   pin D3 (PORT D3)
    Bouton 2  -     ⇒   GND

## HORLOGE DS1307 I²C
    ### RÉFÉRENCE AliExpress
    http://www.aliexpress.com/item/5pcs-lot-Tiny-RTC-I2C-AT24C32-DS1307-Real-Time-Clock-Module-Board-For-Arduino-With-A/32327865928.html

    ### ADRESSES I²C
    0x50 (EEPROM AT24C32)
    0x68 (DS1307)

    ### LIBRAIRIE Adafruit
    https://github.com/adafruit/RTClib.git

    ### CONNEXIONS
    GND    GND
    VCC    +5V
    SDA    pin A4 + pullup 4.7 kΩ
    SCL    pin A5 + pullup 4.7 kΩ

## ÉCRAN OLED 128×64 I²C (compatible SSD1306)
    ### RÉFÉRENCE AliExpress
    http://www.aliexpress.com/item/1Pcs-Yellow-blue-double-color-128X64-OLED-LCD-LED-Display-Module-For-Arduino-0-96/32305641669.html

    ### ADRESSE I²C
    0x3C

    ### LIBRAIRIE Adafruit
    https://github.com/adafruit/Adafruit_SSD1306.git

    ### CONNEXIONS
    GND    GND
    VDD    +5V
    SDA    pin A4 + pullup 4.7 kΩ
    SCK    pin A5 + pullup 4.7 kΩ

## MICROCONTRÔLEUR
    Clone Arduino Nano

juin 2016, ouilogique.com

*/

#include <EEPROM.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <math.h>
#include "RTClib.h"
RTC_DS1307 RTC = RTC_DS1307();
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4 // OLED_RESET=4 pour Arduino Nano // OLED_RESET=16 pour ESP8266
Adafruit_SSD1306 display( OLED_RESET );
#if( SSD1306_LCDHEIGHT != 64 )
#error( "Height incorrect, please fix Adafruit_SSD1306.h!" );
#endif
#include "aTunes.h"
#define carillonPin A0
#define carillonBit PORTC0
#define dXCarillon 5
#define dYCarillon 25
const int adrCarillon = 0;
bool timerOK = false;
const int bBtn1  = PORTD2;

#define avecSerial false

// Modifier ici l’heure d’attention maximum.
// Par exemple, si 7 h 15 est une heure d’attention maximum :
// heureAttentionMax = 7 h 15
// heureAttentionMax = MOD( 7*3600 + 15*60, 5400 )
// heureAttentionMax = 4500 s
// (5400 est le nombre de secondes dans 1 h 30)
const long heureAttentionMax = 4500;
const byte displayWidth = 128;
static const unsigned char cosinus_cmap[ displayWidth ] PROGMEM =
{
  63, 63, 63, 63, 63, 62, 62, 62,
  62, 62, 62, 61, 61, 61, 61, 60,
  60, 60, 59, 59, 58, 58, 57, 57,
  56, 56, 55, 55, 54, 54, 53, 53,
  52, 52, 51, 51, 50, 50, 49, 49,
  48, 48, 47, 47, 46, 46, 45, 45,
  45, 44, 44, 44, 44, 43, 43, 43,
  43, 43, 43, 42, 42, 42, 42, 42,
  42, 42, 42, 42, 42, 43, 43, 43,
  43, 43, 43, 44, 44, 44, 44, 45,
  45, 45, 46, 46, 47, 47, 48, 48,
  49, 49, 50, 50, 51, 51, 52, 52,
  53, 53, 54, 54, 55, 55, 56, 56,
  57, 57, 58, 58, 59, 59, 60, 60,
  60, 61, 61, 61, 61, 62, 62, 62,
  62, 62, 62, 63, 63, 63, 63, 63
};

void prepareCourbeCycle( int16_t frac16eJourPx )
{
  unsigned char py;
  // Partie de la courbe avec remplissage
  for( int16_t px=0; px<=frac16eJourPx; px++ )
  {
    py = pgm_read_byte( &cosinus_cmap[ px ] );
    display.drawLine( px, display.height()-1, px, py, WHITE );
  }
  // Partie de la courbe sans remplissage
  for( int16_t px=frac16eJourPx+1; px<displayWidth; px++ )
  {
    py = pgm_read_byte( &cosinus_cmap[ px ] );
    display.drawPixel( px, py, WHITE );
  }
}

#if avecSerial
void serialEvent()
{
  // Cette procédure permet de régler l’heure de l’horloge
  // via le bus RS232.
  // Exemple de commande à envoyer :
  // 2016,6,14,10,06,10

  const byte nbCharMax = 19;
  char str[ nbCharMax ] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

  // Lecture de la nouvelle heure sur le port série
  byte compteur = 0;
  while( Serial.available() && compteur <= nbCharMax )
  {
    char inChar = ( char )Serial.read();
    if( inChar == '\n' ) break;
    str[ compteur++ ] = inChar;
  }

  // Séparation des éléments
  const char sep[ 2 ] = ",";
  char *token;
  int dateHeureInt[ 6 ];
  compteur = 0;
  // Trouve le premier séparateur
  token = strtok( str, sep );
  dateHeureInt[ 0 ] = atoi( token );
  // Trouve les autres séparateurs
  while( token != NULL )
  {
    token = strtok( NULL, sep );
    dateHeureInt[ ++compteur ] = atoi( token );
  }

  // Réglage de l’horloge et affichage de la nouvelle heure
  RTC.adjust( DateTime(
    dateHeureInt[ 0 ],
    dateHeureInt[ 1 ],
    dateHeureInt[ 2 ],
    dateHeureInt[ 3 ],
    dateHeureInt[ 4 ],
    dateHeureInt[ 5 ] ) );
  DateTime now = RTC.now();
  char nowChar[ 19 ];
  sprintf(
    nowChar,
    "Heure actuelle : %1d-%02d-%02d %02d:%02d:%02d",
    now.year(), now.month(),  now.day(),
    now.hour(), now.minute(), now.second() );
  Serial.println( nowChar );
}
#endif

void boutonPress()
{
  static unsigned long lastMillis;
  unsigned long dT = millis() - lastMillis;
  if( dT < 1000 )
    { return; }
  EEPROM.write( 0, ! EEPROM.read( adrCarillon ) );
  lastMillis = millis();
  timerOK = true;
}

void prepareIconeCarillonBase()
{
  display.drawLine(  0+dXCarillon,  5+dYCarillon,  0+dXCarillon, 11+dYCarillon, WHITE );
  display.drawLine(  1+dXCarillon,  5+dYCarillon,  1+dXCarillon, 11+dYCarillon, WHITE );
  display.drawLine(  2+dXCarillon,  5+dYCarillon,  2+dXCarillon, 11+dYCarillon, WHITE );
  display.drawLine(  3+dXCarillon,  5+dYCarillon,  3+dXCarillon, 11+dYCarillon, WHITE );
  display.drawLine(  4+dXCarillon,  5+dYCarillon,  4+dXCarillon, 11+dYCarillon, WHITE );
  display.drawLine(  5+dXCarillon,  5+dYCarillon,  5+dXCarillon, 11+dYCarillon, WHITE );
  display.drawLine(  6+dXCarillon,  4+dYCarillon,  6+dXCarillon, 12+dYCarillon, WHITE );
  display.drawLine(  7+dXCarillon,  3+dYCarillon,  7+dXCarillon, 13+dYCarillon, WHITE );
  display.drawLine(  8+dXCarillon,  2+dYCarillon,  8+dXCarillon, 14+dYCarillon, WHITE );
  display.drawLine(  9+dXCarillon,  1+dYCarillon,  9+dXCarillon, 15+dYCarillon, WHITE );
  display.drawLine( 10+dXCarillon,  0+dYCarillon, 10+dXCarillon, 16+dYCarillon, WHITE );
}

void prepareIconeCarillonON()
{
  display.drawLine( 13+dXCarillon,  5+dYCarillon, 13+dXCarillon,  6+dYCarillon, WHITE );
  display.drawLine( 13+dXCarillon, 10+dYCarillon, 13+dXCarillon, 11+dYCarillon, WHITE );
  display.drawLine( 14+dXCarillon,  6+dYCarillon, 14+dXCarillon, 10+dYCarillon, WHITE );

  display.drawLine( 15+dXCarillon,  3+dYCarillon, 15+dXCarillon,  4+dYCarillon, WHITE );
  display.drawLine( 15+dXCarillon, 12+dYCarillon, 15+dXCarillon, 13+dYCarillon, WHITE );
  display.drawLine( 16+dXCarillon,  4+dYCarillon, 16+dXCarillon,  5+dYCarillon, WHITE );
  display.drawLine( 16+dXCarillon, 11+dYCarillon, 16+dXCarillon, 12+dYCarillon, WHITE );
  display.drawLine( 17+dXCarillon,  5+dYCarillon, 17+dXCarillon, 11+dYCarillon, WHITE );

  display.drawLine( 17+dXCarillon,  1+dYCarillon, 17+dXCarillon,  2+dYCarillon, WHITE );
  display.drawLine( 17+dXCarillon, 14+dYCarillon, 17+dXCarillon, 15+dYCarillon, WHITE );
  display.drawLine( 18+dXCarillon,  2+dYCarillon, 18+dXCarillon,  3+dYCarillon, WHITE );
  display.drawLine( 18+dXCarillon, 13+dYCarillon, 18+dXCarillon, 14+dYCarillon, WHITE );
  display.drawLine( 19+dXCarillon,  3+dYCarillon, 19+dXCarillon,  5+dYCarillon, WHITE );
  display.drawLine( 19+dXCarillon, 11+dYCarillon, 19+dXCarillon, 14+dYCarillon, WHITE );
  display.drawLine( 20+dXCarillon,  5+dYCarillon, 20+dXCarillon, 12+dYCarillon, WHITE );
}

void prepareIconeCarillonOFF()
{
  display.drawLine( 14+dXCarillon,  5+dYCarillon, 20+dXCarillon, 11+dYCarillon, WHITE );
  display.drawLine( 14+dXCarillon, 11+dYCarillon, 20+dXCarillon,  5+dYCarillon, WHITE );
}

void prepareIconeCarillon()
{
  display.setTextSize( 1 );
  display.setCursor( 2, 25 );
  prepareIconeCarillonBase();
  if( EEPROM.read( adrCarillon ) )
    { prepareIconeCarillonON(); }
  else
    { prepareIconeCarillonOFF(); }
}

void setup()
{
  #if avecSerial
  // Initalisation de la communication série pour régler l’heure
  Serial.begin( 115200 );
  #endif

  // Initialisation du bouton en INPUT_PULLUP
  bitClear( DDRD, bBtn1 );
  bitSet( PORTD, bBtn1 );

  // Initialisation de l’interruption du bouton
  attachInterrupt( 0, boutonPress, FALLING );

  // Initialisation du Timer 1 à 1 s
  cli();                   // disable global interrupts
  TCCR1A = 0b00000000;     // set entire TCCR1A register to 0
  TCCR1B = 0b00000000
         | (1 << WGM12)    // turn on CTC mode:
         | (1 << CS10)     // Set CS10 and CS12 bits for 1024 prescaler:
         | (1 << CS12);
  OCR1A = 15624;           // set compare match register to desired timer count
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt:
  sei();                   // enable global interrupts

  // Initialisation de l’horloge
  RTC.begin();

  // Initialisation de l’écran
  display.begin( SSD1306_SWITCHCAPVCC, 0x3C );
  display.clearDisplay();
  display.setTextColor( INVERSE );
  display.setTextSize( 2 );
  display.setCursor( 25, 0 );
  display.print( F( "HORLOGE" ) );
  display.setCursor( 20, 19 );
  display.print( F( "A CYCLES" ) );
  display.setCursor( 5, 40 );
  display.print( F( "ULTRADIENS" ) );
  display.display();

  // Initialisation du carillon
  carillon();
}

void horloge()
{
  // ****
  // Calculs du pourcentage du cycle d’attention (cycleAtt)
  // et du temps équivalent en 1/16e de jour exprimé en pixels (frac16eJourPx)
  // Temps d’exécution sur l’ATmega328P ≅ 1.3 ms
  // **

  // lecture de l’heure actuelle
  DateTime now = RTC.now();

  // Calcul du temps équivalent en 1/16e de jour
  // NB : - Il y a 16 cycles d’1 h 30 dans 24 h
  //      - 1 h 30 = 5400 s
  long frac16eJour = ( now.secondstime() - heureAttentionMax ) % 5400;

  // Calcul du pourcentage du cycle d’attention
  // 2 * π * 16 / 86400 = 0.0011635528
  double cycleAtt = 100.0 * 0.5 * ( 1.0 + cos( ( double )( frac16eJour ) * 0.0011635528 ) );

  // Conversion de “frac16eJour” en pixels.
  // Le cosinus est affiché avec un déphasage d’une demi-période, donc
  // les valeurs de la 1ère moitié du cycle correspondent à la partie droite du cosinus et
  // les valeurs de la 2e moitié du cycle correspondent à la partie gauche du cosinus.
  // Le code ci-dessous permute les deux moitiés pour qu’elles s’affichent du bon côté.
  int16_t frac16eJourPx;
  if( frac16eJour < 5400/2 )
    { frac16eJourPx = map( frac16eJour,
                              0,              5400/2-1,
                              displayWidth/2, displayWidth-1 ); }
  else
    { frac16eJourPx = map( frac16eJour,
                              5400/2,         5400-1,
                              0,              displayWidth/2-1 ); }


  // ****
  //  Affichage des résultats
  // Temps d’exécution sur l’ATmega328P ≅ 69 ms
  // **

  // Effacement de l’écran
  display.clearDisplay();

  // Préparation de l’affichage de la date
  display.setTextSize( 1 );
  if( now.day() < 10 )
    { display.setCursor( 6, 0 ); }
  else
    { display.setCursor( 0, 0 ); }
  display.print( now.day() );
  display.setCursor( 15, 0 );
  switch( now.month() )
  {
    case  1: display.print( F( "JAN"  ) ); break;
    case  2: display.print( F( "FEV"  ) ); break;
    case  3: display.print( F( "MARS" ) ); break;
    case  4: display.print( F( "AVR"  ) ); break;
    case  5: display.print( F( "MAI"  ) ); break;
    case  6: display.print( F( "JUIN" ) ); break;
    case  7: display.print( F( "JUIL" ) ); break;
    case  8: display.print( F( "AOUT" ) ); break;
    case  9: display.print( F( "SEPT" ) ); break;
    case 10: display.print( F( "OCT"  ) ); break;
    case 11: display.print( F( "NOV"  ) ); break;
    case 12: display.print( F( "DEC"  ) );
  }
  display.setCursor( 15, 9 );
  display.print( now.year() );

  // Préparation de l’affichage de l’heure
  char texteAffichage[ 5 ];
  display.setTextSize( 2 );
  display.setCursor( 54, 0 );
  sprintf( texteAffichage, "%2d", now.hour() );
  display.print( texteAffichage );

  // Préparation de l’affichage du séparateur
  display.setCursor( 76, 0 );
  display.print( F( ":" ) );

  // Préparation de l’affichage des minutes
  display.setCursor( 86, 0 );
  sprintf( texteAffichage, "%02d", now.minute() );
  display.print( texteAffichage );

  // Préparation de l’affichage des secondes
  display.setTextSize( 1 );
  display.setCursor( 112, 0 );
  sprintf( texteAffichage, "%02d", now.second() );
  display.print( texteAffichage );

  // Préparation de l’affichage de la courbe du cycle
  prepareCourbeCycle( frac16eJourPx );

  // Préparation de l’affichage du pourcentage du cycle
  display.setTextSize( 2 );
  #define hY 48
  #if false // Affichage avec 3 chiffres significatifs pour le déverminage
    if( cycleAtt < 0.005 )
      { display.setCursor( 53, hY ); display.print( cycleAtt, 0 ); }
    else if( cycleAtt < 9.995 )
      { display.setCursor( 34, hY ); display.print( cycleAtt, 2 ); }
    else if( cycleAtt < 99.95 )
      { display.setCursor( 34, hY ); display.print( cycleAtt, 1 ); }
    else
      { display.setCursor( 42, hY ); display.print( cycleAtt, 0 ); }
  #else // Affichage sans décimales pour l’utilisation normale
    int16_t tx;
    if( cycleAtt < 9.5 )       { tx = 52; }
    else if( cycleAtt < 99.5 ) { tx = 47; }
    else                       { tx = 41; }
    display.setCursor( tx, hY );
    display.print( cycleAtt, 0 );
    display.setCursor( display.getCursorX() + 3, hY );
  #endif
  display.print( F( "\x25" ) ); // signe %

  // Préparation de l’affichage de l’icône du carillon
  prepareIconeCarillon();

  // Met à jour l’affichage
  display.display();

  // Sonne lorsque le cycle est au maximum
  // Cette procédure est bloquante, donc l’horloge ne sera
  // pas mise à jour pendant la sonnerie !
  if( frac16eJour == 0  )
    { carillon(); }
}

void carillon()
{
  if( EEPROM.read( adrCarillon ) )
  {
    // Carillonne
    MarioBros( carillonPin );

    // On met le carillon en INPUT_PULLUP pour éviter les ronflements
    // parasites causés par l’écran.
    // Il n’y a pas besoin de le mettre en OUTPUT avant l’utilisation,
    // car la procédure “tone” s’en charge.
    bitClear( DDRC, carillonBit );
    bitSet( PORTC, carillonBit );
  }
}

void loop()
{
  if( timerOK )
  {
    horloge();
    timerOK = false;
  }
}

ISR( TIMER1_COMPA_vect )
{
  timerOK = true;
}