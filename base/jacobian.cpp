#include "jacobian.hpp"
#include <time.h>
#include <thread>
#include <wiringPi.h>
using namespace Jacobian;

/*
	This is the implementation of the functions outlined in the Jacobian header file.
	@author Ian Wilkey (iwlkey)
	@since 1.0.0
*/

/*
* Software delay at clock speed of specific device.
* @params
* 		double s: The time (in seconds) the delay should wait.
*/
void Controller::waitForSeconds(double s) {
	clock_t start = clock();
	while(true) {
		if((clock() - start) / (double)CLOCKS_PER_SEC > s)
			break;
	}	
	return;
}

/*
 * This function will log any object T sent to it with tag.
 * @params
 * 		string tag: The prefix to log object.
 * 		T obj: General template for object to be logged.
 */
template <typename T>
void Controller::log(const string tag, const T obj) {
	cout << "JacobianOS [Debug Log] @ t = " << clock() 
		<< ": " << tag << ": " << obj << endl;
	return;
}

/**
 * This function is called automatically when a new controller is constructed.
 */
bool Controller::init(void) {
	if (wiringPiSetup() != 0) 
		return false;
	return true;
}

/**
 * This function will return the current name of the controller.
 * @return
 * 		The string that contains the name of the controller.
 */
string Controller::getName(void) {
	return this->name;
}

/**
 * This function will set the new name of the controller.
 * @params
 * 		string name: The name of the controller. 
 */
void Controller::setName(string name) {
	this->name = name;
	return;
}

/**
 * This is the implementation of the Controller constructor.
 * It will automatically initialize all GPIO pins, sets the name of the controller, and begins
 * the command listener thread.
 * @params
 * 		string name: The name of the controller.
 */
Controller::Controller(string name) {
	if(!init()) {
		log("FATAL", "JacobianOS has encountered an error. See log.txt");
		exit(-1);
	}
	setName(name);
	thread l(readCommand);
	this->listener = l;
	log("Success", "JacobianOS successfully initialized.");
}

/**
 * This function will return the pin ID of the current active pinouts by name.
 * @params
 * 		string pinName: The name of the pin you are searching for.
 * @return
 * 		The integer pin ID from name if it exists, otherwise -1.
 */
int Controller::returnPinFromName(string pinName) {
	for(int i = 0; i < this->pinout.size(); i++) {
		if(this->pinout[i].first == pinName)
			return this->pinout[i].second;
	}
	log("Error", "No pin was found with this name: " + pinName);
	return -1;
}

/**
 * This function will set the pin mode (see WiringPI pinMode for specfic modes) of the pin by name, if it exists.
 * @params
 * 		string pinName: The name of the pin you are setting the mode for.
 * 		int mode: The mode you would like to set.
 */
void Controller::setPinMode(string pinName, int mode) {
	int pin = returnPinFromName(pinName);
	if(pin == -1) return;
	pinMode(pin, mode);
	return; 
}

/**
 * This function will set the pin pull up resistor (see WiringPi pud for specific states) of the pin by name, if it exists.
 * @params
 * 		string pinName: The name of the pin you are setting the pull up for.
 * 		int pud: The pull up mode you are setting it to.
 */
void Controller::setPinPud(string pinName, int pud) {
	int pin = returnPinFromName(pinName);
	if(pin == -1) return;
	pullUpDnControl(pin, pud);
	return;
}

/**
 * This function will read the digital value of the current pin by name, if it exists.
 * @params
 * 		string pinName: The name of the pin.
 * @return
 * 		The value of the pin.
 */
int Controller::readPin(string pinName) {
	int pin = returnPinFromName(pinName);
	if(pin == -1) return -1;
	return digitalRead(pin);
}

/**
 *
 * 
 * 
 * 
 */
void Controller::readCommand() {
	string command;
	getline(cin, command);
	cin.ignore();
	cout << command << endl;
}

/**
 * This function will set the digital value of the pin by name, if it exists.
 * @params
 * 		string pinName: The name of the pin.
 * 		int value: The digital value.
 */
void Controller::setPin(string pinName, int value) {
	int pin = returnPinFromName(pinName);
	if(pin == -1) return;
	digitalWrite(pin, value);
	return;
}

/**
 * This function will add and configure a new pinout on the controller.
 * You must create a pin with this function in order to edit properties of the pin.
 * @params
 * 		int id: The ID (location) of the pin (use $ gpio readall to find correct ID).
 * 		string name: The name of the pin you can use as a unique reference.
 * 		int mode: The default mode of the pin.
 * 		int pud: The default pull resistor mode of the pin.
 */
void Controller::configurePin(int id, string pinName, int mode, int pud) {
	pair<string, int> pin = make_pair(pinName, id);
	this->pinout.push_back(pin);
	setPinMode(pinName, mode);
	setPinPud(pinName, pud);
	log("Success", "A new pin has been configured! Pin ID: " + to_string(id) + ", pinName: " + pinName + ".");
	return;
}

/**
 * This method should be called to kill all processes of Jacobian after use.
 */
void Controller::kill(void) {
	this->listener.join();
	return;
}
