#pragma once


class PumpBehaviourComponent : public Component
{
public:

	void Update(float dt)
	{
		go->position.y -= PUMP_SPEED * dt; // Pump_speed * time

		if (go->position.y < 0) // When the Pump reaches the top of the screen, it disappears.
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