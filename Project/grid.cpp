

#include "grid.h"
#include <iostream>

using namespace std;


void GridBehaviourComponent::Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, GameObject* player, double cellSize, int width, int height)
{
	Component::Create(engine, go, game_objects);
	this->player = player;
	this->cellSize = cellSize;
	this->width = width;
	this->height = height;

	numColumns = width / cellSize;
	numRows = height / cellSize;

	for (int x = 0; x <= numColumns; x++) {
		for (int y = 0; y <= numRows; y++) {
			map.insert(std::make_pair(from2Dto1Dindex(x, y), std::make_pair(0, 0)));
		}
	}
}

void GridBehaviourComponent::Update(float dt)
{
	//Clear the unordered map:
	//map.clear();
	
	/*TODO
		Check the go (player) position and direction, and set pair in map to percentage and direction. Use this for drawing.
	*/
	int object_x = player->position.x;
	int object_y = player->position.y;

	int column = object_x / cellSize; //This gets truncated down
	int row = object_y / cellSize;

	int cell_x = cellSize * column; //Not the same as object_x, as this uses the truncated result
	int cell_y = cellSize * row;

	float percentage_dug_x = (object_x - cell_x) / cellSize;
	float percentage_dug_y = (object_x - cell_x) / cellSize;

	//DIRECTION dir = go->direction;

	if (map[from2Dto1Dindex(column, row)].first != 100) {
		map[from2Dto1Dindex(column, row)].first = 100;
		map[from2Dto1Dindex(column, row)].second = 100;

		for (int x = 0; x <= numColumns; x++) {
			for (int y = 0; y <= numRows; y++) {
				if (map[from2Dto1Dindex(x, y)].first == 100) {
					std::cout << x << "," << y << std::endl;
				}
			}
		}
		std::cout << "------------" << std::endl;
	}




}