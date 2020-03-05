

#include "grid.h"
#include <iostream>

using namespace std;


void GridComponent::Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, GameObject* player, std::vector<GameObject*>* collision_objects, double cellSize, int width, int height, int fine_per_course)
{
	Component::Create(engine, go, game_objects);
	this->collision_objects = collision_objects;
	this->player = player;
	this->fine_per_course = fine_per_course;
	this->course_cell_size = cellSize;
	this->fine_cell_size = cellSize / fine_per_course;


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
	course_digout = engine->createSprite("data/digout_full.bmp");
}

void GridComponent::Update(float dt)
{

	int object_start_x = player->position.x;
	int object_start_y = player->position.y;

	int object_end_x = player->position.x + player->dimensions.x;
	int object_end_y = player->position.y + player->dimensions.y;
	
	int fine_start_column = object_start_x / fine_cell_size; 
	int fine_start_row    = object_start_y / fine_cell_size;

	int fine_end_column   = object_end_x / fine_cell_size;
	int fine_end_row      = object_end_y / fine_cell_size;
	
	int course_collision_column    = (int) (object_start_x / course_cell_size);
	int course_collision_row       = (int) (object_start_y / course_cell_size);
	
	double course_collision_x = (course_cell_size ) * course_collision_column; //Not the same as object_x, as this uses the truncated result
	double course_collision_y = (course_cell_size)  *  course_collision_row;

	//Tell player it can move in both directions
	if (object_start_x == course_collision_x && object_start_y == course_collision_y) {
		player->Receive(BOTH_DIRECTIONS);
	}
	//Fill all small cells that are crossed
	for (int x = fine_start_column; x < fine_end_column; x++) {
		for (int y = fine_start_row; y < fine_end_row; y++)
			if (!fine_grid[from2Dto1Dindex(x, y, fine_columns)]) {
				fine_grid[from2Dto1Dindex(x, y, fine_columns)] = true;
				player->Receive(DIGGING);
			}
			else
				player->Receive(NOT_DIGGING);
	}
	//Ensure course grid cells are set to true when all small cells in it have been crossed
	if (fine_start_column == course_collision_column * fine_per_course && fine_start_row == course_collision_row * fine_per_course)
		course_grid[from2Dto1Dindex(course_collision_column, course_collision_row, course_columns)] = true;

	for (int x = 0; x <= fine_columns; x++) {
		for (int y = 0; y <= fine_rows; y++) {
			if (fine_grid[from2Dto1Dindex(x, y, fine_columns)] == true) {
				fine_digout->draw(x * fine_cell_size, y * fine_cell_size);
			}
		}
	}
	for (int x = 0; x <= course_columns; x++) {
		for (int y = 0; y <= course_rows; y++) {
			if (course_grid[from2Dto1Dindex(x, y, course_columns)] == true) {
				course_digout->draw(x * course_cell_size, y * course_cell_size);
			}
		}
	}
	
	
}