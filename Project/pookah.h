#pragma once

// the main player
class Pookah : public GameObject
{
public:

	virtual ~Pookah() { SDL_Log("Pookah::~Pookah"); }

	virtual void Init(int x, int y)
	{
		SDL_Log("Pookah::Init");
		GameObject::Init(x, y);
		direction = DIRECTION::RIGHT;
	}
	virtual void InitPos(int x, int y) {
		SDL_Log("Pookah::InitPos");
		GameObject::InitPos(x, y);
	}

	virtual void Receive(Message m)
	{
		if (m == HIT)
		{
			SDL_Log("Pookah::Hit!");
			enabled = false;
		}
	}
};

class PookahBehaviourComponent : public Component
{
public:
	virtual ~PookahBehaviourComponent() {}

	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects)
	{
		Component::Create(engine, go, game_objects);
	}

	virtual void Init()
	{
	}

	virtual void Update(float dt)
	{
		Move(dt * POOKAH_SPEED);	
	}


	// move the Pookah left or right, depending on direction
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
};
