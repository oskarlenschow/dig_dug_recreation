#pragma once

// GameObject represents objects which moves are drawn
#include "vector2D.h"
#include "component.h"
#include "avancezlib.h"
#include <vector>


enum Message { HIT, GAME_OVER, BOTH_DIRECTIONS, WALL, DIG, WALK, SCORE_UP, BURST, PUMP, PUMP_RELEASE, POOKAH_BURST, FYGAR_BURST, PLAYER_HIT};

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
	bool enabled, moving;
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