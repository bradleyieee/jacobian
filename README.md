# Introduction
The Bradley IEEE is creating a self-driving RC car using OpenCV, this software, and a modded Traxxas chassis with a Raspberry PI and a 2 by 1 Mux.

# What is "Jacobian"?
The Jacobian library (written in and for C++ developers) is an abstraction layer for configuing and utilizing a Raspberry Pi's GPIO pinouts. 

# What is "JacobianOS"?
This software implements the utilities of the Jacobian library on a Raspberry Pi at the lowest level above the hardware of the Bradley IEEE self-driving RC car. It serves to decompose high level vector input into two PWM signals which are generated using the Pi system clock as well
as physically delivered out of the configured GPIO pins. It is also an interface to communicate to the car via console commands and 
JacobianOS Routine Scripts (*.jors), which specify sequences of timed commands to translate the car. Find a list of valid commands below.

    Compilation: $ g++ ../jacobian.cpp jacobianos.cpp -o build -lwiringPi -pthread

    Running: $ ./build

`[Command ready]: help (no args)`: General help command. Use when the format of commands is forgotten.

`[Command ready]: log (no args)`: This will toggle the debug command logging.

`[Command ready]: load (path_to_routine)`: Load a *.jors file to automate commands. JORS documentation outlined below.

`[Command ready]: stop (no args)`: Terminate JacobianOS. This must be called when exiting the application.

`[Command ready]: drive ('f' or 'b', %_max_speed)`: Translate the car forwards ('f') or backwards ('b') at a specified percentage of max speed.

`[Command ready]: steer (pulse_width_in_milliseconds * 1000)`: Steer the car to the left (2000) or the right (1200) and everything in between these two numbers.

`[Command ready]: break (no args)`: Stop the car from translating immediately.

`[Command ready]: override (0 or 1)`: Set the manual override true or false with software. If overridden, the physical controller of the RC car will control its movement.

# What is "JacobianOS Routine Script (*.jors)"?
This custom high-level programming language serves to describe the behavior of the Bradley IEEE self-driving car over time. It is written in a linear, time dependent syntax, which will be described below. There are currently only a few simple commands, but over time the language capability will be expanded as new behaviors require more complex description. Find a list of current commands below.

NOTE: Please enter commands and arguments with single spaces in between, no commas or other delimiters.

  `drive ('f' or 'b', %_max_speed)`: Translate the car forwards ('f') or backwards ('b') at a specified percentage of max speed.
  
  `steer (pulse_width_in_milliseconds * 1000)`: Steer the car to the left (2000) or the right (1200) and everything in between these two numbers.
  
  `wait (time_in_seconds)`: Pause the routine at specified point and while the car continues its current state.
  
  `break (no_args)`: Stop the car from translating immediately.
  
  `log (message)`: Print the contents of the line after log to the console while the routine runs.
  
-- Example --

~~~
log Example routine.

log Drive forward for five seconds...
drive f 23
wait 5.0

log Break for two seconds...
break
wait 2.0

log Reverse for four and a half seconds...
drive b 30
wait 4.5

log Steer to left...
steer 1200
wait 1.0

log Steer to right...
steer 2000
wait 2.0

log Steer center and break
steer 1600
break

log Drive forwards for 3.325 seconds...
drive f 23
wait 3.325

log Final break for two seconds...
break
wait 2.00
~~~

  
