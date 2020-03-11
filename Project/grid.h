#pragma once

#include <stdio.h>
#include <vector>
#include <unordered_map>
#include "game_object.h"
#include <set>

using namespace std;

class Grid : public GameObject {
	
private:
	inline int from2Dto1Dindex(int x, int y, int columns) { return x + y * columns; }
public:
	std::unordered_map<int, bool> course_grid; //Used for larger sections
	std::unordered_map<int, bool> fine_grid; //Used for smaller sections
	double course_cell_size;
	double fine_cell_size;
	int course_columns, course_rows, fine_columns, fine_rows, width, height, fine_per_course;


	virtual ~Grid() { SDL_Log("Grid::~Grid"); }

	virtual void Create(double cellSize, int width, int height, int fine_per_course);

	virtual void Init(int x, int y)
	{
		SDL_Log("Grid::Init");
		GameObject::Init(x, y);
	}
	virtual void Receive(Message m)
	{
		if (!enabled)
			return;
	}
	virtual void Reset();
};

class GridPlayerComponent : public Component {

	Grid* grid;
	GameObject* player;

private:
	inline int from2Dto1Dindex(int x, int y, int columns) { return x + y * columns; }
public:

	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, GameObject* player);
	virtual void Update(float dt);
};

class GridRenderComponent : public Component {
	Grid* grid;
	Sprite* digout_small;
	Sprite* digout_full;
	Sprite* digout_corner;
	Sprite* digout_end;
private:
	inline int from2Dto1Dindex(int x, int y, int columns) { return x + y * columns; }
public:
	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects);
	virtual void Update(float dt);

};

class GridPathComponent : public Component {
	Grid* grid;
	GameObject* player;
	GameObject* go0;
	Vector2D current_cell;
	Vector2D next_cell;
	bool path;
	double timer;
private:
	inline int from2Dto1Dindex(int x, int y, int columns) { return x + y * columns; }
public:
	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, GameObject* go0, GameObject* player);
	virtual void Update(float dt);
	virtual Vector2D GetNextCell(Vector2D source_cell);
};

class GridCollideComponent : public Component {
	Grid* grid;
	GameObject* player;
	GameObject* go0;
private:
	inline int from2Dto1Dindex(int x, int y, int columns) { return x + y * columns; }
public:
	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, GameObject* go0, GameObject* player);
	virtual void Update(float dt);
};