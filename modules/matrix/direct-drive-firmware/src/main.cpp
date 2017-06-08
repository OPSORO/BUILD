/*
 * Show messages on an 8x8 led matrix,
 * scrolling from right to left.
 *
 * Uses FrequencyTimer2 library to
 * constantly run an interrupt routine
 * at a specified frequency. This
 * refreshes the display without the
 * main loop having to do anything.
 *
 */
#include <Arduino.h>
#include <SerialCommand.h>

/*
col 0  |  col 2
row 4  |  col 5
row 2  |  row 3
col 3  |  col 7
row 7  |  row 5
col 1  |  row 6
row 1  |  col 6
row 0  |  col 4

*/

// pin[xx] on led matrix connected to nn on Arduino (-1 is dummy to make array start at pos 1)
//int pins[17]= {-1, 5, 4, 3, 2, 14, 15, 16, 17, 13, 12, 11, 10, 9, 8, 7, 6};

// col[xx] of leds = pin yy on led matrix
//int cols[8] = {pins[13], pins[3], pins[4], pins[10], pins[06], pins[11], pins[15], pins[16]};


// row[xx] of leds = pin yy on led matrix
//int rows[8] = {pins[9], pins[14], pins[8], pins[12], pins[1], pins[7], pins[2], pins[5]};

// int rows[8] = {A0, 16, A2, 10,  5, A3, 7,  2};
// int cols[8] = { 9,  8,  4, A1,  3, 15, 14, 6};

SerialCommand sCmd;

int rows[8] = {2, 7, A3, 5, 10, A2, 16, A0};
int cols[8] = {6, 14, 15, 3, A1, 4, 8, 9};
uint8_t col = 0;

uint8_t buffer[8] = {
  0B00000000,
  0B01100110,
  0B01100110,
  0B00000000,
  0B10000001,
  0B01000010,
  0B00111100,
  0B00000000

  // 0xFF,0xFF,
  // 0xFF,0xFF,
  // 0xFF,0xFF,
  // 0xFF,0xFF

  // 0, 0,
  // 0, 0,
  // 0, 0,
  // 0, 0
};


void setup();
void loop();
void display();

void cmd_set();
void cmd_identify();

void setup() {
  // set up cols and rows
  for (int i = 0; i < 8; i++) {
    pinMode(cols[i], OUTPUT);
    digitalWrite(cols[i - 1], LOW);
  }

  for (int i = 0; i < 8; i++) {
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], LOW);
  }

  Serial.begin(9600);
  sCmd.addCommand("identify", cmd_identify);
  sCmd.addCommand("set",      cmd_set);

  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

SIGNAL(TIMER0_COMPA_vect){
  display();
}

void loop(){
  sCmd.readSerial();
}

void cmd_set(){
  char *arg = sCmd.next();
  char buff[3];
  uint8_t pos = 0;

  if(arg != NULL){
    while(strlen(arg) >= 2){
      buff[0] = arg[0];
      buff[1] = arg[1];
      buff[2] = 0;

      long raw_data = strtol(buff, NULL, 16);
      buffer[pos % 8] = (uint8_t)raw_data;

      pos++;
      arg+=2;
    }
  }else{
    for(int i = 0; i < 8; i++){
      buffer[i] = 0x00;
    }
  }
}

void cmd_identify(){
  Serial.println("Opsoro Led Matrix v0.2");
}

// Interrupt routine
void display() {
  // digitalWrite(cols[col], LOW);  // Turn whole previous column off
  // col++;
  // if (col == 8) {
  //   col = 0;
  // }
  // for (int row = 0; row < 8; row++) {
  //   if (leds[col][7 - row] == 1) {
  //     digitalWrite(rows[row], LOW);  // Turn on this led
  //   }
  //   else {
  //     digitalWrite(rows[row], HIGH); // Turn off this led
  //   }
  // }
  // digitalWrite(cols[col], HIGH); // Turn whole column on at once (for equal lighting times)

  // Turn whole previous column off
  digitalWrite(cols[col], LOW);

  col++;
  col = col % 8;

  for(uint8_t row = 0; row < 8; row++){
    if( buffer[col] & _BV(row) ){
      // Turn on this led
      digitalWrite(rows[row], LOW);
    }else{
      // Turn off this led
      digitalWrite(rows[row], HIGH);
    }
  }

  // Turn whole column on at once (for equal lighting times)
  digitalWrite(cols[col], HIGH);
}
