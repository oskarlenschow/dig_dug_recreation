#pragma once

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
		go->position.x = 320;
		go->position.y = 480/2;
	}

	virtual void Update(float dt)
	{
		AvancezLib::KeyStatus keys;
		engine->getKeyStatus(keys);
		float speed = dt * PLAYER_SPEED;
		if (keys.right || keys.left) {
			if (keys.right) {
				go->direction = DIRECTION::RIGHT;
				Move(speed);
			}
			if (keys.left) {
				go->direction = DIRECTION::LEFT;
				Move(speed);
			}
		}
		else if (keys.up || keys.down) {
			if (keys.up) {
				go->direction = DIRECTION::UP;
				Move(speed);
			}
			if (keys.down) {
				go->direction = DIRECTION::DOWN;
				Move(speed);
			}
		}
		if (keys.pump)
		{
			if (CanPump())
			{
				
			}
		}
	}


	// move the player
	// param speed depends on the time, so the player moves always at the same speed on any computer
	void Move(float speed)
	{

		switch (go->direction)
		{
		case DIRECTION::LEFT:
			go->position.x -= speed;
			break;
		case DIRECTION::RIGHT:
			go->position.x += speed;
			break;
		case DIRECTION::UP:
			go->position.y -= speed;
			break;
		case DIRECTION::DOWN:
			go->position.y += speed;
			break;
		default:
			break;
		}

		if (go->position.x > (640 - 32))
			go->position.x = 640 - 32;
			
		if (go->position.x < 0)
			go->position.x = 0;
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
	}

	virtual void Receive(Message m) 
	{
		if (m == HIT)
		{ 
			SDL_Log("Player::Hit!");
			RemoveLife();

			if (lives < 0)
				Send(GAME_OVER);
		}
	}

	void RemoveLife()
	{
		lives--;
		SDL_Log("remaining lives %d", lives);
	}
};
