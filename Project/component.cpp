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

void RenderComponent::Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, const char* path, const double speed)
{
	Component::Create(engine, go, game_objects);
	
	for (int i = 0; i < 4; i++) { //Create the 2D vector of sprites, depending on mode
		vector <Sprite*> temp;
		sprites.insert(make_pair(i, temp));
	}

	
	sprite = engine->createSprite(path);
	sprites.at(0).push_back(sprite);

	sprite_index = 0;
	animation_speed = speed;

	go->dimensions.x = sprite->getImageWidth();
	go->dimensions.y = sprite->getImageHeight();
}
void RenderComponent::AddSprite(const char* path, const int mode) {
	cout << mode << endl;
	sprites.at(mode).push_back(engine->createSprite(path));

}
void RenderComponent::SetImageIndex(unsigned int index) {
	sprite_index = index;
}

void RenderComponent::SetImageSpeed(const double speed) {
	animation_speed = speed;
}
void RenderComponent::Update(float dt)
{
	if (!go->enabled)
		return;

	sprite_index = fmod(sprite_index + (animation_speed * dt), sprites.at(go->mode).size());

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


		if (sprites.at(go->mode).at(floor(sprite_index)))
			sprites.at(go->mode).at(floor(sprite_index))->draw(int(go->position.x), int(go->position.y), angle, NULL, flip);
		
		//double old_index = sprite_index;
		//if (sprite_index < old_index) go->Receive(ANIMATION_END);


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


