/*

  rf433-spoof.ino

  This sketch can be used to spoof a RF433 sender, typicaly a key fob to open garage doors or switch on lights.

  The idea comes from Andreas Spiess in his video “How to Hack your 433 MHz Devices with a Raspberry and a RTL-SDR Dongle (Weather Station)”

  https://www.youtube.com/watch?v=L0fSEbGEY-Q

  July 2018, ouilogique.com

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

  int rows = sizeof MESSAGES_TO_SEND / sizeof MESSAGES_TO_SEND[0];
  int cols = sizeof MESSAGES_TO_SEND[0] / sizeof(char);
  Serial.print( "\nMessages rows = " );
  Serial.print( rows );
  Serial.print( "\nMessages cols = " );
  Serial.println( cols );
  Serial.print( "\n" );
}

void sendMessage( uint8_t MESSAGE_ID )
{
  setLED;
  for( uint8_t cnt=0; cnt<MESSAGE_NB_BITS; cnt++ )
  {
    char bitChar = pgm_read_byte_near( &MESSAGES_TO_SEND[ MESSAGE_ID ][ cnt ] );
    bool bitValue = bitChar == 49;
    Serial.print( bitChar );
    writeTXPIN( bitValue );
    delayMicroseconds( BIT_DURATION );
  }
  Serial.print( "\n" );
  clearLED;
}

void loop()
{
  if( readTXBTN1 ){ sendMessage( 0 ); }
  if( readTXBTN2 ){ sendMessage( 1 ); }
  if( readTXBTN3 ){ sendMessage( 2 ); }
}
