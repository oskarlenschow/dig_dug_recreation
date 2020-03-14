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
		GameObject::Init(x, y);
		position.x = x;
		position.y = y;
	}
	virtual void InitPos(int x, int y) {
		SDL_Log("Fygar::InitPos");
		GameObject::InitPos(x, y);
	}

	virtual void Receive(Message m)
	{
		if (m == MOVING_HIT)
		{
			if (mode != CRUSHED) {
				moving = false;
				mode = CRUSHED;
				RenderComponent* renderComponent = GetComponent<RenderComponent*>();
				renderComponent->SetImageIndex(0);
				renderComponent->SetImageSpeed(0.5f);
			}
		}
		if (m == WALL) {
			switch (direction)
			{
			case DIRECTION::LEFT:
				direction = DIRECTION::UP;
				break;
			case DIRECTION::RIGHT:
				direction = DIRECTION::DOWN;
				break;
			case DIRECTION::UP:
				direction = DIRECTION::RIGHT;
				break;
			case DIRECTION::DOWN:
				direction = DIRECTION::LEFT;
				break;
			case DIRECTION::NONE:
				break;
			default:
				break;
			}
		}
		if (m == PUMP) {
			if (mode != DYING) {
				moving = false;
				mode = DYING;
				RenderComponent* renderComponent = GetComponent<RenderComponent*>();
				renderComponent->SetImageIndex(0);
				renderComponent->SetImageSpeed(2.f);
			}
		}
		if (m == PUMP_RELEASE) {
			moving = true;
			mode = WALKING;
		}
		if (m == BURST) {
			Send(FYGAR_BURST);
			mode = WALKING;
			enabled = false;
		}
		if (m == EXTINGUISH) {
			moving = true;
			mode = WALKING;
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

	float timer;	// time from the last flame
	ObjectPool<Flame>* flame_pool;
public:
	virtual ~FygarBehaviourComponent() {}

	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, ObjectPool<Flame>* flame_pool)
	{
		Component::Create(engine, go, game_objects);
		this->flame_pool = flame_pool;
	}

	virtual void Init()
	{
		timer = 10;
	}

	virtual void Update(float dt)
	{

		switch (go->mode)
		{
		case WALKING:
			Move(dt * FYGAR_SPEED);
			break;
		default:
			break;
		}
		
		timer -= dt * game_speed;

		if (timer <= 0 && go->mode != CRUSHED && go->mode != DYING)
		{
			Flame* flame = flame_pool->FirstAvailable();
			if (flame != NULL)
			{
				go->moving = false;
				switch (go->direction)
				{
				case DIRECTION::LEFT:
					flame->Init(go->position.x - CELL_SIZE, go->position.y, DIRECTION::LEFT, go);
					break;
				case DIRECTION::RIGHT:
					flame->Init(go->position.x + CELL_SIZE, go->position.y, DIRECTION::RIGHT, go);
					break;
				case DIRECTION::UP:
					flame->Init(go->position.x, go->position.y - CELL_SIZE, DIRECTION::UP, go);
					break;
				case DIRECTION::DOWN:
					flame->Init(go->position.x, go->position.y + CELL_SIZE, DIRECTION::DOWN, go);
					break;
				case DIRECTION::NONE:
					break;
				default:
					break;
				}
				go->mode = ATTACKING;
				
				RenderComponent* renderComponent = flame->GetComponent<RenderComponent*>();
				renderComponent->SetImageIndex(0);
				renderComponent->SetImageSpeed(6.f);
				go->Send(FYGAR_FIRE);

			}
			timer = 10;
			
		}

	}


	// move the Fygar left or right, depending on direction
	void Move(float move)
	{
		if (move > CELL_SIZE / 4)
			move = CELL_SIZE / 4;
		
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
