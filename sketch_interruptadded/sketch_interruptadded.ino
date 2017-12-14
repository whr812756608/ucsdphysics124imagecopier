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

const int FIELD_LENGTH=25/*How many pixels high is the paper*/;
const int FIELD_WIDTH=25/*How many pixels wide is the paper*/;
const int VERT_STEP=40/*How many steps on a stepper motor movess down one pixel*/;
const int HORZ_STEP=52/*How many steps on a stepper motor movess accross one pixel*/;
const int SENSE_MAX=255/*The maximum sensor value*/;
const int SENSE_MIN=0/*The minimum sensor value*/;
const float PICKUP_THRESHOLD=1/*how far the pen needs to moves for it to be picked up*/;  
const int THRESHOLD=150; /*we shall tune this as needed*/
const int UPANGLE=10;
const int DOWNANGLE=0;

volatile bool points[FIELD_LENGTH][FIELD_WIDTH]; 

void setup() {
  attachInterrupt(digitalPinToInterrupt(18), scan, LOW);
  attachInterrupt(digitalPinToInterrupt(19), draw, LOW);
  Serial.begin(9600);
  apds.init();
  apds.enableLightSensor(false);
  beam.setSpeed(10);
  dolly.setSpeed(10);
  
  pin.attach(22, 450, 2500); //***ADD PULSE TIMES!***
  /*We also may need to do some pin modes*/
}

void loop() {
  interrupts();
  //noInterrupts();

  //scan(points);
  //draw(points);
  // put your main code here, to run repeatedly:


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
//volatile boolean points[][FIELD_WIDTH]

void scan(){
  Serial.println("interrupt");
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

 void draw(){
 int i, j;
  for(i=0; i<FIELD_LENGTH-1; i+=2){
    for(j=0; j<FIELD_WIDTH; j++){
      if (points[i][j]==1){
        Serial.println("I hit a 1");
         pin.write(DOWNANGLE);
         delayMicroseconds(10000);
         //delay(500);
         pin.write(UPANGLE);
         //delay(500);
      }
      dolly.step(HORZ_STEP,FORWARD,DOUBLE);; //may have to change the sign.
    }
      if ( points[i][FIELD_WIDTH]==1){
        Serial.println("I hit a 1");
         pin.write(DOWNANGLE);
         delayMicroseconds(10000);
         //delay(500);
         pin.write(UPANGLE);
         //delay(500);
      }
    beam.step(VERT_STEP,FORWARD,DOUBLE);
    for(j=FIELD_WIDTH-1; j>=0; j--){
      if (points[i+1][j]==1){
        Serial.println("I hit a 1");
         pin.write(DOWNANGLE);
         delayMicroseconds(10000);
         //delay(500);
         pin.write(UPANGLE);
         //delay(500);
      }
      dolly.step(HORZ_STEP,BACKWARD,DOUBLE);// may have to change the sign.
    } 
      if (points[i+1][0]==1){
        Serial.println("I hit a 1");
         pin.write(DOWNANGLE);
         delayMicroseconds(10000);
         ////delay(500);
         pin.write(UPANGLE);
         //delay(500);
      }
    beam.step(VERT_STEP,FORWARD,DOUBLE);
  } 
  for(j=0; j<FIELD_WIDTH; j++){
      if (points[FIELD_LENGTH-1][j]==1){
         pin.write(DOWNANGLE);
         delayMicroseconds(10000);
         //delay(500);
         pin.write(UPANGLE);
         //delay(500);
      }
    dolly.step(HORZ_STEP,FORWARD,DOUBLE); //may have to change the sign.
  }
      if (points[FIELD_LENGTH-1][FIELD_WIDTH]==1){
         pin.write(DOWNANGLE);
         delayMicroseconds(10000);
         //delay(500);
         pin.write(UPANGLE);
         //delay(500);
      }
  beam.step(VERT_STEP,FORWARD,DOUBLE);
  for(j=FIELD_WIDTH-1; j>=0; j--){
      if (points[FIELD_LENGTH][j]==1){
         pin.write(DOWNANGLE);
         delayMicroseconds(10000);
         //delay(500);
         pin.write(UPANGLE);
         //delay(500);
      }
   dolly.step(HORZ_STEP,BACKWARD,DOUBLE);// may have to change the sign.
  }
      if (points[FIELD_LENGTH][0]==1){
         pin.write(DOWNANGLE);
         delayMicroseconds(10000);
         //delay(500);
         pin.write(UPANGLE);
         //delay(500);
      }
  beam.step(VERT_STEP*(FIELD_LENGTH),BACKWARD,DOUBLE);

}







