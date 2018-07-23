/*

  rf433-spoof.ino

  The croquis permet de hacker une clé RF433 non protégée.

  https://ouilogique.com/hacker-une-cle-rf433/

  Juillet 2018, ouilogique.com

*/

#include "rf433-messages.h"

#define TXBTN1  8
#define TXBTN2  9
#define TXBTN3 10
#define readTXBTN1 ! digitalRead( TXBTN1 )
#define readTXBTN2 ! digitalRead( TXBTN2 )
#define readTXBTN3 ! digitalRead( TXBTN3 )
#define setLED       digitalWrite( LED_BUILTIN, HIGH )
#define clearLED     digitalWrite( LED_BUILTIN, LOW )
#define TXPINpinMode DDRD  |=  ( 1<<7 )
#define setTXPIN     PORTD |=  ( 1<<7 )
#define clearTXPIN   PORTD &= ~( 1<<7 )
#define writeTXPIN( bitvalue ) ( bitvalue ? setTXPIN : clearTXPIN )


void setup()
{
  Serial.begin( 115200 );
  Serial.println( "\n\n\nSTART\n" );
  pinMode( TXBTN1, INPUT_PULLUP );
  pinMode( TXBTN2, INPUT_PULLUP );
  pinMode( TXBTN3, INPUT_PULLUP );
  pinMode( LED_BUILTIN, OUTPUT );
  TXPINpinMode;

  int rows = sizeof(MESSAGES_TO_SEND) / sizeof(MESSAGES_TO_SEND[0]);
  int cols = sizeof(MESSAGES_TO_SEND[0]) / sizeof(char);
  Serial.print( "\nMessages rows = " );
  Serial.print( rows );
  Serial.print( "\nMessages cols = " );
  Serial.print( cols );
  Serial.print( "\nMessages total size = " );
  Serial.print( sizeof(MESSAGES_TO_SEND) );
  Serial.print( "\nMessage individual size = " );
  Serial.print( sizeof(MESSAGES_TO_SEND[0]) );
  Serial.print( "\nchar size = " );
  Serial.print( sizeof(char) );
  Serial.print( "\n" );
}

void convertBitStreamToHexArray( uint8_t message_id )
{
  setLED;
  uint8_t curVal = 0;
  Serial.print( "{ " );
  for( uint8_t cnt=0; cnt<MESSAGE_NB_BITS; cnt++ )
  {
    char bitChar = pgm_read_byte_near( &MESSAGES_TO_SEND[ message_id ][ cnt ] );
    bool bitValue = bitChar == 49;
    curVal += bitValue << (7 - cnt % 8);
    if( cnt % 8 == 7 )
    {
      Serial.print( "0x" );
      if( curVal < 16 )
        Serial.print( "0" );
      Serial.print( curVal, HEX );
      if( cnt < MESSAGE_NB_BITS - 1 )
        Serial.print( ", " );
      curVal = 0;
    }
  }
  Serial.print( " }\n" );
  clearLED;
}

void sendMessageHex( uint8_t message_id )
{
  setLED;
  for( uint8_t cntA=0; cntA<MESSAGE_NB_BITS/8; cntA++ )
  {
    uint8_t curByte = pgm_read_byte_near( &MESSAGES_TO_SEND_HEX[ message_id ][ cntA ] );
    for( int8_t cntB=7; cntB>-1; cntB-- )
    {
      bool bitValue = bitRead( curByte, cntB );
      writeTXPIN( bitValue );
      _delay_us( BIT_DURATION );
      Serial.print( bitValue );
    }
  }
  clearTXPIN;
  Serial.print( "\n" );
  clearLED;
  _delay_ms( WAIT_AFTER_SEND );
}

void sendMessage( uint8_t message_id )
{
  setLED;
  for( uint8_t cnt=0; cnt<MESSAGE_NB_BITS; cnt++ )
  {
    char bitChar = pgm_read_byte_near( &MESSAGES_TO_SEND[ message_id ][ cnt ] );
    bool bitValue = bitChar == 49;
    writeTXPIN( bitValue );
    _delay_us( BIT_DURATION );
    Serial.print( bitChar );
  }
  clearTXPIN;
  Serial.print( "\n" );
  clearLED;
  _delay_ms( WAIT_AFTER_SEND );
}

void loop()
{
  if( readTXBTN1 ){ sendMessageHex( 0 ); }
  if( readTXBTN2 ){ sendMessageHex( 2 ); }
  if( readTXBTN3 ){ sendMessage( 2 ); convertBitStreamToHexArray( 2 ); }
}
