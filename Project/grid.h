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
class GridComponent : public Component {
	//std::unordered_map<int, vector<GameObject*>> course_grid; //Used for collision
	std::unordered_map<int, bool> course_grid; //Used for collision
	std::unordered_map<int, bool> fine_grid; //Used for smaller sections
	double course_cell_size;
	double fine_cell_size;
	int course_columns, course_rows, fine_columns, fine_rows, width, height, fine_per_course;
	std::vector<GameObject*> * collision_objects;
	GameObject* player;
	Sprite* digout_small;
	Sprite* digout_full;
	Sprite* digout_corner;
	Sprite* digout_end;
private:
	inline int from2Dto1Dindex(int x, int y, int columns) { return x + y * columns; }
public:

	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, GameObject* player, std::vector<GameObject*>* collision_objects, double cellSize, int width, int height, int fine_per_course);
	virtual void Update(float dt);
	virtual void CalculateAndRender(); //Because there is so much logic involved in the rendering of the grid, this is not it's own rendering component

	//virtual std::set<GameObject*> QueryGridNeighbors(Vector2D position);
};

class GridCollisionComponent : public Component {

};

