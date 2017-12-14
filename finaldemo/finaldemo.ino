#include <Wire.h>
#include <AFMotor.h>
#include <Servo.h>
#include <SparkFun_APDS9960.h>
#define STEPS 1/*steps in one rotation*/


SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint16_t ambient_light = 0;

AF_Stepper beam(400, 2);
AF_Stepper dolly(400, 1);
Servo pin;



/*This assumed that all four steppers are the same, but they may not be.
 * It also assumed that we will use a stepper for the pin.  
 * I like using steppers way more than servos. Servos are a pain,
 * but a stepper may be too heavey for that place in the machine.
 */

const int FIELD_LENGTH=15/*How many pixels high is the paper*/;
const int FIELD_WIDTH=15/*How many pixels wide is the paper*/;
const int VERT_STEP=30/*How many steps on a stepper motor movess down one pixel*/;
const int HORZ_STEP=52/*How many steps on a stepper motor movess accross one pixel*/;
const int SENSE_MAX=255/*The maximum sensor value*/;
const int SENSE_MIN=0/*The minimum sensor value*/;
const float PICKUP_THRESHOLD=1/*how far the pen needs to moves for it to be picked up*/;  
const int THRESHOLD=150; /*we shall tune this as needed*/
const int UPANGLE=10;
const int DOWNANGLE=0;

volatile boolean points[FIELD_LENGTH][FIELD_WIDTH]; 
void smoother(int image[][FIELD_WIDTH], float smooth[][FIELD_WIDTH]);
void sobel(float smooth[][FIELD_WIDTH], float phase[][FIELD_WIDTH], float magnitude[][FIELD_WIDTH]);
void thiner(float magnitude[][FIELD_WIDTH],float phase[][FIELD_WIDTH], float thin[][FIELD_WIDTH]);
void threshold(float magnitude[][FIELD_WIDTH], bool points[][FIELD_WIDTH]);
void prepare(bool points[][FIELD_WIDTH], int sequence[][2]);
void movesr(int sequence [][2], int moves[][3]);
int image[FIELD_LENGTH][FIELD_WIDTH];
float smooth[FIELD_LENGTH][FIELD_WIDTH];
float magnitude[FIELD_LENGTH][FIELD_WIDTH];
float phase[FIELD_LENGTH][FIELD_WIDTH];
float thin[FIELD_LENGTH][FIELD_WIDTH];
volatile int sequence[FIELD_LENGTH*FIELD_WIDTH][2]; 


void setup() {
  // put your setup code here, to run once:
//  stepper.setSpeed(/*probably pretty fast*/);
  //attachInterrupt(/*pin*/, scan, RISING);
  //attachInterrupt(/*pin*/, draw, RISING);
  Serial.begin(9600);
  apds.init();
  apds.enableLightSensor(false);
  beam.setSpeed(10);
  dolly.setSpeed(10);
  
  pin.attach(22, 450, 2500); //***ADD PULSE TIMES!***
  /*We also may need to do some pin modes*/
}

void loop() {
  scan(points);
  smoother(image, smooth);
  sobel(smooth, phase, magnitude);
  thiner(magnitude , phase, thin);
  threshold(thin, points);
  prepare(points,sequence);  
  draw(points);
  // put your main code here, to run repeatedly:
//interrupts();
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


void scan(volatile boolean points[][FIELD_WIDTH]){

  //bool points[FIELD_LENGTH][FIELD_WIDTH];
  
  /*First I scann in the points.*/
  /*For this part, I will assume the the field length and width are even nubmers*/
  int i, j;
  for(i=0; i<FIELD_LENGTH-1; i+=2){
    
    for(j=0; j<FIELD_WIDTH; j++){
      apds.readAmbientLight(ambient_light);
      Serial.println(ambient_light);
      if (ambient_light<THRESHOLD){
         points[i][j]=1;
      }
      else{
         points[i][j]=0;
      }
      //points[i][j]=(int) ambient_light;
     Serial.println(points[i][j]);
      dolly.step(HORZ_STEP,FORWARD,DOUBLE);; //may have to change the sign.
    }
    apds.readAmbientLight(ambient_light);
    Serial.println(ambient_light);
      if (ambient_light<THRESHOLD){
         points[i][FIELD_WIDTH]=1;
      }
      else{
         points[i][FIELD_WIDTH]=0;
      }
    //points[i][FIELD_WIDTH]=(int) ambient_light;
    /*read from the sensor value and put it in points[i][FIELD_WIDTH]*/
    beam.step(VERT_STEP,FORWARD,DOUBLE);
    for(j=FIELD_WIDTH-1; j>=0; j--){
      apds.readAmbientLight(ambient_light);
      Serial.println(ambient_light);
      if (ambient_light<THRESHOLD){
         points[i+1][j]=1;
      }
      else{
         points[i+1][j]=0;
      }
      //points[i+1][j]=(int) ambient_light;
      Serial.println(points[i][j]);
      /*read from the sensor and put the value in points[i+1][j]*/
      dolly.step(HORZ_STEP,BACKWARD,DOUBLE);// may have to change the sign.
    } 
    apds.readAmbientLight(ambient_light);
    Serial.println(ambient_light);
      if (ambient_light<THRESHOLD){
         points[i+1][0]=1;
      }
      else{
         points[i+1][0]=0;
      }
    //points[i+1][0]=(int) ambient_light;
    /*read sensor value and put it into points[i+1][0]*/
    beam.step(VERT_STEP,FORWARD,DOUBLE);
  } 
  for(j=0; j<FIELD_WIDTH; j++){
    apds.readAmbientLight(ambient_light);
    //Serial.println(ambient_light);
      if (ambient_light<THRESHOLD){
         points[FIELD_LENGTH-1][j]=1;
      }
      else{
         points[FIELD_LENGTH-1][j]=0;
      }
    //points[FIELD_LENGTH-1][j]=(int) ambient_light;
    Serial.println(points[i][j]);
    /*read from the sensor and put the value in points[i][j]*/
    dolly.step(HORZ_STEP,FORWARD,DOUBLE); //may have to change the sign.
  }
  apds.readAmbientLight(ambient_light);
  Serial.println(ambient_light);
      if (ambient_light<THRESHOLD){
         points[FIELD_LENGTH-1][FIELD_WIDTH]=1;
      }
      else{
         points[FIELD_LENGTH-1][FIELD_WIDTH]=0;
      }
  //points[FIELD_LENGTH-1][FIELD_WIDTH]=(int) ambient_light;
    /*read om thee sensor value and put it in points[i][FIELD_WIDTH]*/
  beam.step(VERT_STEP,FORWARD,DOUBLE);
  for(j=FIELD_WIDTH-1; j>=0; j--){
    apds.readAmbientLight(ambient_light);
    Serial.println(ambient_light);
      if (ambient_light<THRESHOLD){
         points[FIELD_LENGTH][j]=1;
      }
      else{
         points[FIELD_LENGTH][j]=0;
      }

    //points[FIELD_LENGTH][j]=(int) ambient_light;
    Serial.println(points[FIELD_LENGTH][j]);
    /*read from the sensor and put the value in points[i+1][j]*/
   dolly.step(HORZ_STEP,BACKWARD,DOUBLE);// may have to change the sign.
  }
  apds.readAmbientLight(ambient_light);
  Serial.println(ambient_light);
      if (ambient_light<THRESHOLD){
         points[FIELD_LENGTH][0]=1;
      }
      else{
         points[FIELD_LENGTH][0]=0;
      }
 
  /*read sensor value and put it into points[i+1][0]*/
  //points[FIELD_LENGTH][0]=(int) ambient_light;
  beam.step(VERT_STEP*(FIELD_LENGTH),BACKWARD,DOUBLE);

  //SCAN COMPLETE ................................................................................. 
}

 void draw(volatile boolean points[][FIELD_WIDTH]){
 int i, j;
  for(i=0; i<FIELD_LENGTH-1; i+=2){
    for(j=0; j<FIELD_WIDTH; j++){
      if (points[i][j]==1){
        Serial.println("I hit a 1");
         pin.write(DOWNANGLE);
         //delayMicroseconds(10000);
         delay(500);
         pin.write(UPANGLE);
         delay(500);
      }
      dolly.step(HORZ_STEP,FORWARD,DOUBLE);; //may have to change the sign.
    }
      if ( points[i][FIELD_WIDTH]==1){
        Serial.println("I hit a 1");
         pin.write(DOWNANGLE);
         //delayMicroseconds(10000);
         delay(500);
         pin.write(UPANGLE);
         delay(500);
      }
    beam.step(VERT_STEP,FORWARD,DOUBLE);
    for(j=FIELD_WIDTH-1; j>=0; j--){
      if (points[i+1][j]==1){
        Serial.println("I hit a 1");
         pin.write(DOWNANGLE);
         //delayMicroseconds(10000);
         delay(500);
         pin.write(UPANGLE);
         delay(500);
      }
      dolly.step(HORZ_STEP,BACKWARD,DOUBLE);// may have to change the sign.
    } 
      if (points[i+1][0]==1){
        Serial.println("I hit a 1");
         pin.write(DOWNANGLE);
         //delayMicroseconds(10000);
         delay(500);
         pin.write(UPANGLE);
         delay(500);
      }
    beam.step(VERT_STEP,FORWARD,DOUBLE);
  } 
  for(j=0; j<FIELD_WIDTH; j++){
      if (points[FIELD_LENGTH-1][j]==1){
         pin.write(DOWNANGLE);
         //delayMicroseconds(10000);
         delay(500);
         pin.write(UPANGLE);
         delay(500);
      }
    dolly.step(HORZ_STEP,FORWARD,DOUBLE); //may have to change the sign.
  }
      if (points[FIELD_LENGTH-1][FIELD_WIDTH]==1){
         pin.write(DOWNANGLE);
         //delayMicroseconds(10000);
         delay(500);
         pin.write(UPANGLE);
         delay(500);
      }
  beam.step(VERT_STEP,FORWARD,DOUBLE);
  for(j=FIELD_WIDTH-1; j>=0; j--){
      if (points[FIELD_LENGTH][j]==1){
         pin.write(DOWNANGLE);
         //delayMicroseconds(10000);
         delay(500);
         pin.write(UPANGLE);
         delay(500);
      }
   dolly.step(HORZ_STEP,BACKWARD,DOUBLE);// may have to change the sign.
  }
      if (points[FIELD_LENGTH][0]==1){
         pin.write(DOWNANGLE);
         //delayMicroseconds(10000);
         delay(500);
         pin.write(UPANGLE);
         delay(500);
      }
  beam.step(VERT_STEP*(FIELD_LENGTH),BACKWARD,DOUBLE);

}


void smoother(int image[][FIELD_WIDTH], float smooth[][FIELD_WIDTH]){
 float gauss[][5]={{2.0/159.0, 4.0/159.0, 5.0/159.0, 4.0/159.0, 2.0/159.0},
 {4.0/159.0, 9.0/159.0, 12.0/159.0, 9.0/159.0, 4.0/159.0},
 {5.0/159.0, 12.0/159.0, 15.0/159.0, 12.0/159.0, 5.0/159.0},
 {4.0/159.0, 9.0/159.0, 12.0/159.0, 9.0/159.0, 4.0/159.0},
 {2.0/159.0, 4.0/159.0, 5.0/159.0, 4.0/159.0, 2.0/159.0}};
 float padded[FIELD_LENGTH+4][FIELD_WIDTH+4];
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
  for(i=2; i<FIELD_LENGTH+2; i++){
    for (j=2; j<FIELD_WIDTH+2; j++){
      for (k=i-2; k<i+3; k++){
        for(l=j-2; l<j+3; l++){
          cumsum+=(float)padded[k][l]*gauss[k+2-i][l+2-j];
        }
      }
      smooth[i-2][j-2]=cumsum;
      cumsum=0.0;
    }
  } 
}

void sobel(float smooth[][FIELD_WIDTH], float phase[][FIELD_WIDTH], float magnitude[][FIELD_WIDTH]) {
    
  float sobel_vert[][3]={ {-1.0, 0.0, 1.0}, {-2.0, 0.0, 2.0},{-1.0, 0.0, 1.0}};
  float sobel_horz[][3]={ {-1.0, -2.0, -1.0}, {0.0, 0.0, 0.0},{1.0, 2.0, 1.0}};
  float padded2[FIELD_LENGTH+2][FIELD_WIDTH+2];
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
    for(i=1; i<FIELD_LENGTH+1; i++){
      for (j=1; j<FIELD_WIDTH+1; j++){
        for (k=i-1; k<i+2; k++){
          for(l=j-1; l<j+2; l++){
           vertcumsum+=padded2[k][l]*sobel_vert[k+1-i][l+1-j];
          }
        }
      edge_vert[i-1][j-1]=vertcumsum;
      vertcumsum=0.0;
     }
    } 
    
  float horzcumsum=0.0;
    for(i=1; i<FIELD_LENGTH+1; i++){
      for (j=1; j<FIELD_WIDTH+1; j++){
        for (k=i-1; k<i+2; k++){
          for(l=j-1; l<j+2; l++){
           horzcumsum+=padded2[k][l]*sobel_horz[k+1-i][l+1-j];
          }
        }
      edge_horz[i-1][j-1]=horzcumsum;
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

 

void thiner(float magnitude[][FIELD_WIDTH],float phase[][FIELD_WIDTH], float thin[][FIELD_WIDTH]){
   int i,j;
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
        phase[i][j]=-45.0;
      }
    } 
  }
  
 for(i=0; i<FIELD_LENGTH; i++){
   for(j=0; j<FIELD_WIDTH; j++){
      thin[i][j]=magnitude[i][j];
   }
 }
   

  for(i=1; i<FIELD_LENGTH-1; i++){
    for(j=1; j<FIELD_WIDTH-1; j++){
     if (phase[i][j] ==0.0){
      if (magnitude[i][j] < max(magnitude[i][j-1],magnitude[i][j+1])){
         thin[i][j] = 0.0;
      }
     }
     else if (phase[i][j] ==-45.0){
       if (magnitude[i][j] < max(magnitude[i+1][j-1],magnitude[i-1][j+1])){
         thin[i][j] = 0.0;
      }
     }
     else if (phase[i][j] ==45.0){
       if (magnitude[i][j] < max(magnitude[i+1][j+1],magnitude[i-1][j-1])){
         thin[i][j] = 0.0;
      }
     }
     else if (phase[i][j] ==90.0){
       if (magnitude[i][j] < max(magnitude[i-1][j],magnitude[i+1][j])){
         thin[i][j] = 0.0;
      }
     }
    }
  }  
}

void threshold(float thin[][FIELD_WIDTH], bool points[][FIELD_WIDTH]){
   int i,j;
   float maximum = 0.0;
   for(i=1; i<FIELD_LENGTH-1; i++){
      for(j=1; j<FIELD_WIDTH-1; j++){
      maximum = max(maximum, thin[i][j]);
      }
   } 
  float threshold = 0.12*maximum; //may change number as needed
  for(i=1; i<FIELD_LENGTH-1; i++){
    for(j=1; j<FIELD_WIDTH-1; j++){
      if( thin[i][j] > threshold) {
        points[i][j]= true;
      }
      else {
        points[i][j]= false; 
      }
    }
  }
}

void prepare(bool points[][FIELD_WIDTH], int sequence[][2]){
   int i,j;
   int k=FIELD_LENGTH, l=FIELD_WIDTH;
   for(i=0; i<FIELD_LENGTH; i++){
      for(j=0; j<FIELD_WIDTH; j++){
         if(points[i][j]==1){
            k=i;
            l=j;
            break;
         }
      }
      if(k!=FIELD_LENGTH){
         break;
      }
   }
   sequence[0][0]=k;
   sequence[0][1]=l;
   points[k][l]=0;
   
   int q;
   for(q=1; q<FIELD_LENGTH*FIELD_WIDTH; q++){
    float distance=(float) FIELD_LENGTH + (float) FIELD_WIDTH;
      for(i=0; i<FIELD_LENGTH; i++){
         for(j=0; j<FIELD_WIDTH; j++){
       if (points[i][j]==1){
         if(sqrt((i-sequence[q-1][0])*(i-sequence[q-1][0])+(j-sequence[q-1][1])*(j-sequence[q-1][1]))<distance){
           distance=sqrt((i-sequence[q-1][0])*(i-sequence[q-1][0])+(j-sequence[q-1][1])*(j-sequence[q-1][1]));
           k=i;
           l=j;
         }
       }
     }
    }
    sequence[q][0]=k;
      sequence[q][1]=l;
      points[k][l]=0;
    if (distance==FIELD_LENGTH+FIELD_WIDTH){
      sequence[q][0]=0;
      sequence[q][1]=0;
      break;
    }
   }          
}

void movesr(int sequence [][2], int moves[][3]){
   moves[0][0]=sequence[0][0];
   moves[0][1]=sequence[0][1];
   int i;
   for(i=1; i<FIELD_LENGTH*FIELD_WIDTH; i++){
      moves[i][0]=sequence[i][0]-sequence[i-1][0];
      moves[i][1]=sequence[i][1]-sequence[i-1][1];
   }
   for(i=0; i<FIELD_LENGTH*FIELD_WIDTH; i++){
      if(sqrt((float) moves[i][0]* (float) moves[i][0]+(float) moves[i][1]* (float) moves[i][1])>1.5){
         moves[i][2]=1;
      }
      else {
         moves[i][2]=0;
      }
   }
}




