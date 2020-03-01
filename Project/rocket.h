#pragma once


class RocketBehaviourComponent : public Component
{
public:

	void Update(float dt)
	{
		go->position.y -= ROCKET_SPEED * dt; // rocket_speed * time

		if (go->position.y < 0) // When the rocket reaches the top of the screen, it disappears.
			go->enabled = false;
	}
};


// rockets are shot by the player towards the aliens
class Rocket : public GameObject
{

public:

	virtual void Init(double xPos)
	{
		SDL_Log("Rocket::Init");
		GameObject::Init();

		position.x = xPos;
		position.y = 480 - 52;
	}

	virtual void Receive(Message m)
	{
		if (!enabled)
			return;

		if (m == HIT)
		{
			enabled = false;
			SDL_Log("Rocket::Hit");
		}
	}
};