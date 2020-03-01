#pragma once

#include <stdio.h>
#include <vector>
#include <unordered_map>
#include "game_object.h"
#include <set>

using namespace std;

class Grid : public GameObject {
public:
	virtual ~Grid() { SDL_Log("Grid::~Grid"); }

	virtual void Init()
	{
		SDL_Log("Grid::Init");
		GameObject::Init();
	}

	virtual void Receive(Message m)
	{
		if (!enabled)
			return;
	}
};

class GridBehaviourComponent : public Component {
	std::unordered_map<int, std::pair<float, float>> map;
	double cellSize;
	int numColumns, numRows, width, height;
	GameObject* player;

private:
	inline int from2Dto1Dindex(int x, int y) { return x + y * numColumns; }

public:

	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, GameObject* player, double cellSize, int width, int height);
	virtual void Update(float dt);
	//virtual std::set<GameObject*> QueryGridNeighbors(Vector2D position);
};

class GridCollisionComponent : public Component {

};

class GridRenderComponent : public Component {

};