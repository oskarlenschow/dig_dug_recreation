#include "component.h"
#include "game_object.h"
#include "avancezlib.h"
#include <iostream>

void Component::Create(AvancezLib * engine, GameObject * go, std::set<GameObject*>* game_objects)
{
	this->go = go;
	this->engine = engine;
	this->game_objects = game_objects;
}

void RenderComponent::Create(AvancezLib * engine, GameObject * go, std::set<GameObject*>* game_objects, const char * sprite_name)
{
	Component::Create(engine, go, game_objects);

	sprite = engine->createSprite(sprite_name);
}

void RenderComponent::Update(float dt)
{
	if (!go->enabled)
		return;

	if (sprite)
		sprite->draw(int(go->position.x), int(go->position.y));
}

void RenderComponent::Destroy()
{
	if (sprite != NULL)
		sprite->destroy();
	sprite = NULL;
}


void CollideComponent::Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects)
{
	Component::Create(engine, go, game_objects);
	this->coll_objects = coll_objects;
}


void CollideComponent::Update(float dt)
{
	for (auto i = 0; i < coll_objects->pool.size(); i++)
	{
		GameObject * go0 = coll_objects->pool[i];
		if (go0->enabled)
		{
			if ((go0->position.x > go->position.x - 10) &&
				(go0->position.x < go->position.x + 10) &&
				(go0->position.y > go->position.y - 10) &&
				(go0->position.y < go->position.y + 10))
			{
				go->Receive(HIT);
				go0->Receive(HIT);
			}
		}
	}
}

/*
void CellComponent::Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, int percentage_dug) {
	Component::Create(engine, go, game_objects);
	this->percentage_dug = percentage_dug;
}
void CellComponent::Update(float dt) {

}
void CellComponent::push_back(GameObject* go) {
	this->hosted_objects.push_back(go);
}
std::vector<GameObject*> CellComponent::get_hosted_objects() {
	return hosted_objects;
}
void SpatialHashBehaviourComponent::Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, std::set<GameObject*> * hostable_objects, double cellSize, int width, int height)
{
	Component::Create(engine, go, game_objects);
	this->hostable_objects = hostable_objects;
	this->cellSize = cellSize;
	this->width = width;
	this->height = height;

	numColumns = width / cellSize;
	numRows = height / cellSize;
}

void SpatialHashBehaviourComponent::Update(float dt)
{
	//Clear the unordered map:
	spatialHash.clear();

	//Update grid according to new positions of ball objects in ball_pool:
	for (auto go = hostable_objects->begin(); go != hostable_objects->end(); go++) {
		Vector2D pos = (*go)->position;

		if (pos.x > 0 && pos.y > 0 && pos.x < width && pos.y < height) {
			int x = floor(pos.x / cellSize);
			int y = floor(pos.y / cellSize);

			spatialHash[from2Dto1Dindex(x, y)].push_back(*go);
		}
	}
}

std::set<GameObject*> SpatialHashBehaviourComponent::QueryGridNeighbors(Vector2D position)
{
	std::set<GameObject*> returnVector;

	int cellX = floor(position.x / cellSize) - 1;
	int cellY = floor(position.y / cellSize) - 1;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (cellX + i > 0 && cellY + j > 0 && cellX + i < numColumns && cellY + j < numRows) {
				std::vector<GameObject*> objects_in_cell = spatialHash[from2Dto1Dindex(cellX + i, cellY + j)].get_hosted_objects;
				returnVector.insert(objects_in_cell.begin(), objects_in_cell.end());
			}
		}
	}
	return returnVector;
}

*/