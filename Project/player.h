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

	virtual void Update(float dt)
	{
		if (go->mode != DYING) {
			AvancezLib::KeyStatus keys;
			engine->getKeyStatus(keys);
			float speed = dt * PLAYER_SPEED;

			if (keys.space) {
				if (can_pump) {
					go->mode = ATTACKING;
					pump->Init(go->position.x, go->position.y);
					pump->direction = go->direction;
					can_pump = false;
				}
				go->moving = false;
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
				if (!can_pump) {
					can_pump = true;
					pump->enabled = false;
					pump->in_use = false;
					go->Send(PUMP_RELEASE);
				}
			}
		}
		else
			go->moving = false;
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
	}
	virtual void Receive(Message m) 
	{
		switch (m)
		{
		case HIT:
			if (mode != DYING){
				SDL_Log("Player::Hit!");
				RemoveLife();
				mode = DYING;
				direction = DIRECTION::NONE;
				RenderComponent* renderComponent = GetComponent<RenderComponent*>();
				renderComponent->SetImageIndex(0);
				renderComponent->SetImageSpeed(4.f);
				Send(PLAYER_HIT);
			}
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
		case BURST:
			if (lives < 0)
				Send(GAME_OVER);
			else
				Send(PLAYER_DEAD);
			break;
		default:
			break;
		}
	}
	string GetName() {
		return "player";
	}
	void ResetPosition() {
		GameObject::ResetPosition();
		direction = DIRECTION::RIGHT;
		mode = IDLE;
	}
	void RemoveLife()
	{
		lives--;
		SDL_Log("remaining lives %d", lives);
	}
};
