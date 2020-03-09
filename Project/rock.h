#pragma once

class RockBehaviourComponent : public Component
{
public:

	void Update(float dt)
	{
		go->position.x += ROCK_SPEED * dt; // Rock_speed * time

		if (go->position.x > 480) // When the Rock reaches the bottom of the screen, it disappears.
			go->enabled = false;
	}
};


// rockets are shot by the player towards the aliens
class Rock : public GameObject
{

public:

	virtual void Init(int x, int y)
	{
		SDL_Log("Rock::Init");
		GameObject::Init(x,y);
	}

	virtual void Receive(Message m)
	{
		if (!enabled)
			return;

		if (m == HIT)
		{
			enabled = false;
			SDL_Log("Rock::Hit");
		}
	}
};