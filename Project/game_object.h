#pragma once

// GameObject represents objects which moves are drawn
#include "vector2D.h"
#include "component.h"
#include "avancezlib.h"
#include <vector>


enum Message { HIT, CELL_DUG, GAME_OVER, WALL };

class Component;

class GameObject
{
protected:
	std::vector<GameObject*> receivers;
	std::vector<Component*> components;

public:
	Vector2D position;
	DIRECTION direction;
	bool enabled;

	virtual ~GameObject();

	virtual void Create();
	virtual void AddComponent(Component * component);

	virtual void Init();
	virtual void InitPos(int x, int y);
	virtual void InitDir(DIRECTION dir);
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