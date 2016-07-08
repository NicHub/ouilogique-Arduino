/*

DÉMO DU LECTEUR MP3 DFPlayer
============================


http://ouilogique.com/tests_DFPlayer/


## RÉFÉRENCES
http://www.banggood.com/3Pcs-DFPlayer-Mini-MP3-Player-Module-For-Arduino-p-981366.html
http://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299
http://www.dfrobot.com/image/data/DFR0299/DFPlayer%20Mini%20Manul.pdf
http://www.trainelectronics.com/Arduino/MP3Sound/TalkingTemperature/FN-M16P%20Embedded%20MP3%20Audio%20Module%20Datasheet.pdf


## NOTES
Le DFPlayer communique avec le port série. Comme l’Arduino Nano n’a
qu’un UART, la librairie de DFPlayer permet d’utiliser un port série
logiciel. À priori, n’importe quelle broche digitale de l’Arduino Nano
peut être utilisée à cet effet. J’ai testé avec les couples de broches
10 & 11 ainsi que A0 & A1 et ça fonctionne.

!!! IL FAUT FAIRE ATTENTION DE CONNECTER LE RX DU DFPlayer AU TX DE
L’ARDUINO ET INVERSÉMENT POUR LES DEUX AUTRES SIGNAUX !!!

Pour le déverminage, il peut être utile de passer au morceau suivant
ou précédent en mettant les broches 11 (IO2) et 9 (IO1) à la masse
pour forcer la lecture.

Si on a pas d’haut-parleur, un buzzer peut faire l’affaire lors de la
mise en route.


## NOTES SUR LA  CARTE SD
- Jusqu’à 32 Go (testé avec 32 Go ⇒ OK)
- Formaté en FAT16 ou en FAT32 (testé FAT32 formaté avec OSX ⇒ OK)
- Doit contenir un répertoire appelé “mp3”
- Le répertoire appelé “mp3” peut optionnelement contenir des répertoires appelés “001”, “002”, ...
- Les noms des fichiers doivent commencer par 4 digits et finir par l’extension “.mp3”

/Volumes/DFPLAYER/mp3
├── 0001×××.mp3
├── 0002×××.mp3
├── 0003×××.mp3
├── 0004×××.mp3


## CONNEXIONS DFPlayer
  VCC   (pin 1)     ⇒     5V Arduino Nano
  RX    (pin 2)  ¹  ⇒     software TX Arduino Nano (pin 10) ²
  TX    (pin 3)     ⇒     software RX Arduino Nano (pin 11) ³
  SPK2  (pin 6)     ⇒     Haut-parleur - (ou buzzer -)
  GND   (pin 7)     ⇒     GND Arduino Nano
  SPK1  (pin 8)     ⇒     Haut-parleur + (ou buzzer +)

¹ Ajouter une résistance d’1 kΩ en série pour éviter les ronflements
² fonctionne aussi sur A0
³ fonctionne aussi sur A1


juillet 2016, ouilogique.com

*/

#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

SoftwareSerial DFPlayerSerial( 10, 11 ); // RX, TX

void setup()
{
  Serial.begin( 115200 );
  Serial.print( "\n\nDFPlayer_Mini_Mp3\n" );

  DFPlayerSerial.begin( 9600 );
  mp3_set_serial( DFPlayerSerial );
  mp3_set_device( 2 );  // Carte SD
  mp3_set_volume( 20 ); // 0-30
  mp3_play( 1 );        // 0001×××.mp3
  _delay_ms( 3000 );
}

void loop()
{
  mp3_next();
  _delay_ms( 3000 );
}

