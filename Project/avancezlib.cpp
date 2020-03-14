#include "avancezlib.h"
#include <iostream>

using namespace std;
// Creates the main window. Returns true on success.
bool AvancezLib::init(int width, int height)
{
	SDL_Log("Initializing the engine...\n");

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL failed the initialization: %s\n", SDL_GetError());
		return false;
	}

	//Create window
	window = SDL_CreateWindow("Dig Dug", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	//Create renderer for window
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	TTF_Init();
	font = TTF_OpenFont("data/fonts/space_invaders.ttf", 12); //this opens a font style and sets a size
	if (font == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "font cannot be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// initialize the keys
	key.space = false;	key.left = false; key.right = false; key.up = false; key.down = false; key.esc = false, key.enter = false;

	//Initialize renderer color
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	//Clear screen
	SDL_RenderClear(renderer);

	SDL_Log("Engine up and running...\n");
	return true;
}
 

// Destroys the avancez library instance
void AvancezLib::destroy()
{
	SDL_Log("Shutting down the engine\n");

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_CloseFont(font);

	TTF_Quit();
	SDL_Quit();
}

void AvancezLib::quit() {
	destroy();
	exit(0);
}


void AvancezLib::processInput()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			case SDLK_q:
				key.esc = true;
				break;
			case SDLK_SPACE:
				key.space = true;
				break;
			case SDLK_a:
			case SDLK_LEFT:
				key.left = true;
				break;
			case SDLK_d:
			case SDLK_RIGHT:
				key.right = true;
				break;
			case SDLK_w:
			case SDLK_UP:
				key.up= true;
				break;
			case SDLK_s:
			case SDLK_DOWN:
				key.down = true;
				break;
			case SDLK_RETURN:
				key.enter = true;
				break;
			}
		}

		if (event.type == SDL_KEYUP)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_SPACE:
				key.space = false;
				break;
			case SDLK_a:
			case SDLK_LEFT:
				key.left = false;
				break;
			case SDLK_d:
			case SDLK_RIGHT:
				key.right = false;
				break;
			case SDLK_w:
			case SDLK_UP:
				key.up = false;
				break;
			case SDLK_s:
			case SDLK_DOWN:
				key.down = false;
				break;
			case SDLK_RETURN:
				key.enter = false;
				break;
			}
		}
	}
}

void AvancezLib::swapBuffers() {
	//Update screen
	SDL_RenderPresent(renderer);
}

void AvancezLib::clearWindow() {
	//Clear screen
	SDL_RenderClear(renderer);
}


Sprite* AvancezLib::createSprite(const char* path)
{
	//SDL_Surface* surf = SDL_LoadBMP(path);
	SDL_Surface* surf = IMG_Load(path);

	if (surf == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load image %s! SDL_image Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//Create texture from surface pixels
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//Get rid of old loaded surface
	SDL_FreeSurface(surf);

	Sprite* sprite = new Sprite(renderer, texture);

	return sprite;
}

void AvancezLib::drawText(int x, int y, const char* msg, SDL_Color color, int size)
{
	
	TTF_CloseFont(font);
	font = TTF_OpenFont("data/fonts/PressStart2P.ttf", size); //this opens a font style and sets a size

	SDL_Surface* surface = TTF_RenderText_Solid(font, msg, color); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

	SDL_Texture* msg_texture = SDL_CreateTextureFromSurface(renderer, surface); //now you can convert it into a texture

	int w = 0;
	int h = 0;
	SDL_QueryTexture(msg_texture, NULL, NULL, &w, &h);
	SDL_Rect dst_rect = { x, y, w, h };

	SDL_RenderCopy(renderer, msg_texture, NULL, &dst_rect);

	SDL_DestroyTexture(msg_texture);
	SDL_FreeSurface(surface);
}

float AvancezLib::getElapsedTime()
{
	return SDL_GetTicks() / 1000.f;
}

void AvancezLib::getKeyStatus(KeyStatus& keys)
{
	keys = key;
}


Sprite::Sprite(SDL_Renderer* renderer, SDL_Texture* texture)
{
	this->renderer = renderer;
	this->texture = texture;
}

void Sprite::draw(int x, int y) {
	draw(x, y, 0, NULL, SDL_FLIP_NONE);
}
void Sprite::draw(int x, int y, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Rect rect;

	rect.x = x;
	rect.y = y;

	SDL_QueryTexture(texture, NULL, NULL, &(rect.w), &(rect.h));

	//Render texture to screen
	//SDL_RenderCopy(renderer, texture, NULL, &rect);
	SDL_Point center_new{ 16, 16 };
	SDL_RenderCopyEx(renderer, texture, NULL, &rect, angle, &center_new, flip);
}


void Sprite::destroy()
{
	SDL_DestroyTexture(texture);
}

unsigned int Sprite::getImageWidth() {
	SDL_Rect rect;
	SDL_QueryTexture(texture, NULL, NULL, &(rect.w), &(rect.h));
	return rect.w;
}

unsigned int Sprite::getImageHeight() {
	SDL_Rect rect;
	SDL_QueryTexture(texture, NULL, NULL, &(rect.w), &(rect.h));
	return rect.h;
}

