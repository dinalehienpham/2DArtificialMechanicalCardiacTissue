#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <math.h>
#include <time.h>
//#include "gperftools/profiler.h"
#include "pca9685.h"

// different modes
#define PWM 1    // PWM
#define smotor 2 // servo motor

#define numPins 25
int cells[numPins]; // for generating cell spacing

#define PIN_BASE 100
#define MAX_PWM 4096
#define HZ 50

#define numBoards 4
int pca[numBoards];      // corresponds to each PCA9685
int pins[numBoards][16]; // for each board, pins 0-15 are independent, pin 16 controlls all pins (not used)

// find corresponding board number and actual pin number of that board given reference pin
int findPin(int refPin) {
	int pin;
	for (int i=0;i<numBoards;i++) {
		for (int j=0;j<16;j++) {
			if (pins[i][j]==refPin) {
				pin=j;
			}
		}
	}
	return pin;
}

int findBoard(int refPin) {
	int board;
	for (int i=0;i<numBoards;i++) {
		for (int j=0;j<16;j++) {
			if (pins[i][j]==refPin) {
				board=i;
			}
		}
	}
	return board;
}

// simplifies function that writes to the PCA9685 boards
void PWMwrite(int refPin, int value) {
	int b=findBoard(refPin); // board index
	int pin=findPin(refPin); // corresponding pin of that board
	
	pca9685PWMWrite(pca[b], pin, 0, value);
}

// initialize reference pins matrix to consecutive numbers that can be easily used for PWMwrite
void initializePins(void) {
	for (int i=0,n=0;i<numBoards;i++) {
		for (int j=0;j<16;j++,n++) {
			pins[i][j]=n;
		}
	}
}

// map functions
int pwmmap(double val) { // led, SMA
	if (val>1.25) {
		val=1.25;
	} else if (val<0) {
		val=0;
	}
	return (val/1.25)*PWMrange;
}

int smmap(double val) { // servo motor
	return PWMrange-pwmmap(val);
}

void writeV(int xnum, int ynum, float* v, int numPins, int mode) {
	int sr=sqrt(numPins);
	int factor=xnum/sqrt(numPins);
	
	// evenly space out pins in tissue
	cells[0]=1; // first row
	for (int i=1;i<sr;i++) {
		cells[i]=cells[0]+i*factor;
	}
	for (int j=1,i=sr;i<numPins;j++) { // rest of the tissue
		for (int k=0;k<sr;k++,i++) {
			cells[i]=cells[j]*ynum+cells[k];
		}
	}
	
	// write values to pins and print to screen
	switch (mode) {
		case 1: // PWM
			for (int i=0;i<numPins;i++) {
				int n=cells[i];
				PWMwrite(i, pwmmap(v[n]));
				printf("Pin %2d: %f, mapped: %d\n", i+1, v[n], pwmmap(v[n]));
			}
			break;
		case 2: // servo motor
			for (int i=0;i<numPins;i++) {
				int n=cells[i];
				PWMwrite(i, smmap(v[n]));
				printf("Pin %2d: %f, mapped: %d\n", i+1, v[n], smmap(v[n]));
			}
			break;
	} 
	printf("\n");
}		

int main(void) {
	//ProfilerStart("2DcellsP.prof"); // google pprof
	
	// setup wiringPi
	if (wiringPiSetup()==-1) { 
        printf("Error: wiringPi setup failed.\n");
        return 1;
    }
    
    initializePins();

	// setup PCA9685 boards
	for (int i=0;i<numBoards;i++) {
		pca[i]=pca9685Setup(PIN_BASE+i*17,0x40+i,HZ);
		if (pca[i]<0) {
			printf("Error: PCA9685 board %d setup failed.\n",i);
			return pca[i];
		}
		pca9685PWMReset(pca[i]);
	}
	
	// check if numPins is perfect square root
	if (sqrt(numPins)-floor(sqrt(numPins)!=0) {
		printf("Error: numPins value is not a perfect square root.\n");
		return 1;
	}
    
	// set pinModes
    //setupPins(numPins,smotor);

	while(1) {
		const int xnum=100; // change tissue size
		const int ynum=100; // change tissue size
		const int num=xnum*ynum;

		// array
		float *v=(float*) calloc(num, sizeof(float));   // membrane voltage
		float *h=(float*) calloc(num, sizeof(float));   // Na channel inactivation gate
		float *f=(float*) calloc(num, sizeof(float));   // Ca channel inactivation gate
		float *stim=(float*) calloc(num, sizeof(float));
		float *tmp=(float*) calloc(num, sizeof(float)); // temporary variable to solve diffusion eqn

		// initial values
		for (int i=0;i<num;i++) {
			v[i]=0;
			h[i]=0.8;
			f[i]=0.5;
			stim[i]=0;
		}
		
		// stimlate the corner
		for (int i=0;i<10;i++) {
			for(int j=0;j<10;j++) {
				v[i*ynum+j]=1;
			}
		}
		
		// constants
		const float dt=0.1; // time step (0.1 ms)
		float tmax=200; 	// total simulation time

		// convert to int
		int tnmax=tmax/dt;

		// main loop
		for (int tn=0;tn<tnmax;tn++) {
            // write results every x ms
            int x=1; // change print interval
            if (tn%(x*10)==0) {
				writeV(xnum,ynum,v,numPins,smotor);
				delay(50); // 5 ms delay (change)
			}
		
			// Euler method
			#pragma omp parallel for // openMP
			for (int i=0;i<num;i++) {
				const float tauso=15;
				const float taufi=0.8;
				const float tauh1=4.8;
				const float tauh2=10.0;
				const float tausi=4.0;
				const float tauf1=100;
				const float tauf2=30;
				float minf=pow((v[i]/0.2),6)/(1+pow((v[i]/0.2),6));
				float hinf=1/(1+pow((v[i]/0.1),6));
				float dinf=pow((v[i]/0.4),4)/(1+pow((v[i]/0.4),4));
				float finf=1/(1+pow((v[i]/0.1),4));
				float tauh=tauh1+tauh2*exp(-20*pow((v[i]-0.1),2));
				float tauf=tauf2+(tauf1-tauf2)*v[i]*v[i]*v[i];

				float jfi=h[i]*minf*(v[i]-1.3)/taufi; // Fast inward current (Na current)
				float jsi=f[i]*dinf*(v[i]-1.4)/tausi; // Slow inward current (Ca current)
				float jso=(1-exp(-4*v[i]))/tauso; 	  // outward current (K current)
				float ion=-(jfi+jsi+jso-stim[i]); 	  // total transmembrane current

				float dh=(hinf-h[i])/tauh;
				float df=(finf-f[i])/tauf;

				// update variables
				v[i]+=ion*dt;
				h[i]+=dh*dt;
				f[i]+=df*dt;
			}
			
			// solve Diffusion
			const float dfu=0.001; // originally 0.001 (change)
			const float dx=0.015;  // 0.15 mm
			
			// non-flux boundary
			#pragma omp parallel for // openMP
			for (int i=0;i<xnum;i++) {
				v[i*ynum+0]=v[i*ynum+2];
				v[i*ynum+ynum-1]=v[i*ynum+ynum-3];
			}
			for (int j=0;j<ynum;j++) {
				v[0*ynum+j]=v[2*ynum+j];
				v[(xnum-1)*ynum+j]=v[(xnum-3)*ynum+j];
			}
			#pragma omp parallel for collapse(2) // openMP
			for (int i=1;i<xnum-1;i++) {
				for (int j=1;j<ynum-1;j++) {
					tmp[i*ynum+j]=v[i*ynum+j]+(v[(i-1)*ynum+j]+v[(i+1)*ynum+j]+v[i*ynum+(j-1)]+v[i*ynum+(j+1)]-4*v[i*ynum+j])*dfu*dt/(dx*dx)/2;
				}
			}
			for (int i=0;i<xnum;i++) {
				tmp[i*ynum+0]=tmp[i*ynum+2];
				tmp[i*ynum+ynum-1]=tmp[i*ynum+ynum-3];
			}
			for (int j=0;j<ynum;j++) {
				tmp[0*ynum+j]=tmp[2*ynum+j];
				tmp[(xnum-1)*ynum+j]=tmp[(xnum-3)*ynum+j];
			}
			#pragma omp parallel for collapse(2) // openMP
			for (int i=1;i<xnum-1;i++) {
				for (int j=1;j<ynum-1;j++) {
					v[i*ynum+j]=tmp[i*ynum+j]+(tmp[(i-1)*ynum+j]+tmp[(i+1)*ynum+j]+tmp[i*ynum+(j-1)]+tmp[i*ynum+(j+1)]-4*tmp[i*ynum+j])*dfu*dt/(dx*dx)/2;
				}
			}
		}
		
		free(v);
		free(h);
		free(f);
		free(stim);
		free(tmp);
		
		//return 0; // google pprof
	}
	
	//ProfilerStop(); // google pprof
	return 0;
}
