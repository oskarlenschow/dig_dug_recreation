#pragma once

#include <iostream>

class PlayerBehaviourComponent : public Component
{
	GameObject* pump;
	bool can_pump = true;
public:
	virtual ~PlayerBehaviourComponent() {}

	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, GameObject * pump)
	{

		Component::Create(engine, go, game_objects);
		this->pump = pump;
	}

	virtual void Init(int x, int y)
	{
	}

	virtual void Update(float dt)
	{
		AvancezLib::KeyStatus keys;
		engine->getKeyStatus(keys);
		float speed = dt * PLAYER_SPEED;

		if (keys.space) {
			go->mode = ATTACKING;
			if (can_pump)
			{
				pump->Init(go->position.x, go->position.y);
				pump->direction = go->direction;
				can_pump = false;
			}
		}
		else if (keys.right || keys.left) {
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
		else {
			go->moving = false;
			go->mode = IDLE;
		}

		if (!keys.space) {
			can_pump = true;
			pump->enabled = false;
			go->Send(PUMP_RELEASE);
		}
	}
	// move the player
	void Move(float speed, DIRECTION dir)
	{
		go->moving = true;

		if (go->mode == ATTACKING || go->mode == IDLE)  //This is merely to quickly change from attacking sprite visually, instead of waiting for the grid to tell player if it should walk or dig.
			go->mode = WALKING;
			
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
				if (go->direction == DIRECTION::UP) //If axis is locked, move that axis until unlocked
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

		if (go->position.x + go->dimensions.x > SCREEN_WIDTH)
			go->position.x = SCREEN_WIDTH - go->dimensions.x;
		else if (go->position.x < 0)
			go->position.x = 0;
		if (go->position.y + go->dimensions.y > SCREEN_HEIGHT - CELL_SIZE)
			go->position.y = SCREEN_HEIGHT - CELL_SIZE - go->dimensions.y;
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

	virtual void Init(int x, int y)
	{
		SDL_Log("Player::Init");
		GameObject::Init(x, y);
		lives = NUM_LIVES;
		mode = WALKING;
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
		case DIG:
			mode = DIGGING;
			break;
		case WALK:
			mode = WALKING;
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
