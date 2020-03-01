#pragma once

#include <set>
#include "object_pool.h"
#include <unordered_map>

class GameObject;
class AvancezLib;
class Sprite;

class Component
{
protected:
	AvancezLib * engine;	// used to access the engine
	GameObject * go;		// the game object this component is part of
	std::set<GameObject*> * game_objects;	// the global container of game objects

public:
	virtual ~Component() {}

	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects);

	virtual void Init() {}
	virtual void Update(float dt) = 0;
	virtual void Receive(int message) {}
	virtual void Destroy() {}
};


class RenderComponent : public Component
{
	Sprite* sprite;

public:

	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, const char * sprite_name);
	virtual void Update(float dt);
	virtual void Destroy();

	Sprite * GetSprite() { return sprite; }
};


class CollideComponent : public Component
{
	ObjectPool<GameObject> * coll_objects; // collision will be tested with these objects

public:
	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects);
	virtual void Update(float dt);
};/*
class CellComponent : public Component
{
	int percentage_dug;
	DIRECTION direction;
	std::vector<GameObject*> hosted_objects;
public:
	
	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, int percentage_dug);
	virtual void Update(float dt);
	virtual void push_back(GameObject*);
	virtual std::vector<GameObject*> get_hosted_objects();

};
class SpatialHashBehaviourComponent : public Component
{
	std::unordered_map<int, CellComponent> spatialHash;
	std::set<GameObject*> * hostable_objects;

	double cellSize;
	int numColumns, numRows, width, height;

private:
	inline int from2Dto1Dindex(int x, int y) { return x + y * numColumns; }

public:

	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, std::set<GameObject*> * hostable_objects, double cellSize, int width, int height);
	virtual void Update(float dt);
	virtual std::set<GameObject*> QueryGridNeighbors(Vector2D position);
};
*/
