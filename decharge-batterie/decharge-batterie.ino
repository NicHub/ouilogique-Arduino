/*

 */

int batPin = A0;
int ledPin = 13;
int batVolt = 0;

void setup()
{
  pinMode( ledPin, OUTPUT );
  Serial.begin( 115200 );
}

void loop()
{

  batVolt = analogRead( batPin );
  // batVolt = map( sensorValue, 0, 1023, 0, 5 );
  Serial.println( batVolt );

  digitalWrite( ledPin, HIGH );
  _delay_ms( 100 );
  digitalWrite( ledPin, LOW );
  _delay_ms( 10000 );

}