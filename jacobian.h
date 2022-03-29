/**
 * Jacobian is an abstraction layer for configuing and utilizing a Raspberry PI's
 * GPIO pinouts. 
 * 
 * This software is implemented at the lowest level above the hardware of
 * the Bradley IEEE self-driving RC car. In that context, it serves to decompose high
 * level vector input into two PWM signals and generate them using the PI system clock as well
 * as deliver the logic HIGH and LOW out of the GPIO pins.
 * 
 * @author Ian Wilkey (iwilkey)
 * @since 1.0.0
 */

#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <utility>
#include <time.h>
#include <thread>
using namespace std;

#define VERSION "1.4.0"

/**
* The Jacobian namespace encapsulates three main deliniations of tools: general utilities, 
* Pulse Width Modulation generator, and the Controller object.
* 
* @since 1.0.0
*/
namespace jacobian {
	
	/*******************
	General utilities
	/*******************/

	/**
	 * This set of constants, used in conjuction with the radixShift() utility,
	 * can move the decimal of an SI number in base 10 to redefine it in the proper
	 * context.
	 * (ex: radixShift(1.2, MILLI) = 0.0012 seconds, or 1.2 milliseconds)
	 *
	 * @since 1.1.3
	 */
	enum RADIXSHIFT {
		PICO = -12,
		NANO = -9,
		MICRO = -6,
		MILLI = -3,
		CENTI = -2,
		DECI = -1,
		HECTO = 2,
		KILO = 3,
		MEGA = 6,
		GIGA = 9,
		TERA = 12
	};

	float radixShift(float, int);

	template <typename T>
		void log(string, T); // With tag.
	template <typename T>
		void log(T); // Without tag.

	float timeToDutyCycle(int, float);
	void waitForSeconds(double);
	
	vector<string> tokenize(string, char);
	
	/*******************
	Pulse Width Modulation generator
	/*******************/

	/**
	 * This object creates a Pulse Width Modulation signal at specified frequency
	 * and duty cycle. It operates soley by the change in the internal PI clock, so the mathematics
	 * are done with the hope that the ticks are not delayed.
	 * 
	 * @since 1.1.0
	 */
	class PWM {
		private:
			// Data members.
			bool on; // Is the PWM currently producing a logic HIGH or LOW?
			int frequency; // (hz)
			double dutyCycle; // (%)

			// Internal clock state.
			clock_t now = clock(), 
				last, delta = 0, precisionSince,
				precisionPeriod;

		public:
			const int PRECISION = pow(10, (float)MEGA); // The amount of decimal precision of the PWM clock.
			
			PWM(int, double);
			void setDutyCycle(double);
			void tick(void);
			bool eval(void);
	};
	
	/*******************
	Controller object
	/*******************/

	/**
	 *	The Jacobian controller class defines a controller with software configured GPIO
	 * outputs or inputs. A controller is defined by name, and it's pins by ID.
	 * It has many utility functions that are described in full above the .cpp implmentation
	 * function headers.
	 * 
	 * @since 1.0.0
	 */
	class Controller {
		private:
			// Data members.
			bool running = false, // Is the controller currently running?
				overriden = false; // Is the controller being overriden by manual control?
			string name; // Name of distinct controller.
			vector< pair<string, int> > pinout; // Map of the configured GPIO pins during session.

			bool init(void); // To solidify the configured GPIO pins.
			
		public:
			Controller(string name);
			
			// Pin utilities.
			int returnPinFromName(string);
			void configurePin(int, string, int, int);
			void setPinMode(string, int);
			void setPinPud(string, int);
			int readPin(string);
			void setPin(string, int);

			// Controller state.
			bool isRunning(void);
			void setState(bool);
			bool isOverridden(void);
			void Override(bool);

			string getName(void);
			void setName(string);

			void kill(void); // MUST be called when ending program.
	};

}
