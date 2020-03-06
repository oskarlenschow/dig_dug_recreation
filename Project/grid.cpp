

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
			if( (x == 6 && y > 2 && y < 10) || (y == 9 && x > 4 && x < 8) || (x == 2 && y > 3 && y < 9) || (x == 9 && y < 16 && y > 10) || (y == 4 && x < 13 && x > 8) || (y == 12 && x < 6 && x > 1) || y == 2)
				course_grid.insert(make_pair(from2Dto1Dindex(x, y, course_columns), true));
			else
				course_grid.insert(make_pair(from2Dto1Dindex(x, y, course_columns), false));
		}
	}
	for (int x = 0; x <= fine_columns; x++) {
		for (int y = 0; y <= fine_rows; y++) {
			fine_grid.insert(make_pair(from2Dto1Dindex(x, y, fine_columns), false));
		}
	}
	digout_small = engine->createSprite("data/digout_small.bmp");
	digout_full = engine->createSprite("data/digout_full.bmp");
	digout_corner = engine->createSprite("data/digout_corner.bmp");
	digout_end = engine->createSprite("data/digout_end.bmp");
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
	
	CalculateAndRender();
}

void GridComponent::CalculateAndRender() {

	int fine_x, fine_y;

	for (int course_x = 0; course_x <= course_columns; course_x++) { 
		for (int course_y = 3; course_y <= course_rows; course_y++) { // Start drawing where the dirt starts (y = 3).

			fine_x = course_x * fine_per_course;
			fine_y = course_y * fine_per_course;

			if ((fine_grid[from2Dto1Dindex(fine_x, fine_y, fine_columns)] || fine_grid[from2Dto1Dindex(fine_x + fine_per_course - 1, fine_y + fine_per_course - 1, fine_columns)]) //Check corners, skip if not true
				&& !course_grid[from2Dto1Dindex(course_x, course_y, course_columns)]) {
				int column[4] = { 0,0,0,0 };  //TODO: dynamic vector!
				int row[4] = { 0,0,0,0 };

				for (int xx = 0; xx < fine_per_course; xx++) {
					for (int yy = 0; yy < fine_per_course; yy++) {
						if (fine_grid[from2Dto1Dindex(fine_x + xx, fine_y + yy, fine_columns)]) {  //Check if small cells line up in completed rows and columns.
							column[xx] += 1;
							row[yy] += 1;
							if (column[xx] == fine_per_course)
								digout_small->draw((fine_x + xx) * fine_cell_size, fine_y * fine_cell_size, 270, NULL, SDL_FLIP_NONE);
							if (row[yy] == fine_per_course)
								digout_small->draw(fine_x * fine_cell_size, (fine_y + yy) * fine_cell_size);
						}

					}
				}
			}
			else if (course_grid[from2Dto1Dindex(course_x, course_y, course_columns)]) {

				bool left_cell = false, right_cell = false, top_cell = false, bottom_cell = false, horizontal = false, vertical = false;

				if (course_x > 0) {
					left_cell = true;
					if (!course_grid[from2Dto1Dindex(course_x - 1, course_y, course_columns)]) {  //Check left of cell, for picking sprite
						for (int i = 0; i < fine_per_course; i++) {
							if (!fine_grid[from2Dto1Dindex(course_x * fine_per_course - 1, course_y * fine_per_course + i, fine_columns)]) { //Also check smaller segments
								left_cell = false;
							}
						}
					}
				}
				if (course_x < course_columns - 1) {
					right_cell = true;
					if (!course_grid[from2Dto1Dindex(course_x + 1, course_y, course_columns)]) {  //Check right of cell, for picking sprite
						for (int i = 0; i < fine_per_course; i++) {
							if (!fine_grid[from2Dto1Dindex((course_x + 1) * fine_per_course, course_y * fine_per_course + i, fine_columns)]) { //Also check smaller segments
								right_cell = false;
							}
						}
					}
				}
				if (course_y > 0) {
					top_cell = true;
					if (!course_grid[from2Dto1Dindex(course_x, course_y - 1, course_columns)]) { //Check top of cell, for picking sprite
						for (int i = 0; i < fine_per_course; i++) {
							if (!fine_grid[from2Dto1Dindex(course_x * fine_per_course + i, course_y * fine_per_course - 1, fine_columns)]) { //Also check smaller segments
								top_cell = false;
							}
						}
					}
				}
				if (course_y < course_rows - 1) {
					bottom_cell = true;
					if (!course_grid[from2Dto1Dindex(course_x, course_y + 1, course_columns)]) { //Check bottom of cell, for picking sprite
						for (int i = 0; i < fine_per_course; i++) {
							if (!fine_grid[from2Dto1Dindex(course_x * fine_per_course + i, (course_y + 1) * fine_per_course, fine_columns)]) { //Also check smaller segments
								bottom_cell = false;
							}
						}
					}
				}

				//Draw corners
				if (left_cell && top_cell || top_cell && right_cell || right_cell && bottom_cell || bottom_cell && left_cell) {
					if (left_cell && top_cell)
						digout_corner->draw(course_x * course_cell_size, course_y * course_cell_size, 0, NULL, SDL_FLIP_HORIZONTAL);
					if (top_cell && right_cell)
						digout_corner->draw(course_x * course_cell_size, course_y * course_cell_size);
					if (right_cell && bottom_cell)
						digout_corner->draw(course_x * course_cell_size, course_y * course_cell_size, 0, NULL, SDL_FLIP_VERTICAL);
					if (bottom_cell && left_cell)
						digout_corner->draw(course_x * course_cell_size, course_y * course_cell_size, 0, NULL, (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL));

				}
				//Draw straights
				else if (left_cell && right_cell) {
					digout_full->draw(course_x * course_cell_size, course_y * course_cell_size, 90, NULL, SDL_FLIP_NONE);
				}
				else if (top_cell && bottom_cell) {
					digout_full->draw(course_x * course_cell_size, course_y * course_cell_size);
				}
				//Draw ends
				else if (left_cell) {
					digout_end->draw(course_x * course_cell_size, course_y * course_cell_size, 270, NULL, SDL_FLIP_NONE);
				}
				else if (right_cell) {
					digout_end->draw(course_x * course_cell_size, course_y * course_cell_size, 90, NULL, SDL_FLIP_NONE);
				}
				else if (top_cell) {
					digout_end->draw(course_x * course_cell_size, course_y * course_cell_size, 0, NULL, SDL_FLIP_NONE);
				}
				else if (bottom_cell) {
					digout_end->draw(course_x * course_cell_size, course_y * course_cell_size, 0, NULL, SDL_FLIP_VERTICAL);
				}

			}


		}
	}
}