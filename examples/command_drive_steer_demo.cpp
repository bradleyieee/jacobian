#include <iostream>
#include <thread>
#include "../jacobian.h"
using namespace std;
using namespace jacobian;

// Parse specific commands...
/**
 * drive (0% - 100%): Drive at percentage of top speed.
 * steer (12 - 20): 12 full right, 16 full center, 20 full left.
 * override (0 or 1): Set the controller in control or car or manual override.
 * stop: Stop the program.
 * 
 * @params
 * 	Controller c (reference): The controller to command to.
 *	PWM drive (reference): The driver PWM channel.
 * 	PWM steer (reference): The steer PWM channel.
 */
static void command(Controller & c, PWM & drive, PWM & steer) {
	while(true) {
		static string line, command, args;
		getline(cin, line);
		command = line;
		
		for(int c = 0; c < line.size(); c++) {
			if(line[c] == ' ') {
				command = line.substr(0, c);
				args = line.substr(c + 1, line.size() - c);
				break;
			}
		}
	
		if(command == "stop") {
			c.setState(false);
			break;
		}
		
		// TODO: Make commands fail proof...
		
		// drive [0 - 100](%)...
		if(command == "drive") {
			int percent = stoi(args);
			float time = ((2.0 - 1.5) * ((float)(percent) / 100.0f)) + 1.5f;
			cout << "New drive width set: " << time << endl;
			drive.setDutyCycle(timeToDutyCycle(60, radixShift(time, MILLI)));
			continue;
		}
		
		// Steer time. [12 - 20]
		if(command == "steer") {
			int time = stoi(args);
			steer.setDutyCycle(timeToDutyCycle(60, radixShift(((float)time / 10), MILLI)));
			continue;
		}
		
		if(command == "override") {
			if(args == "0") {
				c.Override(false);
				continue;
			}
			c.Override(true);
			continue;
		}

		log("Error", "JacobianOS does not understand this command!");
	}
	return;
}

// Main instructions.
int main(int argc, char ** args) {
		static Controller c("pi3b");
		c.configurePin(2, "drive", OUTPUT, 1);
		c.configurePin(25, "override", OUTPUT, 1);
		c.configurePin(4, "steer", OUTPUT, 1);
	
		PWM driver(60, timeToDutyCycle(60, radixShift(1.5, MILLI))),
			steerer(60, 9.6f);

		thread l(command, ref(c), ref(driver), ref(steerer));

	while(c.isRunning()) {
		if(!c.isOverridden()) {
			driver.tick();
			steerer.tick();
			c.setPin("drive", driver.eval());
			c.setPin("steer", steerer.eval());
			c.setPin("override", 1);
		} else c.setPin("override", 0);
	}

	c.kill();
	l.join();

	return 0;
}
