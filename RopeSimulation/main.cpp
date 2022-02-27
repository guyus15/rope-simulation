#include <SDL.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720
#define FPS 60

const int stabilityLoops = 5;
const float bounce = 0.999;
const float gravity = 0.5;
const float friction = 0.999;

struct Point {
	float x;
	float y;
	float oldX;
	float oldY;
	bool shouldMove;
};

struct Stick {
	Point& point1;
	Point& point2;
	float length;
	bool shouldRender;
	SDL_Color colour;
};

void updatePoints(Point points[], const int size) {
	for (int i = 0; i < size; i++) {

		if (!points[i].shouldMove) {
			continue;
		}

		float deltaX = (points[i].x - points[i].oldX) * friction;
		float deltaY = (points[i].y - points[i].oldY) * friction;

		points[i].oldX = points[i].x;
		points[i].oldY = points[i].y;

		points[i].x += deltaX;
		points[i].y += deltaY;
		points[i].y += gravity;
	}
}

void updateSticks(Stick sticks[], const int size) {
	for (int i = 0; i < size; i++) {
		float deltaX = sticks[i].point2.x - sticks[i].point1.x;
		float deltaY = sticks[i].point2.y - sticks[i].point1.y;
		float distance = sqrt(pow(deltaX, 2) + pow(deltaY, 2));
		float difference = sticks[i].length - distance;
		float percent = difference / distance / 2;

		float offsetX = deltaX * percent;
		float offsetY = deltaY * percent;

		if (sticks[i].point1.shouldMove) {
			sticks[i].point1.x -= offsetX;
			sticks[i].point1.y -= offsetY;
		}

		if (sticks[i].point2.shouldMove) {
			sticks[i].point2.x += offsetX;
			sticks[i].point2.y += offsetY;
		}
	}
}

void constrainPoints(Point points[], const int size) {
	for (int i = 0; i < size; i++) {

		if (!points[i].shouldMove) {
			continue;
		}

		float deltaX = (points[i].x - points[i].oldX) * friction;
		float deltaY = (points[i].y - points[i].oldY) * friction;

		if (points[i].x > SCREEN_WIDTH) {
			points[i].x = SCREEN_WIDTH;
			points[i].oldX = points[i].x + deltaX * bounce;
		}
		else if (points[i].x < 0) {
			points[i].x = 0;
			points[i].oldX = points[i].x + deltaX * bounce;
		}
		else if (points[i].y > SCREEN_HEIGHT) {
			points[i].y = SCREEN_HEIGHT;
			points[i].oldY = points[i].y + deltaY * bounce;
		}
		else if (points[i].y < 0) {
			points[i].y = 0;
			points[i].oldY = points[i].y + deltaY * bounce;
		}
	}
}

void renderPoints(SDL_Renderer* renderer, const Point points[], const int size) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

	for (int i = 0; i < size; i++) {
		SDL_RenderDrawPointF(
			renderer,
			points[i].x,
			points[i].y
		);
	}
}

void renderSticks(SDL_Renderer* renderer, const Stick sticks[], const int size) {
	for (int i = 0; i < size; i++) {

		SDL_Color c = sticks[i].colour;

		if (c.r > 0 || c.g > 0 || c.b > 0) {
			SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
		}
		else {
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		}

		if (!sticks[i].shouldRender) {
			return;
		}

		SDL_RenderDrawLine(
			renderer,
			sticks[i].point1.x,
			sticks[i].point1.y,
			sticks[i].point2.x,
			sticks[i].point2.y
		);
	}
}

float distanceBetweenPoints(const Point& point1, const Point& point2) {
	float deltaX = point1.x - point2.x;
	float deltaY = point1.y - point2.y;

	return sqrt(pow(deltaX, 2) + pow(deltaY, 2));
}

int main(int argc, char* argv[]) {

	// Random seeding from time
	srand(time(NULL));

	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		std::cout << "There has been an error initialising SDL:\n" << SDL_GetError() << std::endl;
		return -1;
	}

	atexit(SDL_Quit);

	SDL_Window* window = SDL_CreateWindow(
		"Rope Simulation",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		NULL
	);
	
	if (window == nullptr) {
		std::cout << "There has been an error creating the window:\n" << SDL_GetError() << std::endl;
		return -1;
	}
	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, NULL);

	if (renderer == nullptr) {
		std::cout << "There has been an error creating the renderer:\n" << SDL_GetError() << std::endl;
		return -1;
	}

	// Define a set of points
	Point points[] = {
		{SCREEN_WIDTH / 2, 50, SCREEN_WIDTH / 2, 50, false},
		{540, 50, 540, 50, false},
		{500, 100, 500, 100, true},
		{200, 125, 200, 125, true},
		{350, 175, 250, 175, true},
		{450, 175, 450, 175, true},
		{450, 275, 450, 275, true},
		{350, 275, 300, 240, true}
	};
	int pointSize = 8;

	// Define a set of sticks
	Stick sticks[] = {
		{points[0], points[1], distanceBetweenPoints(points[0], points[1]), true},
		{points[1], points[2], distanceBetweenPoints(points[1], points[2]), true},
		{points[2], points[3], distanceBetweenPoints(points[2], points[3]), true},
		{points[3], points[4], distanceBetweenPoints(points[3], points[4]), true},
		{points[4], points[5], distanceBetweenPoints(points[4], points[5]), true},
		{points[5], points[6], distanceBetweenPoints(points[5], points[6]), true},
		{points[6], points[7], distanceBetweenPoints(points[6], points[7]), true},
		{points[7], points[4], distanceBetweenPoints(points[7], points[4]), true},
		{points[7], points[5], distanceBetweenPoints(points[7], points[5]), true},
	};
	int stickSize = 9;

	bool running = true;
	while (running) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

		SDL_RenderClear(renderer);

		updatePoints(points, pointSize);

		// Loop through for stick stablility
		for (int i = 0; i < stabilityLoops; i++) {
			updateSticks(sticks, stickSize);
			constrainPoints(points, pointSize);
		}

		renderSticks(renderer, sticks, stickSize);

		SDL_RenderPresent(renderer);

		SDL_Delay((int)1000 / FPS);
	}

	if (renderer) {
		SDL_DestroyRenderer(renderer);
	}

	if (window) {
		SDL_DestroyWindow(window);
	}

	SDL_Quit();

	return 0;
}