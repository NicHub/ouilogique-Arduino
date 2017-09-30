/*

TEST D’UN BOUTON PIÉZOÉLECTRIQUE À DEUX FILS.
simple-piezo-switch-001.ino

# DESCRIPTION DU PROGRAMME
Chaque fois que l’on pèse sur le bouton piézoélectrique,
la led de l’Arduino s’allume quelques millisecondes.
Ceci montre que le piézo envoie juste une impulsion
à chaque pression.

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
  pinMode( ledPin, OUTPUT );
  pinMode( buttonPin, INPUT_PULLUP );
}

void loop()
{
  digitalWrite( ledPin, buttonRead );
}
