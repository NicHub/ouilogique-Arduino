/*

  YWRobot
  Easy Module Shield v1

 */


#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "aTunes.h"


#define carillonPin 5
#define DHTPIN  4
#define DHTTYPE        DHT11

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

#define LED2  12
#define LED3  13

#define LED5_R  9
#define LED5_G 10
#define LED5_B 11

#define S2 2
#define S3 3

#define READ_S2 ! digitalRead( S2 )
#define READ_S3 ! digitalRead( S3 )


void initDHT()
{
  dht.begin();

  sensor_t sensor;

  Serial.println( "DHTxx Unified Sensor Example" );
  // Print temperature sensor details.
  dht.temperature().getSensor( &sensor );
  Serial.println( F( "------------------------------------" ) );
  Serial.println( F( "Temperature" ) );
  Serial.print  ( F( "Sensor:       " ) ); Serial.println( sensor.name );
  Serial.print  ( F( "Driver Ver:   " ) ); Serial.println( sensor.version );
  Serial.print  ( F( "Unique ID:    " ) ); Serial.println( sensor.sensor_id );
  Serial.print  ( F( "Max Value:    " ) ); Serial.print  ( sensor.max_value);  Serial.println( F( " *C" ) );
  Serial.print  ( F( "Min Value:    " ) ); Serial.print  ( sensor.min_value);  Serial.println( F( " *C" ) );
  Serial.print  ( F( "Resolution:   " ) ); Serial.print  ( sensor.resolution); Serial.println( F( " *C" ) );
  Serial.println( "------------------------------------" );
  // Print humidity sensor details.
  dht.humidity().getSensor( &sensor );
  Serial.println( F( "------------------------------------" ) );
  Serial.println( F( "Humidity" ) );
  Serial.print  ( F( "Sensor:       " ) ); Serial.println( sensor.name );
  Serial.print  ( F( "Driver Ver:   " ) ); Serial.println( sensor.version );
  Serial.print  ( F( "Unique ID:    " ) ); Serial.println( sensor.sensor_id );
  Serial.print  ( F( "Max Value:    " ) ); Serial.print  ( sensor.max_value );  Serial.println( F( "\x25" ) ); // \x25 = signe %
  Serial.print  ( F( "Min Value:    " ) ); Serial.print  ( sensor.min_value );  Serial.println( F( "\x25" ) ); // \x25 = signe %
  Serial.print  ( F( "Resolution:   " ) ); Serial.print  ( sensor.resolution ); Serial.println( F( "\x25" ) ); // \x25 = signe %
  Serial.println( "------------------------------------" );


  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

void carillon()
{
  MarioBros( carillonPin );
  noTone( carillonPin );
  pinMode( carillonPin, INPUT_PULLUP );
}

void setup()
{
  Serial.begin( 115200 );
  Serial.print( "\n\nYWRobot - Easy Module Shield v1\n===============================" );

  pinMode( LED2,   OUTPUT );
  pinMode( LED3,   OUTPUT );
  pinMode( LED5_R, OUTPUT );
  pinMode( LED5_G, OUTPUT );
  pinMode( LED5_B, OUTPUT );
  pinMode( S2, INPUT );
  pinMode( S3, INPUT );

  digitalWrite( LED2,   HIGH );
  digitalWrite( LED3,   HIGH );
  digitalWrite( LED5_B, HIGH );
  delay( 500 );
  digitalWrite( LED2,   LOW );
  digitalWrite( LED3,   LOW );
  digitalWrite( LED5_B, LOW );
  delay( 500 );

  carillon();

  initDHT();
}


void loop()
{
  while( READ_S2 )
  {
    Serial.print( "." );
    delay( 10 );
    if( ! READ_S2 )
      Serial.print( "\n" );
  }
  while( READ_S3 )
  {
    Serial.print( "*" );
    delay( 10 );
    if( ! READ_S3 )
      Serial.print( "\n" );
  }

  getTempAndHum();
  delay( 100 );


  // Potentiomètre
  int A0val = analogRead( A0 );
  Serial.println( A0val );

  // luxmètre
  int A1val = analogRead( A1 );
  Serial.println( A1val );

  //
  int A2val = analogRead( A2 );
  Serial.println( A2val );

  //
  int A3val = analogRead( A3 );
  Serial.println( A3val );


  pinMode( carillonPin, OUTPUT );
  for (int i = 0; i < 10; ++i)
  {
    digitalWrite( carillonPin, HIGH );
    delay( 100 );
    digitalWrite( carillonPin, LOW );
    delay( 100 );
  }
}








void getTempAndHum()
{
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent( &event );
  if( isnan( event.temperature ) )
  {
    Serial.print( F( "NaN *C     " ) );
  }
  else
  {
    Serial.print( event.temperature, 1 );
    Serial.print( F( "*C     " ) );
  }

  // Get humidity event and print its value.
  // !! Si on demande la température (event.temperature) alors qu’on a
  // utilisé dht.humidity().getEvent( &event );, c’est la valeur de
  // l’humidité que l’on obtient...
  dht.humidity().getEvent( &event );
  if( isnan( event.relative_humidity ) )
  {
    Serial.print( F( "NaN \x25" ) );
  }
  else
  {
    Serial.print( event.relative_humidity, 1 );
    Serial.print( F( " \x25" ) ); // signe %
  }

  // Ajout séparateur de mesures Serial
  Serial.print( "\n" );
}