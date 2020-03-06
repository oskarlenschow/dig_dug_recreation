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

	if (sprite) {
		SDL_RendererFlip flip = SDL_FLIP_NONE;
		double angle = 0;

		switch (go->direction)
		{
		case DIRECTION::LEFT:
			flip = SDL_FLIP_HORIZONTAL;
			break;
		case DIRECTION::UP:
			angle = 270; 
			break;
		case DIRECTION::DOWN:
			angle = 90;
			break;
		default:
			break;
		}
		sprite->draw(int(go->position.x), int(go->position.y), angle, NULL, flip);


	}
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


