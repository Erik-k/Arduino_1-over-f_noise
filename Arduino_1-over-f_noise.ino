/* One_Over_F_Random_Numbers
  Generates 1/f random notes and plays them on speaker

  Transistor: BC547 or any general purpose transistor
  Speaker: 8 ohm, .5 watt 
  Circuit: 
  Pin 12, 1k resistor to transistor base
  Transistor collector, speaker, 100 ohm, +5v
  Changing 100 ohm resistor will change volumn
  Note about the random function:
  random(min, max) min is inclusive, max is exclusive
  This means if you want 4 random numbers, max must be 5
*/

// Note_table contains note designations and frequencies
// It also contains a stop note (ST = 0)
#include "note_table.h"

// -----------------------------------------------------------
// Set up variables
const int pinSpeaker = 12;  //Set up on PWM pin
const int buttonPin = 2;   // for selecting type of music
int i = 0;  // index
int j = 0;  // index
int duration = 0;  // The length of the note
int freq = 0;  // The note frequency
int rest = 0;  // The rest, or no note, in music terminology

// Need to set long on these values, int doesn't work
unsigned long oneSec = 1000;  // 1 sec = 1000 msec
unsigned long oneMin = 60000;  // 1 min = 60000 msec

// Calculating note lengths
// 60000 ms(1 min)/Tempo(BPM)=Delay Time in ms for quarter-note beats
// 60000/120 = 500 ms
int T = 120;  // Tempo, change this as you wish
int Q = oneMin / T; // Quarter note in ms

// Other note designations
int H = Q * 2;  // Half
int W = H * 2;  // Whole
int E = Q / 2;  // Eighth
int S = E / 2;  // Sixteenth

int Row = 3;   // Number of columns to change

// Places to store random numbers
int Rnd0 = 0;
int Rnd1 = 0;
int Rnd2 = 0;
int Rnd3 = 0;
int Rnd4 = 0;
int Rnd5 = 0;
int Rnd6 = 0;
int Rnd7 = 0;
int NumColsToChangeN[] = {3,1,2,1,3,1,2,1};
int NumColsToChangeL[] = {2,1,2,1,2,1,2,1};
// Rest durations
int restDurations[] = {S,E,Q,H,W};

// -----------------------------------------------------------
// Notes in a sequence. These are for playing with.
// Note: A sequence can be notes as scale groups, or next to each other in a row

// Notes from C3 to A4, like on a piano. The 1's at end are for rests
// Notes are picked from 3 to 18
int sequence0[] = 
  {1,1,1, NC3,ND3,NE3,NF3,NG3,NA3,NB3,NC4,ND4,NE4,NF4,NG4,NA4, 1,1,1};

// Notes in the pentatonic scale: C, D, F, G, A
int sequence1[] = 
  {1,1,1, NC4,ND4,NF4,NG4,NA4, NC4,ND4,NF4,NG4,NA4, NC4,ND4,NF4, 1,1,1};
int sequence2[] = 
  {1,1,1, NC4,NC4, ND4,ND4, NF4,NF4, NG4,NG4, NA4,NA4, NC4,NC4, ND4, 1,1,1};

// Notes using the black keys
int sequence3[] = 
  {1,1,1, NC4S,ND4S,NF4S,NG4S,NA4S, NC4S,ND4S,NF4S,NG4S,NA4S, NC4S,ND4S,NF4S, 1,1,1};

//  A Major scale  A, B, C♯, D, E, F♯, G♯
int sequence4[] = 
  {1,1,1,NA4,NB4,NC4S,ND4,NE4,NF4S,NG4S, NA4,NB4,NC4S,ND4,NE4,NF4S,NG4S, 1,1};
int sequence5[] = 
  {1,1,1, NA4,NA4, NB4,NB4, NC4S,NC4S, ND4,ND4, NE4,NE4, NF4S,NF4S, NG4S,NG4S, 1,1};
  
// Indian Raga Kalyan scale C, D, E, F#, G, A B (there are others as well)
int sequence6[] = 
  {1,1,1, NC4,NC4, ND4,ND4, NE4,NE4, NF4S,NF4S, NG4,NG4, NB4,NB4, NC4,NC4, 1,1};

//  Debussy whole tone scale C, D, E, F#, G#, B A#
int sequence7[] = 
 {1,1,1, NC4,NC4, ND4,ND4, NE4,NE4, NF4S,NF4S, NG4S,NG4S, NB4,NB4, NA4S,NA4S, 1,1};

//  Close Encounters' notes
int sequence8[] = 
  {1,1,1, NC4,NC4, NG4,NG4, NC4,NC4, ND4,ND4, NE4,NE4, NC4,NC4, NG4,NG4, 1,1};

int* sequences[9]; // the array of arrays which allows us to increment through 
                  // each song type
int sequence_number = 0;
// -----------------------------------------------------------
// Note: sizeof returns the number of bytes in an array or variable
//int lenSeq = (sizeof(sequence)/2);  // Length of sequence (19)

// Note durations:
// All Quarter notes
//int noteDurations[] = {Q,Q,Q,Q,Q,Q,Q,Q,Q,Q,Q,Q,Q};
// Using only Sixteenth, Eight, and Quarter notes
int noteDurations[] = {1,1,S,E,Q,S,E,Q,S,E,Q,S,E};  // 2 to 12

// -----------------------------------------------------------
void setup() {
  sequences[0] = sequence0;
  sequences[1] = sequence1;
  sequences[2] = sequence2;
  sequences[3] = sequence3;
  sequences[4] = sequence4;
  sequences[5] = sequence5;
  sequences[6] = sequence6;
  sequences[7] = sequence7;
  sequences[8] = sequence8;
  
  pinMode(pinSpeaker,OUTPUT);
  pinMode(buttonPin, INPUT); // pin 2 receives the button press signal 
  attachInterrupt(digitalPinToInterrupt(buttonPin), receiveButton, RISING);
  Serial.begin(9600);
  // Get random sequence based on noise from analog pin 0
  randomSeed(analogRead(0));
  //Serial.println("Length of sequence: ");
  //Serial.println(lenSeq);
}

// -----------------------------------------------------------
void loop() {
  Serial.println("Start of sequence");
  
  // Play 8 random notes
  for (i=0; i<8; i++) {
    Row = NumColsToChangeN[i];
    switch (Row){
      case 3:  // Throw 3 dice
        Rnd1 = random(1, 7);  // 1 to 6 dots
        Rnd2 = random(1, 7);
        Rnd3 = random(1, 7);
      case 2:  // Throw 2 dice
        Rnd1 = random(1, 7);
        Rnd2 = random(1, 7);
      case 1:  // Throw 1 di
        Rnd1 = random(1, 7);
     }
    Rnd0 = Rnd1 + Rnd2 + Rnd3; // Min 3 (1+1+1), Max 18 (6+6+6)
    
    // Get note length
    Row = NumColsToChangeL[i];
    switch (Row){
      case 2:  // Throw 2 dice
        Rnd4 = random(1, 7);
        Rnd5 = random(1, 7);
      case 1:  // Throw 1 di
        Rnd4 = random(1, 7);
     }
    Rnd6 = Rnd4 + Rnd5; // Min 2 (1+1), Max 12 (6+6)
    
    // Get a rest length
    Rnd7 = random(0, 3);  // Get a random # from 0 to 2 (S,E,Q)
    rest = restDurations[Rnd7];
    
    // Play the note or rest
    duration = noteDurations[Rnd6];
    freq = sequences[sequence_number][Rnd0];
    // This plays a rest
    if (freq == 1) {
      playRest(duration,rest);
    }
    if (freq > 1) {
      playTone(duration,freq);
    }
    
    //ShowIt();  // For testing
  }
  Serial.println("End of sequence");
  Serial.println("-----------------");
  //delay(250);  // For testing
  
}

// -----------------------------------------------------------
// Functions
// Duration in mSecs, Frequency in hertz

void receiveButton() {
  // Need to debounce in software as well as use the 10uF cap across the 
  // switch terminals. Since this button press will change "songs", a 
  // noticeable delay is ok. 
  delayMicroseconds(40000); //cannot use millis or any other delay() function
  Serial.println("BUTTON PRESSED");
  sequence_number++;
  if (sequence_number >8){
    sequence_number = 0;
  }
  Serial.print("sequence_number is now ");
  Serial.print(sequence_number);
}

void playTone(long Duration, int Frequency) {
  Duration *= 1000;
  int period = (1.0 / Frequency) * 1000000;
  long elapsed_time = 0;
  while (elapsed_time < Duration) {
    digitalWrite(pinSpeaker,HIGH);
    delayMicroseconds(period / 2);
    digitalWrite(pinSpeaker,LOW);
    delayMicroseconds(period / 2);
    elapsed_time += (period);
  }
}

//-----------------------------------------
// Duration in mSecs, Frequency in hertz
void playRest(long Duration, int Frequency) {
  Duration *= 1000;
  int period = (1.0 / Frequency) * 1000000;
  long elapsed_time = 0;
  while (elapsed_time < Duration) {
    //digitalWrite(pinSpeaker,HIGH);
    delayMicroseconds(period / 2);
    //digitalWrite(pinSpeaker,LOW);
    delayMicroseconds(period / 2);
    elapsed_time += (period);
  }
}

//-----------------------------------------
// For testing, show the variables
void ShowIt(){
  Serial.println("-----------------");
  Serial.print("Count: ");
  Serial.println(i+1);
  //Serial.print("Index j: ");
  //Serial.println(j);
  /*
  Serial.print("Row: ");
  Serial.println(Row);
  Serial.print("Rnd1: ");
  Serial.println(Rnd1);
  Serial.print("Rnd2: ");
  Serial.println(Rnd2);
  Serial.print("Rnd3: ");
  Serial.println(Rnd3);
  Serial.print("Rnd4: ");
  Serial.println(Rnd4);
  Serial.print("Rnd5: ");
  Serial.println(Rnd5);
  */
  
  Serial.print("Len Rnd6: ");
  Serial.println(Rnd6);
  Serial.print("Duration: ");
  Serial.println(duration);
  Serial.print("Note Rnd0: ");
  Serial.println(Rnd0);
  Serial.print("Rest Rnd7: ");
  Serial.println(Rnd7);
  Serial.print("Rest: ");
  Serial.println(rest);
  Serial.print("Freq: ");
  Serial.println(freq);
  //delay(1000);  // For testing
}
