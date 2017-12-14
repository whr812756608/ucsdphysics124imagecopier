

#include <Stepper.h>
#define STEPS /*steps in one rotation*/
#define PI 3.141592653589793

Stepper beam(STEPS, /*pins*/);
Stepper dolly(STEPS, /*pins*/);
Stepper pin(STEPS, /*pins*/);

/*This assumed that all four steppers are the same, but they may not be.
 * It also assumed that we will use a stepper for the pin.  
 * I like using steppers way more than servos. Servos are a pain,
 * but a stepper may be too heavey for that place in the machine.
 */

const int FIELD_LENGTH=/*How many pixels high is the paper*/;
const int FIELD_WIDTH=/*How many pixels wide is the paper*/;
const int VERT_STEP=/*How many steps on a stepper motor moves down one pixel*/;
const int HORZ_STEP=/*How many steps on a stepper motor moves accross one pixel*/;
const int SENSE_MAX=/*The maximum sensor value*/;
const int SENSE_MIN=/*The minimum sensor value*/
const float PICKUP_THRESHOLD=/*how far the pen needs to move for it to be picked up*/;

volatile int sequence[FIELD_LENGTH*FIELD_WIDTH][2]; 

void setup() {
  // put your setup code here, to run once:
  stepper.setSpeed(/*probably pretty fast*/);
  attachInterrupt(/*pin*/, scan, RISING);
  attachInterrupt(/*pin*/, draw, RISING);
  /*We also may need to do some pin modes*/
}

void loop() {
  // put your main code here, to run repeatedly:
interrupts();
//That's it for now. There does not really need to be anything here,
//unless we want some idle stuff.
}

/*int findmax(int list[], int listsize){
  float maximum=list[0];
  int i;
  for(i=1; i<listsize; i++){
    maximum=max(maximum, list[i]);
  }
  return maximum; 
}*/

//I wrote the find max thing, but after reallizing there is a threshold
//function I do not think we will needd it.

void smooth(int image[][], float smooth[][]);
void sobel(float smooth[][], float phase[][], float magnitude[][]);
void thin(float magnitude[][],float phase[][]);
void threshold(float magnitude[][], boolean points[][]);
void prepare(boolean points[][], int sequence[][]);
void stepsequence(int sequence[][], int steps[][]); // this goes in the draw function 

void scan(int sequence[][]){
  int image[FIELD_LENGTH][FIELD_WIDTH];
  float smoother[FIELD_LENGTH][FIELD_WIDTH];
  float magnitude[FIELD_LENGTH][FIELD_WIDTH];
  float phase[FIELD_LENGTH][FIELD_WIDTH];
  boolean points[FIELD_LENGTH][FIELD_WIDTH];
  /*First I scann in the image.*/
  /*For this part, I will assume the the field length and width are even nubmers*/
  int i, j;
  for(i=0; i<FIELD_LENGTH-1; i+=2){
    for(j=0; j<FIELD_WIDTH; j++){
      /*read from the sensor and put the value in image[i][j]*/
      dolly.step(HORZ_STEP); //may have to change the sign.
    }
    /*read om thee sensor value and put it in image[i][FIELD_WIDTH]*/
    beam.step(VERT_STEP);
    for(j=FIELD_WIDTH-1; j>=0; j--){
      /*read from the sensor and put the value in image[i+1][j]*/
      dolly.step(-HORZ_STEP);// may have to change the sign.
    } 
    /*read sensor value and put it into image[i+1][0]*/
    beam.step(VERT_STEP);
  } 
  for(j=0; j<FIELD_WIDTH; j++){
    /*read from the sensor and put the value in image[i][j]*/
    dolly.step(HORZ_STEP); //may have to change the sign.
  }
    /*read om thee sensor value and put it in image[i][FIELD_WIDTH]*/
    beam.step(VERT_STEP);
  for(j=FIELD_WIDTH-1; j>=0; j--){
    /*read from the sensor and put the value in image[i+1][j]*/
    dolly.step(-HORZ_STEP);// may have to change the sign.
  } 
  /*read sensor value and put it into image[i+1][0]*/
  beam.step(-VERT_STEP*(FIELD_LENGTH));

  //SCAN COMPLETE .................................................................................
  smoother(int image[][], float smooth[][]);
  sobel(float smooth[][], float phase[][], float magnitude[][]);
  thin(float magnitude[][],float phase[][]);
  threshold(float magnitude[][], boolean points[][]);
  prepare(boolean points[][], int sequence[][]);    
}

void draw(int sequence[][]){
  int steps[FIELD_LENGTH*FIELD_WIDTH][3]
  stepsequence(int sequence[][], int steps[][]);
  //put pin down (DO LATER)
  int i=0;
  
  while(sequence[i][0]!=0 && sequence[i][1]!=0){
    if (steps[i][2]==1){
      //lift the pin (DO LATER)
      rack.step(VERT_STEP*steps[i][0]);
      dolly.step(HORZ_STEP*steps[i][1]);
      //put pin back down (DO LATER)
    }
    else {
      rack.step(VERT_STEP*steps[i][0]);
      dolly.step(HORZ_STEP*steps[i][1]);
    }
    i++;
  }
  //lift the pin (DO LATER)
  rack.step(-sequence[i-1][0]);
  dolly.step(-sequence[i-1][1]);
}

void smoother(int image[][], float smooth[][]);{
 float gauss[][]={{2.0/159.0, 4.0/159.0, 5.0/159.0, 4.0/159.0, 2.0/159.0},{4.0/159.0, 9.0/159.0, 12.0/159.0, 9.0/159.0, 4.0/159.0},{5.0/159.0, 12.0/159.0, 15.0/159.0, 12.0/159.0, 5.0/159.0},{4.0/159.0, 9.0/159.0, 12.0/159.0, 9.0/159.0, 4.0/159.0},{2.0/159.0, 4.0/159.0, 5.0/159.0, 4.0/159.0, 2.0/159.0}};
 float padded[FIELD_LENGTH+4][FEILD_WIDTH+4];
 int i;
 int j;
 for(i=0; i<FIELD_LENGTH+4; i++){
  for (j=0; j<FIELD_WIDTH+4; j++){
    padded[i][j]=0; //Note, you may want change the pad value. 
  }
 }
 for(i=0; i<FIELD_LENGTH; i++){
  for (j=0; j<FIELD_WIDTH; j++){
    padded[i+2][j+2]=image[i][j]; 
  }
 }
 int k;
 int l;
 float cumsum=0.0;
  for(i=0; i<FIELD_LENGTH; i++){
    for (j=0; j<FIELD_WIDTH; j++){
      for (k=i-2; k<i+3; k++){
        for(l=j-2; l<j+3; l++){
          cumsum+=image[k][l]*gauss[k+2][l+2];
        }
      }
      smooth[i][j]=cumsum;
      cumsum=0.0;
    }
  } 
}

void sobel(float smooth[][], float phase[][], float magnitude[][]) {
    
  float sobel_vert[][]={ {-1.0, 0.0, 1.0}, {-2.0, 0.0, 2.0},{-1.0, 0.0, 1.0}};
  float sobel_horz[][]={ {-1.0, -2.0, -1.0}, {0.0, 0.0, 0.0},{1.0, 2.0, 1.0}};
  float padded2[FIELD_LENGTH+2][FEILD_WIDTH+2];
  int i;
  int j;
   for(i=0; i<FIELD_LENGTH+2; i++){
    for (j=0; j<FIELD_WIDTH+2; j++){
     padded2[i][j]=0; //Note, you may want change the pad value. 
    }
   }
   for(i=0; i<FIELD_LENGTH; i++){
    for (j=0; j<FIELD_WIDTH; j++){
      padded2[i+1][j+1]=smooth[i][j]; 
    }
  }
  float edge_vert[FIELD_LENGTH][FIELD_WIDTH];
  float edge_horz[FIELD_LENGTH][FIELD_WIDTH];
  int k;
  int l;
  float vertcumsum=0.0;
    for(i=0; i<FIELD_LENGTH; i++){
      for (j=0; j<FIELD_WIDTH; j++){
        for (k=i-1; k<i+2; k++){
          for(l=j-1; l<j+2; l++){
           vertcumsum+=smooth[k][l]*sobel_vert[k+1][l+1];
          }
        }
      edge_vert[i][j]=vertcumsum;
      vertcumsum=0.0;
     }
    } 
  float horzcumsum=0.0;
    for(i=0; i<FIELD_LENGTH; i++){
      for (j=0; j<FIELD_WIDTH; j++){
        for (k=i-1; k<i+2; k++){
          for(l=j-1; l<j+2; l++){
           horzcumsum+=smooth[k][l]*sobel_horz[k+1][l+1];
          }
        }
      edge_horz[i][j]=horzcumsum;
      horzcumsum=0.0;
     }
    }
  for(i=0; i<FIELD_LENGTH; i++){
    for(j=0; j<FIELD_WIDTH; j++){
      magnitude[i][j]= sqrt(edge_vert[i][j]*edge_vert[i][j]+edge_horz[i][j]*edge_horz[i][j]);
      phase[i][j] = atan(edge_horz[i][j]/edge_vert[i][j]);
    }
  }
}

void thin(float magnitude[][],float phase[][]){
  for(i=0; i<FIELD_LENGTH; i++){
    for(j=0; j<FIELD_WIDTH; j++){
      if (phase[i][j]>3.0/8.0*PI || phase[i][j]<-3.0/8.0*PI){
        phase[i][j]=90.0;
      }
      else if(phase[i][j]<3.0/8.0*PI && phase[i][j]>1.0/8.0*PI){
        phase[i][j]=45.0;
      }
      else if(phase[i][j]<1.0/8.0*PI && phase[i][j]>-1.0/8.0*PI){
        phase[i][j]=0.0;
      }
      else if(phase[i][j]<-1.0/8.0*PI && phase[i][j]>-3.0/8.0*PI){
        pahase[i][j]=-45.0;
      }
    } 
  }

  for(i=1; i<FIELD_LENGTH-1; i++){
    for(j=1; j<FIELD_WIDTH-1; j++){
     if (phase[i][j] ==0.0){
      if (magnitude[i][j] < max(magnitude[i][j-1],magnitude[i][j+1])){
         magnitude[i][j] = 0.0;
      }
     }
     else if (phase[i][j] ==-45.0){
       if (magnitude[i][j] < max(magnitude[i+1][j-1],magnitude[i-1][j+1])){
         magnitude[i][j] = 0.0;
      }
     }
     else if (phase[i][j] ==45.0){
       if (magnitude[i][j] < max(magnitude[i+1][j+1],magnitude[i-1][j-1])){
         magnitude[i][j] = 0.0;
      }
     }
     else if (phase[i][j] ==90.0){
       if (magnitude[i][j] < max(magnitude[i-1][j],magnitude[i+1][j])){
         magnitude[i][j] = 0.0;
      }
     }
    }
  }  
}

void threshold(float magnitude[][], boolean points[][]){
 float maximum = 0.0;
  for(i=1; i<FIELD_LENGTH-1; i++){
    for(j=1; j<FIELD_WIDTH-1; j++){
      maximum = max(maximum, magnitude[i][j]);
    }
  } 
  float threshold = 0.12*maximum //may change number as needed
  for(i=1; i<FIELD_LENGTH-1; i++){
    for(j=1; j<FIELD_WIDTH-1; j++){
      if( magnitude[i][j] > threshold) {
        points[i][j]= true;
      }
      else {
        point[i][j]= false; 
      }
    }
  }
}


