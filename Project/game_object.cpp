#include "game_object.h"
#include "component.h"
#include "avancezlib.h"

void GameObject::Create()
{
	SDL_Log("GameObject::Create");

	enabled = false;
}

void GameObject::AddComponent(Component * component)
{
	components.push_back(component);
}


void GameObject::Init(int x, int y)
{
	SDL_Log("GameObject::Init");
	position.x = x;
	position.y = y;
	start_position.x = x;
	start_position.y = y;

	for (auto it = components.begin(); it != components.end(); it++)
		(*it)->Init();

	enabled = true;
}
void GameObject::InitPos(int x, int y) {
	position.x = x;
	position.y = y;
}
void GameObject::InitDir(DIRECTION dir) {
	direction = dir;
}
void GameObject::ResetPosition() {
	position = start_position;
}
void GameObject::Update(float dt)
{
	if (!enabled)
		return;

	for (auto it = components.begin(); it != components.end(); it++)
		(*it)->Update(dt);
}

void GameObject::Destroy()
{
	for (auto it = components.begin(); it != components.end(); it++)
		(*it)->Destroy();
}

GameObject::~GameObject()
{
	SDL_Log("GameObject::~GameObject");
}
string GameObject::GetName() {
	return "game_object";
}
void GameObject::AddReceiver(GameObject * go)
{
	receivers.push_back(go);
}

void GameObject::Send(Message m)
{
	for (auto i = 0; i < receivers.size(); i++)
	{
		if (!receivers[i]->enabled)
			continue;

		receivers[i]->Receive(m);
	}
}
