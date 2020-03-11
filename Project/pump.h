#pragma once

#include <iostream>

using namespace std;

// Pumps are shot by the player towards the aliens
class Pump : public GameObject
{

public:

	virtual void Init(int x, int y)
	{
		SDL_Log("Pump::Init");
		GameObject::Init(x, y);
		mode = WALKING;
		moving = true;
	}

	virtual void Receive(Message m)
	{
		if (!enabled)
			return;

		if (m == PUMP)
		{
			moving = false;
			//SDL_Log("Pump::Hit");
		}
		if (m == FYGAR_BURST || m == POOKAH_BURST) {
			enabled = false;
		}
		if (m == WALL) {
			enabled = false;
		}
	}
};

class PumpBehaviourComponent : public Component
{
public:
	virtual void Update(float dt)
	{
		if (abs(go->position.x - go->start_position.x) > CELL_SIZE * 3 || abs(go->position.y - go->start_position.y) > CELL_SIZE * 3) {
			go->enabled = false;
		}
		else if (go->moving) {
			switch (go->direction)
			{
			case DIRECTION::LEFT:
				go->position.x -= PUMP_SPEED * dt;
				break;
			case DIRECTION::RIGHT:
				go->position.x += PUMP_SPEED * dt;
				break;
			case DIRECTION::UP:
				go->position.y -= PUMP_SPEED * dt;
				break;
			case DIRECTION::DOWN:
				go->position.y += PUMP_SPEED * dt;
				break;
			default:
				break;
			}
			
		}
		
		
	}
};
class PumpCollideComponent : public Component
{
	ObjectPool<GameObject>* coll_objects; // collision will be tested with these objects

public:
	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, ObjectPool<GameObject>* coll_objects)
	{
		Component::Create(engine, go, game_objects);
		this->coll_objects = coll_objects;
	}
	virtual void Update(float dt)
	{
		for (auto i = 0; i < coll_objects->pool.size(); i++)
		{
			GameObject* go0 = coll_objects->pool[i];
			if (go0->enabled)
			{
				
				if ((go0->position.x > go->position.x - 10) &&
					(go0->position.x < go->position.x + 10) &&
					(go0->position.y > go->position.y - 10) &&
					(go0->position.y < go->position.y + 10))
				{
					//cout << "test" << endl;
					go->Receive(PUMP);
					go0->Receive(PUMP);
				}
			}
		}
	}
};

