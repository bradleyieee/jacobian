/**
 * This software, JacobianOS, is implemented on a Raspberry PI 3b at the lowest level above the hardware of
 * the Bradley IEEE self-driving RC car. It serves to decompose high
 * level vector input into two PWM signals and generate them using the PI system clock as well
 * as deliver the logic HIGH and LOW out of the GPIO pins. It is also an interface to communicate to the car via console commands and 
 * JacobianOS Routine Scripts (*.jors), which specify sequences of timed commands to translate the car.
 *
 * @since Jacobian 1.4.0
 * @version 1.1.0
 * @author Ian Wilkey (iwilkey)
 * 
 * Compilation: g++ ../jacobian.cpp jacobianos.cpp -o build -lwiringPi -pthread -std=c++11
 */

#include <iostream>
#include <fstream>
#include <thread>
#include <wiringPi.h>
#include "../jacobian.h"
using namespace std;
using namespace jacobian;

#define VERSION "1.1.0"

/*******************
Invokable commands
/*******************/

/**
 * Translate the car forwards or backwards declairing direction and percentage speed. 
 * Command style: drive (f or b, 0 - 100)[%]...
 * 
 * @params
 * 	string args (reference): The non-parsed arguments passed to command.
 * 	bool dlog (reference): The state of the JacobianOS log option (writing output to console).
 * 	bool reverse (reference): The state of the reverse mode on the car.
 * 	PWM drive (reference): The actual PWM object connected to the selected GPIO pinout for the drive motor.
 */
void invokeDrive(string & args, bool & dlog, bool & reverse, PWM & drive) {
	vector<string> argTokens = tokenize(args, ' ');
	if(argTokens.size() != 2) {
		log("Error", "Drive command must be invoked with exactly two arguments! See \"help\" for details.");
		return;
	}
	int dir = -1; // undef dir.
	if(argTokens[0] == "f") dir = 1;
	else if(argTokens[0] == "b") dir = 0;
	if(dir < 0) {
		log("Error", "Drive command must be invoked with a valid direction! See \"help\" for details.");
		return;
	}
	if(dir == 1) {
		if(reverse) reverse = false;
		// Forward implementation.
		int percent = stoi(argTokens[1]); // TODO: Make this a float.
		percent = (percent > 100) ? 100 : percent;
		percent = (percent < 0) ? 0 : percent;
		float time = ((2.0f - 1.5f) * ((float)(percent) / 100.0f)) + 1.5f;
		drive.setDutyCycle(timeToDutyCycle(60, radixShift(time, MILLI)));
		if(dlog)
			log("Success", "The car is now moving forward at " + to_string(percent) + "% of its top speed. Pulse width in ms: " + to_string(time));
		return;
	} else {
		if(!reverse) {	

			// Backwards implementation.
			// Needs special timed beginning sequence!
			
			// Development note: I notice that my waitForSeconds() method is changing the true
			// pulse width measured by oscilloscope. I need to find an alternative to delay.
			// Idea: thread sleep_for()? 
			 
			if(dlog) log("Break routine", "Beginning break routine...");
			drive.setDutyCycle(timeToDutyCycle(60, radixShift(1.05f, MILLI)));
			if(dlog) log("Break routine", "Holding break...");
			waitForSeconds(1.0f); // These times should be tweaked to find the shortest possible time for pulse.
			drive.setDutyCycle(timeToDutyCycle(60, radixShift(1.5, MILLI)));
			if(dlog) log("Break routine", "Pulsing reset...");
			waitForSeconds(0.25f);

			// drive.setDutyCycle(timeToDutyCycle(60, radixShift(1.05f, MILLI)));
			// if(dlog) log("Break routine", "Setting to break mode now...");
			// waitForSeconds(0.25f);

			reverse = true;
			if(dlog) 
				log("Break routine", "Break routine finished.");
		}
		
		int percent = stoi(argTokens[1]); // TODO: Make this a float.
		percent = (percent > 100) ? 100 : percent;
		percent = (percent < 0) ? 0 : percent;
		float time = ((1.0f - 1.5f) * ((float)(percent) / 100.0f)) + 1.5f;
		drive.setDutyCycle(timeToDutyCycle(60, radixShift(time, MILLI)));
		
		if(dlog)
			log("Success", "The car is now moving backwards at " + to_string(percent) + "% of its top speed. Pulse width in ms: " + to_string(time));
	}	
	return;
}

/**
 * Rotate the front axis full right to full left specifiying pulse time in milliseconds * 1000.
 * Command style: steer (1200 - 2000)[ms * 1000]...
 * 
 * @params
 * 	string line (reference): The non-parsed command passed.
 * 	bool dlog (reference): The state of the JacobianOS log option (writing output to console).
 * 	PWM steer (reference): The actual PWM object connected to the selected GPIO pinout for the servo motor.
 */
void invokeSteer(string & line, bool & dlog, PWM & steer) {
	vector<string> argTokens = tokenize(line, ' ');
	if(argTokens.size() != 2) {
		log("Error", "Steer command must be invoked with exactly one specified pulse time (ms) * 1000! See \"help\" for details.");
		return;
	}
	int time = stoi(argTokens[1]);
	time = (time > 2000) ? 2000 : time;
	time = (time < 1200) ? 1200 : time;
	steer.setDutyCycle(timeToDutyCycle(60, radixShift(((float)time / 1000.0f), MILLI)));
	if(dlog)
		log("Success", "The steering pulse width is now set to: " + to_string(((float)time / 1000.0f)));
	return;
}


/**
 * Stop car in tracks.
 * Command style: break (no args)...
 * 
 * @params
 *  bool reverse (reference): The state of the reverse mode on the car.
 * 	bool dlog (reference): The state of the JacobianOS log option (writing output to console).
 * 	PWM drive (reference): The actual PWM object connected to the selected GPIO pinout for the drive motor.
 */
void invokeBreak(bool & reverse, bool & dlog, PWM & drive) {
	if(reverse) {
		drive.setDutyCycle(timeToDutyCycle(60, radixShift(1.6, MILLI)));
		waitForSeconds(0.10f);
		drive.setDutyCycle(timeToDutyCycle(60, radixShift(1.00, MILLI)));
		goto out;
	}
	drive.setDutyCycle(timeToDutyCycle(60, radixShift(1.00, MILLI)));
	waitForSeconds(0.10f);
	drive.setDutyCycle(timeToDutyCycle(60, radixShift(1.5, MILLI)));
	out:;
	if(dlog)
		log("Success", "The car has stopped moving.");
	return;
}

/**
 * Stop entire JacobianOS.
 * Command style: stop (no args)...
 * 
 * @params
 *  Controller c (reference): The actual configured Pi3b Controller object attached to car.
 */
void invokeStop(Controller & c) {
	c.setState(false);
	return;
}

/**
 * Parse specific commands...
 * 
 * @params
 * 	Controller c (reference): The controller to command to.
 *	PWM drive (reference): The driver PWM channel.
 * 	PWM steer (reference): The steer PWM channel.
 */
static void command(Controller & c, PWM & drive, PWM & steer) {
	bool dlog = false,
		reverse = false;
	while(true) {
		
		cout << "[Command ready]: ";
		static string line, command, args;
		getline(cin, line);
		command = line;
		
		// Tokenize command...
		for(int c = 0; c < line.size(); c++) {
			if(line[c] == ' ') {
				command = line.substr(0, c);
				args = line.substr(c + 1, line.size() - c);
				break;
			}
		}
	
		// Terminate entire program.
		// Command style: stop (no args)...
		if(command == "stop") {
			invokeStop(c);
			break;
		}
		
		// Load a routine script written in JacobianOS Rountine Script (.jors).
		// Command style: load (path_to_jrs)...
		if(command == "load") {
			
			vector<string> argTokens = tokenize(line, ' ');
			if(argTokens.size() != 2) {
				log("Error", "Load command must be invoked with a path to routine script! See \"help\" for details.");
				continue;
			}
			
			vector<string> subArgs = tokenize(argTokens[1], '.');
			if(subArgs.size() != 2) {
				log("Error", "Routine script is in an invalid format! See \"help\" for details.");
				continue;
			}
			if(subArgs[1] != "jors") {
				log("Error", "Routine script must be a JacobianOS Routine Script (.jors)! See \"help\" for details.");
				continue;
			}
			
			ifstream in;
			in.open(subArgs[0] + ".jors");
			
			if(!in) {
				log("Error", "Routine script does not exist at specified path! See \"help\" for details.");
				continue;
			}
			
			string line;
			int comC = 0;
			log("Success", "JacobianOS is now beginning specified routine...");
			while(getline(in, line)) {
				comC++;
				if(line.empty()) continue;
				// Reading JORS...
				
				// Tokenize line...
				string command, args;
				for(int c = 0; c < line.size(); c++) {
					if(line[c] == ' ') {
						command = line.substr(0, c);
						args = line.substr(c + 1, line.size() - c);
						break;
					}
				}
				
				if(command == "drive") {
					invokeDrive(args, dlog, reverse, drive);
					continue;
				}
				
				if(command == "steer") {
					invokeSteer(line, dlog, steer);
					continue;
				}
				
				if(line == "break") {
					invokeBreak(reverse, dlog, drive);
					continue;
				}
				
				if(command == "log") {
					log("JORS Log", args);
					continue;
				}
				
				if(command == "wait") {
					vector<string> argTokens = tokenize(line, ' ');
					if(argTokens.size() != 2) {
						log("JORS Syntax Error", "Wait time must be specified as: wait (float)[time in seconds].");
						continue;
					}
					waitForSeconds(stof(argTokens[1]));
					continue;
				}
				
				log("JORS Syntax Error", "Unknown command at line " + to_string(comC));
			}
			
			in.close();
			log("Success", "JacobianOS has finished specified routine...");
			invokeBreak(reverse, dlog, drive);
			steer.setDutyCycle(9.6f);
			continue;
		}
		
		if(command == "drive") {
			invokeDrive(args, dlog, reverse, drive);
			continue;
		}
		
		// Stop car in tracks.
		// Command style: break (no args)...
		if(command == "break") {
			invokeBreak(reverse, dlog, drive);
			continue;
		}
		
		// Rotate the front axis full right to full left specifiying pulse time in milliseconds * 1000.
		// Command style: steer (1200 - 2000)[ms * 1000]...
		if(command == "steer") {
			invokeSteer(line, dlog, steer);
			continue;
		}
		
		// Set the manual override true or false with software.
		// Command style: override (0 or 1)...
		if(command == "override") {
			vector<string> argTokens = tokenize(line, ' ');
			if(argTokens.size() != 2) {
				log("Error", "Override command must be invoked with exactly one specified state! See \"help\" for details.");
				continue;
			}
			if(args == "0") {
				c.Override(false);
				continue;
			}
			c.Override(true);
			continue;
		}
		
		if(command == "log") {
			dlog = !dlog;
			if(dlog) 
				log("Success", "JacobianOS is now set to log commands.");
			else log("Success", "JacobianOS will now discontinue to log commands.");
			continue;
		}
		
		// General help command. Use when the format of commands is forgotten.
		if(command == "help") {
			cout << endl;
			cout << "JacobianOS version " << VERSION << endl;
			cout << "List of valid commands..." << endl;
			cout << "[NOTE] Please enter commands and arguments with single spaces in between, no commas or other delimiters." << endl << endl;
			cout << "	help (no args): General help command. Use when the format of commands is forgotten." << endl;
			cout << "	log (no args): This will toggle the debug command logging." << endl;
			cout << "	load (path_to_routine): Load a *.jors file to automate commands. JORS documentation is coming soon." << endl;
			cout << "	stop (no args): Terminate entire application." << endl;
			cout << "	drive ('f' or 'b', 0 - 100): Translate the car forwards or backwards specifying direction and percentage max speed." << endl;
			cout << "	break (no args): Stop the car from translating instantaneously." << endl;
			cout << "	steer (1200 - 2000): Rotate the front axis full right to full left specifiying pulse time in milliseconds * 1000." << endl;
			cout << "	override (0 or 1): Set the manual override true or false with software." << endl;
			cout << endl;
			continue;
		}

		log("Error", "JacobianOS does not understand this command! Please type \"help\" for a list of commands.");
	}
	return;
}

// Main instructions.
int main(int argc, char ** args) {
	
	// Init controller...
	static Controller c("pi3b");
	c.configurePin(2, "drive", OUTPUT, 1);
	c.configurePin(4, "steer", OUTPUT, 1);
	c.configurePin(25, "override", OUTPUT, 1);
	
	// Init PWM channels...
	static PWM driver(60, timeToDutyCycle(60, radixShift(1.5, MILLI))),
		steer(60, 9.6f);
	
	// Start command listener...
	thread listener(command, ref(c), ref(driver), ref(steer));

	// Main loop...
	while(c.isRunning()) {
		if(!c.isOverridden()) {
			driver.tick();
			steer.tick();
			c.setPin("drive", driver.eval());
			c.setPin("steer", steer.eval());
			if(c.readPin("override") != 1) 
				c.setPin("override", 1);
		} else {
			if(c.readPin("override") != 0) 
				c.setPin("override", 0);
		}
	}

	// Kill all processes.
	c.kill();
	listener.join();

	return 0;
}
