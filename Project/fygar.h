#pragma once

// the main player
class Fygar : public GameObject
{
public:

	int lives;

	virtual ~Fygar() { SDL_Log("Fygar::~Fygar"); }

	virtual void Init(int x, int y)
	{
		SDL_Log("Fygar::Init");
		GameObject::Init();
		direction = DIRECTION::RIGHT;
		position.x = x;
		position.y = y;
	}
	virtual void InitPos(int x, int y) {
		SDL_Log("Fygar::InitPos");
		GameObject::InitPos(x, y);
	}

	virtual void Receive(Message m)
	{
		if (m == HIT)
		{
			SDL_Log("Fygar::Hit!");
			enabled = false;
		}
		if (m == WALL) {
			switch (direction)
			{
			case DIRECTION::LEFT:
				position.x += 1;
				direction = DIRECTION::UP;
				break;
			case DIRECTION::RIGHT:
				position.x -= 1;
				direction = DIRECTION::DOWN;
				break;
			case DIRECTION::UP:
				position.y += 1;
				direction = DIRECTION::RIGHT;
				break;
			case DIRECTION::DOWN:
				position.y -= 1;
				direction = DIRECTION::LEFT;
				break;
			case DIRECTION::NONE:
				break;
			default:
				break;
			}
		}
	}

	void RemoveLife()
	{
		lives--;
		SDL_Log("remaining lives %d", lives);
	}
};

class FygarBehaviourComponent : public Component
{
	float last_time_fire;	// time from the last time the fire button was pressed
	ObjectPool<Rock>* bombs_pool;
public:
	virtual ~FygarBehaviourComponent() {}

	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, ObjectPool<Rock>* bombs_pool)
	{
		Component::Create(engine, go, game_objects);
		this->bombs_pool = bombs_pool;
	}

	virtual void Init()
	{
		last_time_fire = 0;
	}

	virtual void Update(float dt)
	{


		Move(dt * POOKAH_SPEED);


		if (CanFire())
		{
			// fetches a rocket from the pool and use it in game_objects
			Bomb* bomb = bombs_pool->FirstAvailable();
			if (bomb != NULL)	// rocket is NULL is the object pool can not provide an object
			{
				bomb->Init(go->position.x, go->position.y + 32);
				game_objects->insert(bomb);
			}
			//((Pookah*)go)->fire = false;
		}

	}


	// move the Fygar left or right, depending on direction
	void Move(float move)
	{
		if (move > CELL_SIZE / 2)
			move = CELL_SIZE / 2;
		
		switch (go->direction)
		{
		case DIRECTION::LEFT:
			go->position.x -= move;
			break;
		case DIRECTION::RIGHT:
			go->position.x += move;
			break;
		case DIRECTION::UP:
			go->position.y -= move;
			break;
		case DIRECTION::DOWN:
			go->position.y += move;
			break;
		case DIRECTION::NONE:
			break;
		default:
			break;
		}
		
		
		if (go->position.x + go->dimensions.x > SCREEN_WIDTH) {
			go->position.x = SCREEN_WIDTH - go->dimensions.x;
			go->direction = DIRECTION::LEFT;

		}
		else if (go->position.x < 0) {
			go->position.x = 0;
			go->direction = DIRECTION::RIGHT;
		}
		if (go->position.y + go->dimensions.y > SCREEN_HEIGHT - CELL_SIZE) {
			go->position.y = SCREEN_HEIGHT - CELL_SIZE - go->dimensions.y;
			go->direction = DIRECTION::UP;
		}
		else if (go->position.y < CELL_SIZE * 2) {
			go->position.y = CELL_SIZE * 2;
			go->direction = DIRECTION::DOWN;
		}
		
	}

	// return true if enough time has passed from the previous bomb
	bool CanFire()
	{
		return false;
	}
};
