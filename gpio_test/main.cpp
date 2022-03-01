/**
 * JACOBIANOS v.1.0.0 Test (Debug mode) 
 * 
 * This program is meant to test Jacobian to see if it works correctly.
 * It will blink a simple LED on the 3.3V pinout (17). Please make sure pin 0 is ground.
 * 
 * @author Bradley IEEE 2022 (send money)
 * @since 1.0.0
*/
#include <iostream>
#include <time.h>
#include <math.h>
#include <pigpio.h>
using namespace std;

#define vOUT 17

// COMPILATION: 
// $ g++ -Wall -pthread -o main main.cpp -lpigpio -lrt
// $ sudo ./main

/*
 * Software delay at clock speed of specific device.
 * @perams
 * 	long long int ms: The time (in seconds) the delay should wait.
 * @return
 * 	null (void)
 * */
void delay(long long int s) {
	clock_t start = clock();
	while(true) {
		if((clock() - start) / (double)CLOCKS_PER_SEC > s)
			break;
	}	
	return;
}

template <typename T>
void log(const string tag, const T obj) {
	cout << "JacobianOS [Debug Log] @ t = " << clock() 
		<< ": " << tag << ": " << obj << endl;
	return;
}

bool init(void) {
	if(gpioInitialise() < 0) return false;
	gpioSetMode(vOUT, PI_OUTPUT); // 3.3V pin.
	return true;
}

// Main method begins here.
int main(void) {
	
	if(!init()) {
		log("FATAL", "JacobianOS has encountered an error. See log.txt");
		return -1;
	}
	log("Success", "JacobianOS successfully initialized.");
	
	static int runs = 100;
	while(runs > 0) {
			static bool tog = false;
			gpioWrite(vOUT, ((tog) ? 0x1 : 0x0));
			log("GPIO 17", tog);
			delay(1);
			tog = !tog;
			runs--;
	}
	
	gpioTerminate();
	log("Success", "JacobianOS termination was successful.");
	
	return 0;
}
