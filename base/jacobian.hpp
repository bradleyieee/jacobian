#include <iostream>
#include <vector>
using namespace std;

/**
 * Jacobian is an abstraction layer for configuing and utilizing a microcontrollers
 * GPIO pinouts.
 * 
 * @author Bradley IEEE 2022
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
			string name;
			vector< pair<string, int> > pinout;
			bool init(void);
		public:
			Controller(string name);
			template <typename T>
				void log(string tag, T obj);
			int returnPinFromName(string pinName);
			void configurePin(int id, string name, int mode, int pud);
			void setPinMode(string pin, int mode);
			void setPinPud(string pin, int pud);
			void waitForSeconds(double s);
			int readPin(string pin);
			void setPin(string pin, int value);
			string getName(void);
			void setName(string name);
			void readCommand(string command);
	};
}
