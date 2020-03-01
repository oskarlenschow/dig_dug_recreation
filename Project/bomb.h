#pragma once

class BombBehaviourComponent : public Component
{
public:

	void Update(float dt)
	{
		go->position.x += BOMB_SPEED * dt; // bomb_speed * time

		if (go->position.x > 480) // When the bomb reaches the bottom of the screen, it disappears.
			go->enabled = false;
	}
};


// rockets are shot by the player towards the aliens
class Bomb : public GameObject
{

public:

	virtual void Init(double xPos, double yPos)
	{
		SDL_Log("Bomb::Init");
		GameObject::Init();

		position.x = xPos;
		position.x = yPos;
	}

	virtual void Receive(Message m)
	{
		if (!enabled)
			return;

		if (m == HIT)
		{
			enabled = false;
			SDL_Log("Bomb::Hit");
		}
	}
};