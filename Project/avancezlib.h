#pragma once

#include "SDL.h"
#include "SDL_ttf.h"

enum class DIRECTION { LEFT, RIGHT, UP, DOWN, NONE};
enum class DIRECTION_AXIS { HORIZONTAL, VERTICAL, BOTH};

class Sprite
{
	SDL_Renderer* renderer;
	SDL_Texture* texture;

public:

	Sprite(SDL_Renderer* renderer, SDL_Texture* texture);

	// Destroys the sprite instance
	void destroy();

	// Draw the sprite at the given position.
	// Valid coordinates are between (0,0) (upper left) and (width-32, height-32) (lower right).
	// (All sprites are 32*32 pixels, clipping is not supported)
	void draw(int x, int y);
};


class AvancezLib
{
public:
	// Destroys the avancez library instance
	void destroy();

	// Destroys the avancez library instance and exits
	void quit();

	// Creates the main window. Returns true on success.
	bool init(int width, int height);

	// Clears the screen and draws all sprites and texts which have been drawn
	// since the last update call.
	// If update returns false, the application should terminate.
	void processInput();

	void swapBuffers();

	void clearWindow();

	// Create a sprite given a string.
	// All sprites are 32*32 pixels.
	Sprite* createSprite(const char* name);

	// Draws the given text.
	void drawText(int x, int y, const char* msg);

	// Return the total time spent in the game, in seconds.
	float getElapsedTime();

	struct KeyStatus
	{
		bool pump; // space
		bool left; // left arrow
		bool right; // right arrow
		bool up;
		bool down;
		bool esc; // escape button
	};

	// Returns the keyboard status. If a flag is set, the corresponding key is being held down.
	void getKeyStatus(KeyStatus& keys);

private:
	SDL_Window* window;
	SDL_Renderer* renderer;

	TTF_Font* font;

	KeyStatus key;
};

