#include "jacobian.hpp"
#include <time.h>
#include <wiringPi.h>
using namespace Jacobian;

/*
* Software delay at clock speed of specific device.
* @perams
* 		double s: The time (in seconds) the delay should wait.
* @return
* 		void
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
 * @return
 * 		void
 */
template <typename T>
void Controller::log(const string tag, const T obj) {
	cout << "JacobianOS [Debug Log] @ t = " << clock() 
		<< ": " << tag << ": " << obj << endl;
	return;
}

bool Controller::init(void) {
	if (wiringPiSetup() != 0) 
		return false;
	return true;
}

string Controller::getName(void) {
	return this->name;
}

void Controller::setName(string name) {
	this->name = name;
	return;
}

Controller::Controller(string name) {
	if(!init()) {
		log("FATAL", "JacobianOS has encountered an error. See log.txt");
		exit(-1);
	}
	setName(name);
	log("Success", "JacobianOS successfully initialized.");
}

int Controller::returnPinFromName(string pinName) {
	for(int i = 0; i < this->pinout.size(); i++) {
		if(this->pinout[i].first == pinName)
			return this->pinout[i].second;
	}
	log("Error", "No pin was found with this name: " + pinName);
	return -1;
}

void Controller::setPinMode(string pinName, int mode) {
	int pin = returnPinFromName(pinName);
	if(pin == -1) return;
	pinMode(pin, mode);
	return; 
}

void Controller::setPinPud(string pinName, int pud) {
	int pin = returnPinFromName(pinName);
	if(pin == -1) return;
	pullUpDnControl(pin, pud);
	return;
}

int Controller::readPin(string pinName) {
	int pin = returnPinFromName(pinName);
	if(pin == -1) return -1;
	return digitalRead(pin);
}

void Controller::readCommand(string command) {
	// TODO: Implement some way to read commands from terminal concurrently. This will
	// need a separate thread.
}

void Controller::setPin(string pinName, int value) {
	int pin = returnPinFromName(pinName);
	if(pin == -1) return;
	digitalWrite(pin, value);
	return;
}

void Controller::configurePin(int id, string pinName, int mode, int pud) {
	pair<string, int> pin = make_pair(pinName, id);
	this->pinout.push_back(pin);
	setPinMode(pinName, mode);
	setPinPud(pinName, pud);
	log("Success", "A new pin has been configured! Pin ID: " + to_string(id) + ", pinName: " + pinName + ".");
	return;
}
