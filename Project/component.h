#pragma once

#include <set>
#include "object_pool.h"
#include <unordered_map>

class GameObject;
class AvancezLib;
class Sprite;

using namespace std;

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
	unordered_map<int, vector<Sprite*>> sprites;
	double sprite_index;
	double animation_speed;

	Sprite* sprite;

public:

	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, const char* first_sprite, const double animation_speed);
	virtual void Update(float dt);
	virtual void Destroy(); 

	virtual void AddSprite(const char* sprite_name, const int mode);
	virtual void SetImageIndex(unsigned int index);
	virtual void SetImageSpeed(const double speed);
	virtual unsigned int GetImageIndex() { return sprite_index; };

	Sprite * GetSprite() { return sprite; }
};


class CollideComponent : public Component
{
	ObjectPool<GameObject> * coll_objects; // collision will be tested with these objects

public:
	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects);
	virtual void Update(float dt);
};
