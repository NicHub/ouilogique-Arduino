/*

  https://ouilogique.com/hacker-une-cle-rf433/

  Juillet 2018, ouilogique.com

*/

static const uint8_t MESSAGE_NB_BITS = 128; // Nombre de bits du message
static const uint16_t BIT_DURATION = 280; // En microsecondes
static const uint8_t WAIT_AFTER_SEND = 0; // Attente en millisecondes après l’envoi d’un message.

// Ce tableau stocke les messages à envoyer dans un format qui est
// facile à comprendre pour un humain, mais qui prend beaucoup de
// place en mémoire. Un Arduino UNO ne peut stocker que 231 messages
// de ce type.
static const char MESSAGES_TO_SEND[][ MESSAGE_NB_BITS ] PROGMEM  =
{
  { "11111111111111111111111100000000000000000000000011111111111111111111111100000000000000000000000011111111111111111111111100000000" },
  { "00000000000000000000000011111111111111111111111100000000000000000000000011111111111111111111111100000000000000000000000011111111" },
  { "11101110111010001000100010001000111011101110100010001110100010001000100011101110100010001110100010000000000000000000000000000000" }
};

// Ce tableau stocke les messages à envoyer en format numérique (8
// bits non signés) ce qui est beaucoup plus ennuyeux à lire pour un
// humain, mais qui prend beaucoup moins de place en mémoire. Un
// Arduino UNO peut stocker 1800 messages de ce type.
// La procédure “convertBitStreamToHexArray” permet de convertir les
// messages dans ce format.
static const uint8_t MESSAGES_TO_SEND_HEX[][ MESSAGE_NB_BITS/8 ] PROGMEM  =
{
  { 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00 },
  { 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF },
  { 0xEE, 0xE8, 0x88, 0x88, 0xEE, 0xE8, 0x8E, 0x88, 0x88, 0xEE, 0x88, 0xE8, 0x80, 0x00, 0x00, 0x00 }
};
