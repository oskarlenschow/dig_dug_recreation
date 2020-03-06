#pragma once

#include <iostream>

class PlayerBehaviourComponent : public Component
{

public:
	virtual ~PlayerBehaviourComponent() {}

	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<Rocket> * rockets_pool)
	{
		Component::Create(engine, go, game_objects);
	}

	virtual void Init()
	{

	}

	virtual void Update(float dt)
	{
		AvancezLib::KeyStatus keys;
		engine->getKeyStatus(keys);
		float speed = dt * PLAYER_SPEED;


		if (keys.right || keys.left) {
			if (keys.right) 
				Move(speed, DIRECTION::RIGHT);
			if (keys.left) 
				Move(speed, DIRECTION::LEFT);
		}
		else if (keys.up || keys.down) {
			if (keys.up) 
				Move(speed, DIRECTION::UP);
			if (keys.down)
				Move(speed, DIRECTION::DOWN);
		}
		if (keys.pump)
		{
			if (CanPump())
			{
				
			}
		}
	}
	// move the player
	void Move(float speed, DIRECTION dir)
	{
		if (go->axis == DIRECTION_AXIS::BOTH) {
			switch (dir)
			{
			case DIRECTION::LEFT:
				go->position.x -= speed;
				go->axis = DIRECTION_AXIS::HORIZONTAL; //If moved at least one pixel, lock axis until next whole tile
				break;
			case DIRECTION::RIGHT:
				go->position.x += speed;
				go->axis = DIRECTION_AXIS::HORIZONTAL;
				break;
			case DIRECTION::UP:
				go->position.y -= speed;
				go->axis = DIRECTION_AXIS::VERTICAL;
				break;
			case DIRECTION::DOWN:
				go->position.y += speed;
				go->axis = DIRECTION_AXIS::VERTICAL;
				break;
			default:
				break;
			}
			go->direction = dir;
		}
		else if (go->axis == DIRECTION_AXIS::HORIZONTAL) {
			switch (dir)
			{
			case DIRECTION::LEFT:
				go->position.x -= speed;
				go->direction = dir;
				break;
			case DIRECTION::RIGHT:
				go->position.x += speed;
				go->direction = dir;
				break;
			case DIRECTION::UP:
				if (go->direction == DIRECTION::LEFT) //If axis is locked, move that axis until unlocked
					go->position.x -= speed;
				else if (go->direction == DIRECTION::RIGHT)
					go->position.x += speed;
				break;
			case DIRECTION::DOWN:
				if (go->direction == DIRECTION::LEFT)
					go->position.x -= speed;
				else if (go->direction == DIRECTION::RIGHT)
					go->position.x += speed;
				break;
			default:
				break;
			}
		}
		else if (go->axis == DIRECTION_AXIS::VERTICAL) {
			switch (dir)
			{
			case DIRECTION::LEFT:
				if (go->direction == DIRECTION::UP)
					go->position.y -= speed;
				else if (go->direction == DIRECTION::DOWN)
					go->position.y += speed;
				break;
			case DIRECTION::RIGHT:
				if (go->direction == DIRECTION::UP)
					go->position.y -= speed;
				else if (go->direction == DIRECTION::DOWN)
					go->position.y += speed;
				break;
			case DIRECTION::UP:
				go->position.y -= speed;
				go->direction = dir;
				break;
			case DIRECTION::DOWN:
				go->position.y += speed;
				go->direction = dir;
				break;
			default:
				break;
			}
		}

		if (go->position.x > SCREEN_WIDTH - CELL_SIZE)
			go->position.x = SCREEN_WIDTH - CELL_SIZE;
		else if (go->position.x < 0)
			go->position.x = 0;

		if (go->position.y > SCREEN_HEIGHT - CELL_SIZE * 2)
			go->position.y = SCREEN_HEIGHT - CELL_SIZE * 2;
		else if (go->position.y < CELL_SIZE * 2)
			go->position.y = CELL_SIZE * 2;
	}

	// return true if enough time has passed from the previous rocket
	bool CanPump()
	{
		SDL_Log("pump!");
		return true;
	}
};


// the main player
class Player : public GameObject
{
public:

	int lives;	// it's game over when goes below zero 

	virtual ~Player()	{		SDL_Log("Player::~Player");	}

	virtual void Init()
	{
		SDL_Log("Player::Init");
		GameObject::Init();
		lives = NUM_LIVES;
		axis = DIRECTION_AXIS::BOTH;
		direction = DIRECTION::LEFT;
		position.x = SCREEN_WIDTH - CELL_SIZE;
		position.y = CELL_SIZE * 2;
		dimensions.x = CELL_SIZE;
		dimensions.y = CELL_SIZE;
	}
	virtual void Receive(Message m) 
	{
		switch (m)
		{
		case HIT:
			SDL_Log("Player::Hit!");
			RemoveLife();
			if (lives < 0)
				Send(GAME_OVER);
			break;
		case GAME_OVER:
			break;
		case BOTH_DIRECTIONS:
			axis = DIRECTION_AXIS::BOTH;
			break;
		case WALL:
			break;
		case DIGGING:
			break;
		case NOT_DIGGING:
			break;
		default:
			break;
		}
	}

	void RemoveLife()
	{
		lives--;
		SDL_Log("remaining lives %d", lives);
	}
};
