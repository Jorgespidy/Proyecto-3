/* 
 Educational BoosterPack MK II - Birthday Tune
http://boosterpackdepot.info/wiki/index.php?title=Educational_BoosterPack_MK_II
 
 Play birthday tune through the buzzer, demonstrating
 buzzer tune() API and pitch/tone (hence music) generation
 
 Dec 2012 - Created for Educational BoosterPack
            buzzer Pin = 19
 Dec 2013 - Modified for Educational BoosterPack MK II
            buzzer Pin = 40
 by Dung Dang
 
 */
#include "pitches.h"
#define NOTE_C4_1 260
#define VACIO 1


//----------Definicion de Variables---------------

// estado de botones
int state1 = 0;
int buzzerPin = 37;

//bandera
int flag = 1;

//  PARA CANCION INICIAL
int melody1[] = {
  B5, CS5, E5, FS5, E5, DS5, CS5, B5, B5, VACIO,
  DS4, FS4, DS4, CS5, DS4, B5, DS4, FS4, DS4, CS5, DS4, B5, DS4, FS4, DS4, VACIO,
  DS4, E4, FS4, B5, AS5, B5, FS4, DS4, B4, CS4, DS4, CS4,
  DS3, DS3, E3, E3, E3, DS4, E4,
  VACIO, DS4, E4, FS4, B5, CS5, B5, 
  B5, CS5, E5, DS5, CS5, B5, FS4, GS4, CS5, CS5,
  CS5, DS5, E5, DS5, CS5, B5,
  FS4, CS4, CS4, CS4, CS4, CS4,
  CS5, DS5, E5, DS5, CS5, B5, FS5};

int noteDurations1[] = {
  8, 8, 4, 16, 16, 4, 4, 2, 2, 4,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 8,
  2, 2, 4, 4, 8, 4, 4, 4, 4, 4, 4, 1,
  2, 2, 8, 8, 8, 2, 4,
  8, 16, 8, 8, 8, 8, 1, 
  8, 8, 4, 4, 8, 4, 8, 4, 8, 1,
  8, 8, 2, 4, 8, 4,
  4, 2, 2, 8, 2, 8, 
  8, 8, 4, 4, 8, 4, 1};


//  CANCION DEL MENU PRINCIPAL
int melody2[] = { 
   FS3, VACIO, A4, CS4, VACIO, A4, VACIO, FS3, D3, D3, D3, VACIO,
   CS3, D3, FS3, A4, CS4, VACIO, A4, VACIO, FS3, E4, DS4, C4, VACIO,
   GS3, VACIO, CS4, FS3, VACIO, CS4, VACIO, GS3, VACIO, CS4, VACIO, G3, FS3, VACIO, E3, VACIO, E3, E3, E3,
   VACIO, E3, E3, E3, VACIO, DS3, D3, CS3, VACIO, A4, CS4, VACIO, A4, VACIO, FS3, E3, E3, E3,
   VACIO, E4, E4, E4, VACIO, B2, FS3, A4, CS4, VACIO, A4, VACIO, FS3, CS4, B4};
   
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations2[] = {
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 4, 4, 1,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  1, 4, 4, 4, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 4};


void setup() 
{
pinMode(buzzerPin,OUTPUT);
pinMode(PUSH1, INPUT_PULLUP);

// -------------- TOCA LA CANCION INICIAL UNA VEZ -----------------------
for (int thisNote = 0; thisNote < 80; thisNote++) {

    int noteDuration = 1000/noteDurations1[thisNote];
    if (melody1[thisNote] == 1){}
    else{
    tone(buzzerPin, melody1[thisNote],noteDuration);
    }
   
    int pauseBetweenNotes = noteDuration *1.3;      //delay between pulse
    delay(pauseBetweenNotes);
    
    noTone(buzzerPin);
}

}
void loop() 
{
  
  if (flag == 1){
  for (int thisNote = 0; thisNote < 77; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations2[thisNote];
    if (melody2[thisNote] == 1){}
    else{
    tone(buzzerPin, melody2[thisNote],noteDuration);
    }
    int pauseBetweenNotes = noteDuration *1.1;      //delay between pulse
    delay(pauseBetweenNotes);
    
    noTone(buzzerPin);                // stop the tone playing
    state1 = digitalRead(PUSH1);
    if (state1 == LOW){
      flag = 0;
      break;
    }
  }
  }
}
