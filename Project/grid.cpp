

#include "grid.h"
#include <iostream>

using namespace std;


void GridComponent::Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, GameObject* player, std::vector<GameObject*>* collision_objects, double cellSize, int width, int height)
{
	Component::Create(engine, go, game_objects);
	this->collision_objects = collision_objects;
	this->player = player;

	this->course_cell_size = cellSize;
	this->fine_cell_size = cellSize / 4;


	this->width = width;
	this->height = height;

	course_columns = (int)(width  / course_cell_size);
	course_rows    = (int)(height / course_cell_size);
	fine_columns   = (int)(width  / fine_cell_size);
	fine_rows      = (int)(height / fine_cell_size);

	for (int x = 0; x <= course_columns; x++) {
		for (int y = 0; y <= course_rows; y++) {
			//course_grid.insert(make_pair(from2Dto1Dindex(x, y, course_columns), vector<GameObject*>()));
			course_grid.insert(make_pair(from2Dto1Dindex(x, y, course_columns), false));
		}
	}
	for (int x = 0; x <= fine_columns; x++) {
		for (int y = 0; y <= fine_rows; y++) {
			fine_grid.insert(make_pair(from2Dto1Dindex(x, y, fine_columns), false));
		}
	}
	fine_digout = engine->createSprite("data/digout_small.bmp");
	course_digout = engine->createSprite("data/digout.bmp");
}

void GridComponent::Update(float dt)
{
	/*TODO
		Check the go (player) position and direction, and set pair in map to percentage and direction. Use this for drawing.
	*/

	double object_x = player->position.x;
	double object_y = player->position.y;

	int fine_collision_column = (int) (object_x / fine_cell_size); 
	int fine_collision_row    = (int) (object_y / fine_cell_size);

	int course_collision_column    = (int) (object_x / course_cell_size);
	int course_collision_row       = (int) (object_y / course_cell_size);
	/*
	double cell_x = (cellSize/4) * column; //Not the same as object_x, as this uses the truncated result
	double cell_y = (cellSize/4) * row;
	
	double percentage_dug_x = (object_x - cell_x) / (cellSize /4);
	double percentage_dug_y = (object_x - cell_x) / (cellSize /4);
	*/
	//DIRECTION dir = go->direction;

	if (fine_grid[from2Dto1Dindex(fine_collision_column, fine_collision_row, fine_columns)] != true) {
		fine_grid[from2Dto1Dindex(fine_collision_column, fine_collision_row, fine_columns)] =  true;
	}
	if (course_grid[from2Dto1Dindex(course_collision_column, course_collision_row, course_columns)] != true) {
		course_grid[from2Dto1Dindex(course_collision_column, course_collision_row, course_columns)]  = true;
	}
	for (int x = 0; x <= course_columns; x++) {
		for (int y = 0; y <= course_rows; y++) {
			if (course_grid[from2Dto1Dindex(x, y, course_columns)] == true) {
				//cout << x << ":" << y << " , ";
				course_digout->draw(x * course_cell_size + 1, y * course_cell_size + 1);
			}
		}
	}
	for (int x = 0; x <= fine_columns; x++) {
		for (int y = 0; y <= fine_rows; y++) {
			if (fine_grid[from2Dto1Dindex(x, y, fine_columns)] == true) {
				//cout << x << ":" << y << " , ";
				fine_digout->draw(x * fine_cell_size + 1, y * fine_cell_size + 1);
			}
		}
	}
	


}