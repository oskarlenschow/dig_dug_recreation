#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "avancezlib.h"
#include "object_pool.h"

#include <set>

const unsigned int	NUM_LIVES = 2;

const unsigned int	MAX_NUM_ROCKETS = 32;
const unsigned int	MAX_NUM_BOMBS = 32;
const unsigned int	POINTS_PER_ALIEN = 100;


//Used for indexing sprite arrays for all mobs, including player.
const unsigned int WALKING = 0;		
const unsigned int ATTACKING = 1;
const unsigned int DIGGING = 2;
const unsigned int IDLE = 3;


const unsigned int	CELL_SIZE = 32;
const unsigned int  SCREEN_WIDTH = CELL_SIZE * 14;
const unsigned int	SCREEN_HEIGHT = CELL_SIZE * 18;

const float			FIRE_TIME_INTERVAL = .5f;
const float			BOMB_TIME_INTERVAL = 1.25f;
const float			PLAYER_SPEED = 70.0f;
const float			ROCKET_SPEED = 160.0f;
	  float			POOKAH_SPEED = 80.0f;
      float			FYGAR_SPEED = 80.0f;
const float			BOMB_SPEED = 120.0f;


float game_speed = 1.f;		// speed of the game; it is increased each time all the aliens are hit
							// it is also the score multiplier


#include "component.h"
#include "game_object.h"


#include "rocket.h"
#include "bomb.h"
#include "player.h"
#include "pookah.h"
#include "fygar.h"
#include "grid.h"

#include "game.h"


int main(int argc, char** argv)
{

	AvancezLib engine;

	engine.init(SCREEN_WIDTH, SCREEN_HEIGHT);

	Game game;
	game.Create(&engine);
	game.Init();

	float lastTime = engine.getElapsedTime();
	while (true)
	{
		float newTime = engine.getElapsedTime();
		float dt = newTime - lastTime;
		dt = dt * game_speed;
		lastTime = newTime;

		engine.processInput();
		game.Update(dt);
		game.Draw();
	}

	// clean up
	game.Destroy();
	engine.destroy();

	return 0;
}



