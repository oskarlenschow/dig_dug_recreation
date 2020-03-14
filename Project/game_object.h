#pragma once

// GameObject represents objects which moves are drawn
#include "vector2D.h"
#include "component.h"
#include "avancezlib.h"
#include <vector>


enum Message { HIT, MOVING_HIT, GAME_OVER, BOTH_DIRECTIONS, WALL, NO_WALL, DIG, WALK, SCORE_UP, BURST, PUMP, PUMP_RELEASE, POOKAH_BURST, FYGAR_BURST, PLAYER_HIT, PLAYER_DEAD, EXTINGUISH, ROCK_LOOSE, FYGAR_FIRE };

class Component;

class GameObject
{
protected:
	std::vector<GameObject*> receivers;
	std::vector<Component*> components;

public:
	Vector2D position;
	Vector2D start_position;
	Vector2D dimensions;

	DIRECTION direction;
	DIRECTION_AXIS axis;
	bool enabled, moving, in_use=false;
	int mode = 0;

	virtual ~GameObject();

	virtual void Create();
	virtual void AddComponent(Component * component);

	virtual void Init(int x, int y);
	virtual void InitPos(int x, int y);
	virtual void InitDir(DIRECTION dir);
	virtual void ResetPosition();
	virtual void Update(float dt);
	virtual void Destroy();
	virtual void AddReceiver(GameObject *go);
	virtual void Receive(Message m) {}
	virtual string GetName();
	void Send(Message m);
	
	template<typename T>
	T GetComponent() {
		for (Component* c : components) {
			T t = dynamic_cast<T>(c);  //ugly but it works...
			if (t != nullptr) {
				return t;
			}
		}

		return nullptr;
	}
	
};