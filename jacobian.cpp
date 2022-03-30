/**
 * Jacobian library implementation file.
 * 
 * @author Ian Wilkey (iwilkey)
 * @since 1.0.0
 */

#include "jacobian.h"
#include <wiringPi.h>
#include <chrono>
using namespace jacobian;

/*******************
General utilities
/*******************/

// General logging. These methods print a simple message to the terminal during runtime.
template <typename T>
void jacobian::log(string tag, T obj) {
	cout << "JacobianOS [Debug Log] @ t = " << clock() 
		<< ": [" << tag << "] " << obj << endl;
	return;
}
template <typename T>
void jacobian::log(T obj) {
	cout << "JacobianOS [Debug Log] @ t = " << clock() 
		<< ": " << obj << endl;
	return;
}

/**
 * This function translates time in seconds to duty cycle with defined frequency.
 *
 * @params
 * 	int frequency: The frequency of the signal.
 * 	float time: The time, in seconds, to be translated.
 * @return the time as a percentage of period. 
 */
float jacobian::timeToDutyCycle(int frequency, float time) {
	if(time <= 0.0f) return 0.0f;
	float period = 1 / (double)frequency;
	if(time >= period) return 100.0f;
	return (time / period) * 100.0f;
}

/**
 * Shift the decimal point of si by shift digits.
 * 
 * @params
 * 	float si: The original number, preferred in it's deka state.
 * 	int shift: The amount to shift the decimal.
 * @return the shifted number.
 */
float jacobian::radixShift(float si, int shift) {
	si *= pow(10, shift);
	return si;
}
	
/**
 * Simple delay using system clock in the calling thread.
 * 
 * @params
 * 	double s: The time to wait, in seconds.
 * @return void
 */
void jacobian::waitForSeconds(double s) {
	static unsigned long long int micros;
	micros = (int)(s * 1000000);
	this_thread::sleep_for(chrono::microseconds(micros));
	return;
}

/**
 * Return a list of strings seperated from every occurence of a specified delimiter.
 * 
 * @params
 * 	string in: The string to be parsed through.
 * 	char delimiter: The char to cut the input string by.
 * @return a list of tokens in the parsed string cut by the delimiter.
 */
vector<string> jacobian::tokenize(string in, char delimiter) {
	vector<string> ret;
	stringstream check(in);
	string intermediate;
	while(getline(check, intermediate, delimiter))
		ret.push_back(intermediate);
	return ret;
}

/*******************
Controller object
/*******************/

// Controller constructor.
Controller::Controller(string name) {
	if(!init()) {
		log("FATAL", "JacobianOS has encountered an error. See log.txt");
		exit(-1);
	}
	setName(name);
	setState(true);
	log("Success", "JacobianOS successfully initialized.");
}

/**
 * This function is called automatically when a new Controller is constructed.
 * Its main purpose is to initiate wiringPI (selected GPIO library).
 */
bool Controller::init(void) {
	if (wiringPiSetup() != 0) 
		return false;
	return true;
}

/**
 * This function MUST be called after every JacobianOS program. It resets the state of the
 * configured GPIO pins to a neutral state. This is to avoid any electrical discharge
 * or damage of componets after use.
 */
void Controller::kill(void) {
	for(pair<string, int> pin : pinout) {
		setPin(pin.first, 0);
		setPinMode(pin.first, INPUT);
	}
	log("Success", "Controller terminated. It is now safe to touch the electronic components.");
	return;
}

// Return if the controller is currently overriden by manual controller.
bool Controller::isOverridden(void) {
	return this->overriden;
}

// Set the state of the Controller override.
void Controller::Override(bool verdict) {
	this->overriden = verdict;
	if(verdict) log("Success", "The Controller is no longer in control of the RC car.");
	else log("Success", "The Controller is now in direct control of the RC car.");
	return;
}

// Return the distinct name of the Controller.
string Controller::getName(void) {
	return this->name;
}

// Set the name of the Controller.
void Controller::setName(string name) {
	this->name = name;
	return;
}

// Set whether or not the Controller is on or off.
void Controller::setState(bool state) {
	this->running = state;
}

// Is the controller on or off?
bool Controller::isRunning(void) {
	return this->running;
}

/**
 * This function will return the pin ID of the current active pinouts by name.
 * 
 * @params
 * 	string pinName: The name of the pin you are searching for.
 * @return the integer pin ID from name if it exists, otherwise -1.
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
 * 
 * @params
 * 	string pinName: The name of the pin you are setting the mode for.
 * 	int mode: The mode you would like to set.
 */
void Controller::setPinMode(string pinName, int mode) {
	int pin = returnPinFromName(pinName);
	if(pin == -1) return;
	pinMode(pin, mode);
	return; 
}

/**
 * This function will set the pin pull up resistor (see WiringPi pud for specific states) of the pin by name, if it exists.
 * 
 * @params
 * 	string pinName: The name of the pin you are setting the pull up for.
 * 	int pud: The pull up mode you are setting it to.
 */
void Controller::setPinPud(string pinName, int pud) {
	int pin = returnPinFromName(pinName);
	if(pin == -1) return;
	pullUpDnControl(pin, pud);
	return;
}

/**
 * This function will read the digital value of the current pin by name, if it exists.
 * 
 * @params
 * 	string pinName: The name of the pin.
 * @return the value of the pin.
 */
int Controller::readPin(string pinName) {
	int pin = returnPinFromName(pinName);
	if(pin == -1) return -1;
	return digitalRead(pin);
}

/**
 * This function will set the digital value of the pin by name, if it exists.
 * 
 * @params
 * 	string pinName: The name of the pin.
 * 	int value: The digital value.
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
 * 
 * @params
 * 	int id: The ID (location) of the pin (use $ gpio readall to find correct ID).
 * 	string name: The name of the pin you can use as a unique reference.
 * 	int mode: The default mode of the pin.
 * 	int pud: The default pull resistor mode of the pin.
 */
void Controller::configurePin(int id, string pinName, int mode, int pud) {
	pair<string, int> pin = make_pair(pinName, id);
	this->pinout.push_back(pin);
	setPinMode(pinName, mode);
	setPinPud(pinName, pud);
	log("Success", "A new pin has been configured! Pin ID: " 
		+ to_string(id) + ", pinName: " + pinName + ".");
	return;
}
	
/*******************
Pulse Width Modulation generator
/*******************/

// PWM constructor.
PWM::PWM(int freq, double duty) {
	this->frequency = freq;
	this->dutyCycle = duty;
	precisionPeriod = (1 / (float)frequency) * (PRECISION);
}

/**
 * Set the duty cycle of the PWM signal.
 * 
 * @params
 * 	double duty: The duty cycle [0.1% - 100%].
 */
void PWM::setDutyCycle(double duty) {
	duty = (duty > 100.0f) ? 100.0f : duty;
	duty = (duty <= 0.0f) ? 0.1f : duty;
	this->dutyCycle = duty;
}

/**
 * This method must be called every undelayed loop cycle in PI main
 * loop while controller is active. This will update the state of the
 * PWM as time moves.
 */
void PWM::tick(void) {
	last = now;
	now = clock();
	delta += (now - last);

	precisionSince = delta / (CLOCKS_PER_SEC / PRECISION);
	if(precisionSince <= (precisionPeriod * (this->dutyCycle / 100.0f))) {
		on = true;
		return;
	} else if (precisionSince >= precisionPeriod)
		delta = 0;
	on = false;
}

// Evaluate the current state of the PWM signal: logic HIGH or LOW.
bool PWM::eval(void) {
	return this->on;
}
