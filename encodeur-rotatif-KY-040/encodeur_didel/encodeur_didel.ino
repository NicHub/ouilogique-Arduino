
//TestEncoME.ino| Enco câblé sur pins 8,9 Moteur 4,5
//Soft dérivé de kidule mach état
#define IN0 digitalRead (8)
#define IN1 digitalRead (9)
byte olda, oldb, newab ;  // portB 0b000000AB
void setup() {  
  DDRB = 0x00110000 ; // 11..8 in 13 12 out
  DDRC = 0xFF ; // affichage 6 bits
  PORTC = 0xff ;
  DDRD = 0xF0  ; //moteurs
  PORTD = 0x0C0 ; 
  analogWrite (5,250) ;
}
enum {e0,e1,e2,e3} next = e0;
int pos=0 ;
void loop() {
  switch(next)
  {
  case e0: 
    if (IN0) {pos++; next = e1; break;}
    if (IN1) {pos--; next = e3; break;}
    break;
  case e1: 
    if (IN1) {pos++; next = e2; break;}
    if (!IN0) {pos--; next = e0; break;}
    break;
  case e2: 
    if (!IN0) {pos++; next = e3; break;}
    if (!IN1) {pos--; next = e1; break;}
    break;
  case e3:
    if (!IN1) {pos++; next = e0; break;}
    if (IN0)  {pos--; next = e2; break;}
    break; 
  }
  PORTC = pos ;
  delay (1); 
}



