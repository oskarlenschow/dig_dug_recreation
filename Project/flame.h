#pragma once


#pragma once

#include <iostream>

using namespace std;

// Pumps are shot by the player towards the aliens
class Flame : public GameObject
{
	GameObject * fygar;
public:

	virtual void Init(int x, int y, DIRECTION dir, GameObject* fygar)
	{
		SDL_Log("Flame::Init");
		GameObject::Init(x, y);
		this->fygar = fygar;
		direction = dir;
		mode = DYING;
	}

	virtual void Receive(Message m)
	{
		if (!enabled)
			return;
		if (m == BURST) {
			fygar->Receive(EXTINGUISH);
			enabled = false;
			cout << "test" << endl;
		}

	}
};

class FlameBehaviourComponent : public Component
{
public:
	virtual void Update(float dt)
	{


	}
};
class FlameCollideComponent : public Component
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

