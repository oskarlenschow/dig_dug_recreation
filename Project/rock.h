#pragma once

class RockBehaviourComponent : public Component
{
public:

	void Update(float dt)
	{
		double move = ROCK_SPEED * dt;

		if (move > CELL_SIZE / 4)
			move = CELL_SIZE / 4;

		if (go->moving)
			go->position.y += move;
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
			if (mode != CRUSHED) {
				mode = CRUSHED;
				RenderComponent* renderComponent = GetComponent<RenderComponent*>();
				renderComponent->SetImageIndex(0);
				renderComponent->SetImageSpeed(0.5f);
			}
		}
		if (m == BURST) {
			enabled = false;
		}
		if (m == WALL) {
			moving = false;
		}
		if (m == NO_WALL)
			moving = true;
	}
};