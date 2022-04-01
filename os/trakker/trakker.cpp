/**
 * Trakker is an external software utility included in JacobianOS that allows real-time control of the 
 * Bradley IEEE car using a vector system. When the external window gets touched, or clicked, a line is 
 * drawn from the center of the screen to the cursor postion. Red and blue lines will be drawn along the 
 * axis indicating the the magnitude of each component. The Y axis controls the drive speed of the car, 
 * the X axis controls the steer of the car. This tool can be used to also crunch high level vector input 
 * into pulse width times for each channel.
 * 
 * [NOTE]: Trakker, in this current version, is not applied in the context of a running JacobianOS thread.
 * That will be added very soon.
 * 
 * @author Ian Wilkey (iwilkey)
 * @since Jacobian 1.4.0, JacobianOS 1.1.0
 * @version 1.0.0
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <utility>
#include <SDL2/SDL.h>
using namespace std;

// Constant application variables...
const char * TITLE = "Trakker [JacobianOS Utility]";
const int SCREEN_WIDTH = 800,
	SCREEN_HEIGHT = SCREEN_WIDTH,
	PADDING = 50;
const float FPS = 60.0f;

/**
 * Crunch the cursor position into pulse width times for each channel.
 * 
 * @params
 * 	pair<int, int> in: The current cursor position.
 */
void simulate(pair<int, int> in) {
	float percentageX = ((float)((SCREEN_WIDTH / 2) - in.first) / ((SCREEN_WIDTH / 2) - PADDING)) * 100.0f;
	percentageX = (percentageX > 100) ? 100.0f : percentageX;
	percentageX = (percentageX < -100) ? -100.0f : percentageX;
	float percentageY = ((float)((SCREEN_HEIGHT / 2) - in.second) / ((SCREEN_HEIGHT / 2) - PADDING)) * 100.0f;
	percentageY = (percentageY > 100) ? 100.0f : percentageY;
	percentageY = (percentageY < -100) ? -100.0f : percentageY;
	float driveTime = 1.5f, steerTime = 1.6f;
	driveTime = ((2.0f - 1.5f) * (percentageY / 100.0f)) + 1.5f;
	steerTime = ((2.0f - 1.6f) * (percentageX / 100.0f)) + 1.6f;
	return;
}

// Main method begins here. This application is built on top of SDL.
int main(int argc, char ** args) {

	// Init SDL... Standard stuff.
	SDL_Window * window = nullptr;
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		cout << "SDL could not be initialized. See error below for more detail." << endl;
		cerr << SDL_GetError() << endl;
		return -1;
	}
	window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if(window == nullptr) {
		cout << "SDL window could not be initialized. See error below for more detail." << endl;
		cerr << SDL_GetError() << endl;
		return -1;
	}
	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);

	// SIMULATION VARS
	bool tracking = false;
	pair<int, int> curs = make_pair(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	// END SIMULATION VARS

	// Main loop...
	bool running = true;
	while(running) {
		// Synchronize loop time to frame rate...
		static unsigned long long int micros;
		micros = (unsigned long long int)((1 / FPS) * 1000000L);
		this_thread::sleep_for(chrono::microseconds(micros));

		// Poll events start...
		static SDL_Event e;
		SDL_PollEvent(&e);

		e.motion.x = (e.motion.x < PADDING) ? PADDING : e.motion.x;
		e.motion.x = (e.motion.x > SCREEN_WIDTH - PADDING) ? SCREEN_WIDTH - PADDING : e.motion.x;
		e.motion.y = (e.motion.y < PADDING) ? PADDING : e.motion.y;
		e.motion.y = (e.motion.y > SCREEN_WIDTH - PADDING) ? SCREEN_WIDTH - PADDING : e.motion.y;

		switch(e.type) {
			case SDL_QUIT:
				running = false;
				break;

			case SDL_MOUSEBUTTONDOWN:
				curs.first = e.motion.x;
				curs.second = e.motion.y;
				tracking = true;
				break;

			case SDL_MOUSEBUTTONUP:
				curs.first = SCREEN_WIDTH / 2;
				curs.second = SCREEN_HEIGHT / 2;
				tracking = false;
				break;

			case SDL_MOUSEMOTION:
				if(tracking) {
					curs.first = e.motion.x;
					curs.second = e.motion.y;
				}
				break;
		}
		// Poll events end.

		// Logic start...
		simulate(curs);
		// End logic.

		// Rendering start...
		SDL_SetRenderDrawColor(renderer, 244, 244, 244, 255);
        SDL_RenderClear(renderer);

        	// Draw grid.
        	SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
	        SDL_RenderDrawLine(renderer, SCREEN_WIDTH / 2, PADDING, SCREEN_WIDTH / 2, SCREEN_HEIGHT - PADDING);
	        SDL_RenderDrawLine(renderer, PADDING, SCREEN_HEIGHT / 2, SCREEN_WIDTH - PADDING, SCREEN_HEIGHT / 2);

	        // Draw vector and components...
	        if(tracking) {
        		// Cursor line.
		        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		        SDL_RenderDrawLine(renderer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, curs.first, curs.second);
		        // X...
		        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		        SDL_RenderDrawLine(renderer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, curs.first, SCREEN_HEIGHT / 2);
		        // Y...
		        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		        SDL_RenderDrawLine(renderer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2, curs.second);
		    }

		SDL_RenderPresent(renderer);
		// Rendering end.
	}

	// Dispose
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
