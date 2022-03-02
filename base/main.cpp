/**
 * JACOBIANOS v.1.1.0 Test (Debug mode) 
 * 
 * Simple blink with JacobianOS.
 * 
 * @author Bradley IEEE 2022 (Send money!)
 * @since 1.1.0
*/
                        
#include <wiringPi.h>
#include "jacobian.hpp"
using namespace Jacobian;
using namespace std;

#define vOUT 0 // Okay, Jacobian doesn't understand the ACTUAL pinout system. It's all relative. See log.txt.

// COMPILATION: 
	// $ g++ jacobian.cpp main.cpp -o build -pthread -lwiringPi
	// $ sudo ./build

// Main method begins here.
int main(void) {
	
	Controller pi("PI 3");
	pi.configurePin(vOUT, "LED", OUTPUT, 1);
	
	bool dir = true;
	float delay = 0.25f;
	while(true) {
		static bool tog = false;
		pi.setPin("LED", ((tog) ? 1 : 0));
		tog = !tog;
		
		// PWM demonstration; ocillating between fast and slow delays.
		delay = (dir) ? delay / 1.05f : delay * 1.05f;
		if(delay <= 0.000000001f) dir = false;
		else if(delay >= 0.1f) dir = true;
		pi.log("Info", "New delay: " + to_string(delay));
		
		pi.waitForSeconds(delay);
	}

	pi.kill();
	
	return 0;
}
