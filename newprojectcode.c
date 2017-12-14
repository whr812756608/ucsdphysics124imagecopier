#include <stdio.h> 
#include <math.h>
typedef int bool;
#define true 1
#define false 0
#define PI 3.141592653589793


#define FIELD_LENGTH 20
#define FIELD_WIDTH 20

void smoother(int image[][FIELD_WIDTH], float smooth[][FIELD_WIDTH]);
void sobel(float smooth[][FIELD_WIDTH], float phase[][FIELD_WIDTH], float magnitude[][FIELD_WIDTH]);
void thiner(float magnitude[][FIELD_WIDTH],float phase[][FIELD_WIDTH], float thin[][FIELD_WIDTH]);
void threshold(float magnitude[][FIELD_WIDTH], bool points[][FIELD_WIDTH]);
void prepare(bool points[][FIELD_WIDTH], int sequence[][2]);
void mover(int sequence [][2], int moves[][3]);

float max(float a, float b){
   if(a>=b){
      return a;
   }
   else{
      return b;
   }
}

int main() {
  int image[][FIELD_WIDTH]={{0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0},
  {0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0}};
  
  float smooth[FIELD_LENGTH][FIELD_WIDTH];
  float magnitude[FIELD_LENGTH][FIELD_WIDTH];
  float phase[FIELD_LENGTH][FIELD_WIDTH];
  float thin[FIELD_LENGTH][FIELD_WIDTH];
  bool points[FIELD_LENGTH][FIELD_WIDTH];
  int sequence[FIELD_LENGTH*FIELD_WIDTH][2];
  int move[FIELD_LENGTH*FIELD_WIDTH][3];
  
  smoother(image, smooth);
  sobel(smooth, phase, magnitude);
  thiner(magnitude , phase, thin);
  threshold(thin, points);
  
  int i,j;
  
  for(i=0; i<FIELD_LENGTH; i++){
	  for(j=0; j<FIELD_WIDTH; j++){
		  printf("%d ", ((int) points[i][j]));
	  }
	  printf("\n");
  }
  
  prepare(points, sequence);
  mover(sequence, move);
  
  for(i=0; i<FIELD_LENGTH*FIELD_WIDTH; i++){
	  for(j=0; j<3; j++){
		  printf("%d ", ((int) move[i][j]));
	  }
	  printf("\n");
  }
	
return 0;
}


void smoother(int image[][FIELD_WIDTH], float smooth[][FIELD_WIDTH]){
 float gauss[][5]={{2.0/159.0, 4.0/159.0, 5.0/159.0, 4.0/159.0, 2.0/159.0},
 {4.0/159.0, 9.0/159.0, 12.0/159.0, 9.0/159.0, 4.0/159.0},
 {5.0/159.0, 12.0/159.0, 15.0/159.0, 12.0/159.0, 5.0/159.0},
 {4.0/159.0, 9.0/159.0, 12.0/159.0, 9.0/159.0, 4.0/159.0},
 {2.0/159.0, 4.0/159.0, 5.0/159.0, 4.0/159.0, 2.0/159.0}};
 float padded[FIELD_LENGTH+4][FIELD_WIDTH+4];
 int i;
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    `
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

void mover(int sequence [][2], int move[][3]){
   move[0][0]=sequence[0][0];
   move[0][1]=sequence[0][1];
   int i;
   for(i=1; i<FIELD_LENGTH*FIELD_WIDTH; i++){
      move[i][0]=sequence[i][0]-sequence[i-1][0];
      move[i][1]=sequence[i][1]-sequence[i-1][1];
   }
   for(i=0; i<FIELD_LENGTH*FIELD_WIDTH; i++){
      if(sqrt((float) move[i][0]* (float) move[i][0]+(float) move[i][1]* (float) move[i][1])>1.5){
         move[i][2]=1;
      }
      else {
         move[i][2]=0;
      }
   }
}
