#include <MatrixMathInt.h> // https://github.com/ceritium/MatrixMathInt
#include "LedControl.h"

/*
 Now we need a LedControl to work with.
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 But the maximum default of 8 MAX72XX wil also work.
 */

#define SCREENS (2)
#define SCREEN_SIZE (8)
#define COLS (8)
#define ROWS (16)

LedControl lc=LedControl(12,11,10,SCREENS);

/* we always wait a bit between updates of the display */
unsigned long delaytime=100;


int world[ROWS][COLS] = {};
int newWorld[ROWS][COLS] = {};
int randNumber;
 
void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  //we have already set the number of devices when we created the LedControl
  int devices=lc.getDeviceCount();
  //we have to init all devices in a loop
  for(int address=0;address<devices;address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(address,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(address,1);
    /* and clear the display */
    lc.clearDisplay(address);
  }

  for(int row=0;row<ROWS;row++) {
    for(int col=0;col<COLS;col++) {
        int value = live(row, col);
        randNumber = random(7) == 1 ? 1 : 0;
        world[row][col] = randNumber;
    }
  }

  render(world);

  delay(delaytime*10);

  
}

void loop() { 
  for(int row=0;row<ROWS;row++) {
    for(int col=0;col<COLS;col++) {
        int value = live(row, col);
        newWorld[row][col] = value;
    }
  }

  Matrix.Copy((int*)newWorld, ROWS, COLS,(int*)world);
  render(world);

  delay(delaytime);


}

bool live(int row, int col){
  int count = neighbours(row, col);
  if(world[row][col] == 1) {
    return (3 >= count) && (count >= 2);  
  } else {
    return count == 3;
  }
  
}

int neighbours(int row, int col){
  int count = 0;

  count = count + world[nrow(row-1)][ncol(col-1)];
  count = count + world[nrow(row-1)][ncol(col)];
  count = count + world[nrow(row-1)][ncol(col+1)];
  count = count + world[nrow(row)][ncol(col-1)];
  count = count + world[nrow(row)][ncol(col+1)];
  count = count + world[nrow(row+1)][ncol(col-1)];
  count = count + world[nrow(row+1)][ncol(col)];
  count = count + world[nrow(row+1)][ncol(col+1)];

  return count;
}

int nrow(int x){
  if (x < 0){
    return ROWS-1;
  }

  if (x > ROWS-1){
    return 0;
  }
  return x;
}

int ncol(int x){
  if (x < 0){
    return COLS-1;
  }

  if (x > COLS-1){
    return 0;
  }
  
  return x;
}

void render(int matrix[ROWS][COLS]){
  int row = 0;
        
  for(int screen=0;screen<SCREENS;screen++) {

    int matrox[SCREEN_SIZE][SCREEN_SIZE] = {};    
    for(int srow=0;srow<SCREEN_SIZE;srow++) {
      for(int col=0;col<COLS;col++) {
        matrox[srow][col] = matrix[row][col];
      }
      row++;
    }
  
    int matrex[SCREEN_SIZE][SCREEN_SIZE] = {};    
    Matrix.Transpose((int*)matrox,SCREEN_SIZE, SCREEN_SIZE, (int*)matrex);
    for(int srow=0;srow<SCREEN_SIZE;srow++) {
      for(int col=0;col<COLS;col++) {
        lc.setLed(screen,srow,SCREEN_SIZE-col-1,matrex[srow][col]);
      }
    }
  }
}

