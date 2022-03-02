#include <iostream>
#include <vector>
using namespace std;

/**
 * Jacobian is an abstraction layer for configuing and utilizing a microcontrollers
 * GPIO pinouts.
 * 
 * @author Ian Wilkey (iwilkey)
 * @since 1.1.0
 */
namespace Jacobian {
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
			// Data members of controller.
			string name;
			thread listener;
			vector< pair<string, int> > pinout;
			bool init(void);
			bool kill(void);

			// General utilties
			template <typename T>
				void log(string tag, T obj);
			void waitForSeconds(double s);
			void readCommand(string command);

			// Pin utilites
			int returnPinFromName(string pinName);
			void configurePin(int id, string name, int mode, int pud);
			void setPinMode(string pin, int mode);
			void setPinPud(string pin, int pud);
			int readPin(string pin);
			void setPin(string pin, int value);

		public:
			Controller(string name);

			string getName(void);
			void setName(string name);
	};
}
