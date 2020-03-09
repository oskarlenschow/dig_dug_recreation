#pragma once

#include <iostream>

using namespace std;

class PumpBehaviourComponent : public Component
{
public:

	void Update(float dt)
	{
		go->position.x -= PUMP_SPEED * dt; // Pump_speed * time
		cout << go->position.x << " : " << go->position.y << endl;
		if (go->position.x < 0) // When the Pump reaches the top of the screen, it disappears.
			go->enabled = false;
	}
};


// Pumps are shot by the player towards the aliens
class Pump : public GameObject
{

public:

	virtual void Init(int x, int y)
	{
		SDL_Log("Pump::Init");
		GameObject::Init(x, y);
		mode = WALKING;
	}

	virtual void Receive(Message m)
	{
		if (!enabled)
			return;

		if (m == HIT)
		{
			enabled = false;
			SDL_Log("Pump::Hit");
		}
	}
};