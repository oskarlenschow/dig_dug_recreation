#pragma once

// the main player
class Alien : public GameObject
{
public:

	int lives;	// it's game over when goes below zero 

	int direction;

	bool fire;

	virtual ~Alien() { SDL_Log("Alien::~Alien"); }

	virtual void Init(int x, int y)
	{
		SDL_Log("Alien::Init");
		GameObject::Init();
		direction = 1;
		position.x = x;
		position.y = y;
		fire = false;
	}
	virtual void InitPos(int x, int y) {
		SDL_Log("Alien::InitPos");
		GameObject::InitPos(x, y);
	}

	virtual void Receive(Message m)
	{
		if (m == HIT)
		{
			SDL_Log("Alien::Hit!");
			enabled = false;
		}
		if (m == WALL) {
			direction *= -1;
			position.y += 32;
		}
	}

	void RemoveLife()
	{
		lives--;
		SDL_Log("remaining lives %d", lives);
	}
};

class AlienBehaviourComponent : public Component
{
	float last_time_fire;	// time from the last time the fire button was pressed
	ObjectPool<Bomb>* bombs_pool;
public:
	virtual ~AlienBehaviourComponent() {}

	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, ObjectPool<Bomb>* bombs_pool)
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
		Move(dt * ALIEN_SPEED * ((Alien*) go)->direction);
	
		if (CanFire())
		{
			// fetches a rocket from the pool and use it in game_objects
			Bomb* bomb = bombs_pool->FirstAvailable();
			if (bomb != NULL)	// rocket is NULL is the object pool can not provide an object
			{
				bomb->Init(go->position.x, go->position.y + 32);
				game_objects->insert(bomb);
			}
			((Alien*)go)->fire = false;
		}
		
	}


	// move the alien left or right, depending on direction
	void Move(float move)
	{
		go->position.x += move;

		if (go->position.x > (640 - 32)){
			go->position.x = 640 - 32;
			go->Send(WALL);
		}
		else if (go->position.x < 0){
			go->position.x = 0;
			go->Send(WALL);
		}
		if (go->position.y > 480 - 16) {
			go->Send(GAME_OVER);
		}
		
	}

	// return true if enough time has passed from the previous bomb
	bool CanFire()
	{
		return ((Alien*)go)->fire;
	}
};
