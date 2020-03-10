#pragma once

#include "grid.h"
#include <iostream>
#include <queue>

using namespace std;
extern float game_speed;
extern float FYGAR_SPEED;

void Grid::Create(double cellSize, int width, int height, int fine_per_course) {

	SDL_Log("Grid::Create");

	this->fine_per_course = fine_per_course;
	this->course_cell_size = cellSize;
	this->fine_cell_size = cellSize / fine_per_course;
	this->width = width;
	this->height = height;
	course_columns = (int)(width / course_cell_size);
	course_rows = (int)(height / course_cell_size);
	fine_columns = (int)(width / fine_cell_size);
	fine_rows = (int)(height / fine_cell_size);

	for (int x = 0; x < course_columns; x++) {
		for (int y = 0; y < course_rows; y++) {
			if ((x == 6 && y > 2 && y < 10) || (y == 9 && x > 4 && x < 8) || (x == 2 && y > 3 && y < 9) || (x == 9 && y < 16 && y > 10) || (y == 4 && x < 13 && x > 8) || (y == 12 && x < 6 && x > 1) || y == 2) {
				course_grid.insert(make_pair(from2Dto1Dindex(x, y, course_columns), true));
				for (int xx = 0; xx < fine_per_course; xx++) {
					for (int yy = 0; yy < fine_per_course; yy++) {
						fine_grid.insert(make_pair(from2Dto1Dindex(x * fine_per_course + xx, y * fine_per_course + yy, fine_columns), true));
					}
				}
			}
			else {
				course_grid.insert(make_pair(from2Dto1Dindex(x, y, course_columns), false));
				for (int xx = 0; xx < fine_per_course; xx++) {
					for (int yy = 0; yy < fine_per_course; yy++) {
						fine_grid.insert(make_pair(from2Dto1Dindex(x * fine_per_course + xx, y * fine_per_course + yy, fine_columns), false));
					}
				}
			}
		}
	}
}

void Grid::Reset() {
	for (int x = 0; x < course_columns; x++) {
		for (int y = 0; y < course_rows; y++) {
			if ((x == 6 && y > 2 && y < 10) || (y == 9 && x > 4 && x < 8) || (x == 2 && y > 3 && y < 9) || (x == 9 && y < 16 && y > 10) || (y == 4 && x < 13 && x > 8) || (y == 12 && x < 6 && x > 1) || y == 2) {
				course_grid[from2Dto1Dindex(x, y, course_columns)] = true;
				for (int xx = 0; xx < fine_per_course; xx++) {
					for (int yy = 0; yy < fine_per_course; yy++) {
						fine_grid[from2Dto1Dindex(x * fine_per_course + xx, y * fine_per_course + yy, fine_columns)] = true;
					}
				}
			}
			else {
				course_grid[from2Dto1Dindex(x, y, course_columns)] = false;
				for (int xx = 0; xx < fine_per_course; xx++) {
					for (int yy = 0; yy < fine_per_course; yy++) {
						fine_grid[from2Dto1Dindex(x * fine_per_course + xx, y * fine_per_course + yy, fine_columns)] = false;
					}
				}
			}
		}
	}
}

void GridPlayerComponent::Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, GameObject* player){
	Component::Create(engine, go, game_objects);
	this->player = player;
	grid = dynamic_cast<Grid*>(go);
}

void GridPlayerComponent::Update(float dt){

	if (player->moving){

		int object_start_x = player->position.x;
		int object_start_y = player->position.y;

		int object_end_x = player->position.x + player->dimensions.x;
		int object_end_y = player->position.y + player->dimensions.y;

		int fine_start_column = round(object_start_x /	grid->fine_cell_size);
		int fine_start_row = round(object_start_y / grid->fine_cell_size);

		int fine_end_column = object_end_x / grid->fine_cell_size;
		int fine_end_row = object_end_y / grid->fine_cell_size;

		int course_collision_column = (int)(object_start_x / grid->course_cell_size);
		int course_collision_row = (int)(object_start_y / grid->course_cell_size);

		double course_collision_x = (grid->course_cell_size)*course_collision_column; //Not the same as object_x, as this uses the truncated result (example: object_x = 67, course_collision_x = floor(67 / 32) * 32 = 64
		double course_collision_y = (grid->course_cell_size)*course_collision_row;    //These two values are used to know if player is exactly inside a cell, without margin.

		//Tell player it can move in both directions
		if (object_start_x == course_collision_x && object_start_y == course_collision_y) {
			player->Receive(BOTH_DIRECTIONS);
			switch (player->direction)
			{
			case DIRECTION::LEFT:
				if(grid->course_grid[from2Dto1Dindex(course_collision_column - 1, course_collision_row, grid->course_columns)])
					player->Receive(WALK);
				break;
			case DIRECTION::RIGHT:
				if (grid->course_grid[from2Dto1Dindex(course_collision_column + 1, course_collision_row, grid->course_columns)])
					player->Receive(WALK);
				break;
			case DIRECTION::UP:
				if (grid->course_grid[from2Dto1Dindex(course_collision_column, course_collision_row - 1, grid->course_columns)])
					player->Receive(WALK);
				break;
			case DIRECTION::DOWN:
				if (grid->course_grid[from2Dto1Dindex(course_collision_column, course_collision_row + 1, grid->course_columns)])
					player->Receive(WALK);
				break;
			default:
				break;
			}
			
		}
		//Fill all small cells that are crossed
		for (int x = fine_start_column; x < fine_end_column; x++) {
			for (int y = fine_start_row; y < fine_end_row; y++)
				if (!grid->fine_grid[from2Dto1Dindex(x, y, grid->fine_columns)]) {
					grid->fine_grid[from2Dto1Dindex(x, y, grid->fine_columns)] = true;
					player->Receive(DIG);
				}
		}
		//Ensure course grid cells are set to true when all small cells in it have been crossed
		if (fine_start_column == course_collision_column * grid->fine_per_course && fine_start_row == course_collision_row * grid->fine_per_course
			&& !grid->course_grid[from2Dto1Dindex(course_collision_column, course_collision_row, grid->course_columns)]) {
			go->Send(SCORE_UP);
			grid->course_grid[from2Dto1Dindex(course_collision_column, course_collision_row, grid->course_columns)] = true;
		}
				
	}
}
void GridRenderComponent::Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects) {

	Component::Create(engine, go, game_objects);
	grid = dynamic_cast<Grid*>(go);

	digout_small = engine->createSprite("data/digout_small.bmp");  //TODO: These should be arguments sent in?
	digout_full = engine->createSprite("data/digout_full.bmp");
	digout_corner = engine->createSprite("data/digout_corner.bmp");
	digout_end = engine->createSprite("data/digout_end.bmp");

}

void GridRenderComponent::Update(float dt) {

	int fine_x, fine_y;

	for (int course_x = 0; course_x <= grid->course_columns; course_x++) {
		for (int course_y = 3; course_y <= grid->course_rows; course_y++) { // Start drawing where the dirt starts (y = 3).

			fine_x = course_x * grid->fine_per_course;
			fine_y = course_y * grid->fine_per_course;

			if ((grid->fine_grid[from2Dto1Dindex(fine_x, fine_y, grid->fine_columns)] 
				|| grid->fine_grid[from2Dto1Dindex(fine_x + grid->fine_per_course - 1, fine_y + grid->fine_per_course - 1, grid->fine_columns)]) //Check corners, skip if not true
				&& !grid->course_grid[from2Dto1Dindex(course_x, course_y, grid->course_columns)]) {
				int column[4] = { 0,0,0,0 };  //TODO: dynamic vector!
				int row[4] = { 0,0,0,0 };

				for (int xx = 0; xx < grid->fine_per_course; xx++) {
					for (int yy = 0; yy < grid->fine_per_course; yy++) {
						if (grid->fine_grid[from2Dto1Dindex(fine_x + xx, fine_y + yy, grid->fine_columns)]) {  //Check if small cells line up in completed rows and columns.
							column[xx] += 1;
							row[yy] += 1;
							if (column[xx] == grid->fine_per_course)
								digout_small->draw((fine_x + xx) * grid->fine_cell_size, fine_y * grid->fine_cell_size, 270, NULL, SDL_FLIP_NONE);
							if (row[yy] == grid->fine_per_course)
								digout_small->draw(fine_x * grid->fine_cell_size, (fine_y + yy) * grid->fine_cell_size);
						}

					}
				}
			}
			else if (grid->course_grid[from2Dto1Dindex(course_x, course_y, grid->course_columns)]) {

				bool left_cell = false, right_cell = false, top_cell = false, bottom_cell = false;

				if (course_x > 0) {
					left_cell = true;
					if (!grid->course_grid[from2Dto1Dindex(course_x - 1, course_y, grid->course_columns)]) {  //Check left of cell, for picking sprite
						for (int i = 0; i < grid->fine_per_course; i++) {
							if (!grid->fine_grid[from2Dto1Dindex(course_x * grid->fine_per_course - 1, course_y * grid->fine_per_course + i, grid->fine_columns)]) { //Also check smaller segments
								left_cell = false;
							}
						}
					}
				}
				if (course_x < grid->course_columns - 1) {
					right_cell = true;
					if (!grid->course_grid[from2Dto1Dindex(course_x + 1, course_y, grid->course_columns)]) {  //Check right of cell, for picking sprite
						for (int i = 0; i < grid->fine_per_course; i++) {
							if (!grid->fine_grid[from2Dto1Dindex((course_x + 1) * grid->fine_per_course, course_y * grid->fine_per_course + i, grid->fine_columns)]) { //Also check smaller segments
								right_cell = false;
							}
						}
					}
				}
				if (course_y > 0) {
					top_cell = true;
					if (!grid->course_grid[from2Dto1Dindex(course_x, course_y - 1, grid->course_columns)]) { //Check top of cell, for picking sprite
						for (int i = 0; i < grid->fine_per_course; i++) {
							if (!grid->fine_grid[from2Dto1Dindex(course_x * grid->fine_per_course + i, course_y * grid->fine_per_course - 1, grid->fine_columns)]) { //Also check smaller segments
								top_cell = false;
							}
						}
					}
				}
				if (course_y < grid->course_rows - 1) {
					bottom_cell = true;
					if (!grid->course_grid[from2Dto1Dindex(course_x, course_y + 1, grid->course_columns)]) { //Check bottom of cell, for picking sprite
						for (int i = 0; i < grid->fine_per_course; i++) {
							if (!grid->fine_grid[from2Dto1Dindex(course_x * grid->fine_per_course + i, (course_y + 1) * grid->fine_per_course, grid->fine_columns)]) { //Also check smaller segments
								bottom_cell = false;
							}
						}
					}
				}
				
				//Draw corners
				if (left_cell && top_cell || top_cell && right_cell || right_cell && bottom_cell || bottom_cell && left_cell) {
					if (left_cell && top_cell)
						digout_corner->draw(course_x * grid->course_cell_size, course_y * grid->course_cell_size, 0, NULL, SDL_FLIP_HORIZONTAL);
					if (top_cell && right_cell)
						digout_corner->draw(course_x * grid->course_cell_size, course_y * grid->course_cell_size);
					if (right_cell && bottom_cell)
						digout_corner->draw(course_x * grid->course_cell_size, course_y * grid->course_cell_size, 0, NULL, SDL_FLIP_VERTICAL);
					if (bottom_cell && left_cell)
						digout_corner->draw(course_x * grid->course_cell_size, course_y * grid->course_cell_size, 0, NULL, (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL));

				}
				//Draw straights
				else if (left_cell && right_cell) {
					digout_full->draw(course_x * grid->course_cell_size, course_y * grid->course_cell_size, 90, NULL, SDL_FLIP_NONE);
				}
				else if (top_cell && bottom_cell) {
					digout_full->draw(course_x * grid->course_cell_size, course_y * grid->course_cell_size);
				}
				//Draw ends
				else if (left_cell) {
					digout_end->draw(course_x * grid->course_cell_size, course_y * grid->course_cell_size, 270, NULL, SDL_FLIP_NONE);
				}
				else if (right_cell) {
					digout_end->draw(course_x * grid->course_cell_size, course_y * grid->course_cell_size, 90, NULL, SDL_FLIP_NONE);
				}
				else if (top_cell) {
					digout_end->draw(course_x * grid->course_cell_size, course_y * grid->course_cell_size, 0, NULL, SDL_FLIP_NONE);
				}
				else if (bottom_cell) {
					digout_end->draw(course_x * grid->course_cell_size, course_y * grid->course_cell_size, 0, NULL, SDL_FLIP_VERTICAL);
				}
				
			}


		}
	}
}

void GridCollideComponent::Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, vector<ObjectPool<GameObject> *> collision_pools, GameObject* player) {
	this->player = player;
	grid = dynamic_cast<Grid*>(go);
	this->collision_pools = collision_pools;
}
void GridCollideComponent::Update(float dt) {

	//cout << clamp_length << endl;

	for (int i = 0; i < collision_pools.size(); i++)

		for (int j = 0; j < collision_pools[i]->pool.size(); j++)
		{
			GameObject* go0 = collision_pools[i]->pool[j];
			
			if (go0->enabled)
			{
				bool path = false;
				//Check if enemy is outside of bounds
				Vector2D center_cell = Vector2D(floor((go0->position.x + (go0->dimensions.x / 2)) / grid->course_cell_size), 
					floor((go0->position.y + (go0->dimensions.y / 2)) / grid->course_cell_size));
				
				Vector2D left_cell = Vector2D(floor((go0->position.x) / grid->course_cell_size),
					floor((go0->position.y + (go0->dimensions.y / 2)) / grid->course_cell_size));

				Vector2D right_cell = Vector2D(floor((go0->position.x + (go0->dimensions.x - 1)) / grid->course_cell_size),
					floor((go0->position.y + (go0->dimensions.y / 2)) / grid->course_cell_size));

				Vector2D top_cell = Vector2D(floor((go0->position.x + (go0->dimensions.x / 2)) / grid->course_cell_size),
					floor((go0->position.y) / grid->course_cell_size));

				Vector2D bottom_cell = Vector2D(floor((go0->position.x + (go0->dimensions.x / 2)) / grid->course_cell_size),
					floor((go0->position.y + (go0->dimensions.y - 1)) / grid->course_cell_size));
				
				//Move enemy back in
				if (!grid->course_grid[from2Dto1Dindex(left_cell.x, left_cell.y, grid->course_columns)]) {
					go0->position.x = center_cell.x * grid->course_cell_size;
				}
				if (!grid->course_grid[from2Dto1Dindex(right_cell.x, right_cell.y, grid->course_columns)]) {
					go0->position.x = center_cell.x * grid->course_cell_size;
				}
				if (!grid->course_grid[from2Dto1Dindex(top_cell.x, top_cell.y, grid->course_columns)]) {
					go0->position.y = center_cell.y * grid->course_cell_size;
				}
				if (!grid->course_grid[from2Dto1Dindex(bottom_cell.x, bottom_cell.y, grid->course_columns)]) {
					go0->position.y = center_cell.y * grid->course_cell_size;
				}
					
				
				

				Vector2D next_cell = GetNextCell(go0->position);
					
				if (next_cell.x != -1)
					path = true;
				

				//Hunt player
				if (path && (int)go0->position.x == center_cell.x * grid->course_cell_size && (int)go0->position.y == center_cell.y * grid->course_cell_size) {

					next_cell = GetNextCell(go0->position);
					
					if (next_cell.x != -1) {
						int next_column = next_cell.x;
						int next_row = next_cell.y;
						int current_column = left_cell.x;
						int current_row = left_cell.y;

						if (next_column < current_column)
							go0->direction = DIRECTION::LEFT;
						else if (next_column > current_column)
							go0->direction = DIRECTION::RIGHT;
						else if (next_row < current_row)
							go0->direction = DIRECTION::UP;
						else if (next_row > current_row)
							go0->direction = DIRECTION::DOWN;
					}

				}
				//Mindless walking
				else if (go0->position.x == center_cell.x * grid->course_cell_size && go0->position.y == center_cell.y * grid->course_cell_size) {
					bool left = grid->course_grid[from2Dto1Dindex(center_cell.x - 1, center_cell.y, grid->course_columns)];
					bool right = grid->course_grid[from2Dto1Dindex(center_cell.x + 1, center_cell.y, grid->course_columns)];
					bool up = grid->course_grid[from2Dto1Dindex(center_cell.x, center_cell.y - 1, grid->course_columns)];
					bool down = grid->course_grid[from2Dto1Dindex(center_cell.x, center_cell.y + 1, grid->course_columns)];
					
					switch (go0->direction)
					{
					case DIRECTION::LEFT:
						if (left)
							break;
						else
							go0->direction = DIRECTION::RIGHT;
						break;
					case DIRECTION::RIGHT:
						if (right)
							break;
						else
							go0->direction = DIRECTION::LEFT;
						break;
					case DIRECTION::UP:
						if (up)
							break;
						else
							go0->direction = DIRECTION::DOWN;
						break;
					case DIRECTION::DOWN:
						if (down)
							break;
						else
							go0->direction = DIRECTION::UP;
						break;
					case DIRECTION::NONE:
						break;
					default:
						break;
					}
				}
				//Check collision with player
				if ((go0->position.x < player->position.x + (player->dimensions.x - 10)) &&
					(go0->position.x + go0->dimensions.x > (player->position.x + 10)) &&
					(go0->position.y + go0->dimensions.y > (player->position.y + 10)) &&
					(go0->position.y < player->position.y + (player->dimensions.y - 10)))
				{
					go0->Receive(HIT);
					player->Receive(HIT);
				}

				//go0->Receive(HIT);
				
				
			}
		}
		
		if (timer < 0) {
			timer = 1;
		}
		timer -= (dt * game_speed);
}
class QItem {
public:
	int row;
	int col;
	int dist;
	vector<Vector2D> cells;
	QItem(int x, int y, int w, vector<Vector2D> cells)
		: row(x), col(y), dist(w), cells(cells)
	{
	}
};
Vector2D GridCollideComponent::GetNextCell(Vector2D source_cell)
{

	vector<Vector2D> cells;
	cells.push_back(Vector2D(floor(source_cell.x / grid->course_cell_size), floor(source_cell.y / grid->course_cell_size)));

	QItem source(0, 0, 0, cells);

	// To keep track of visited QItems. Marking 
	// blocked cells as visited. 

	bool visited[14][18];    //TODO : Non-static size
	for (int i = 0; i < 14; i++) {
		for (int j = 0; j < 18; j++)
		{
			if (!grid->course_grid[from2Dto1Dindex( i, j, grid->course_columns)])
				visited[i][j] = true;
			else
				visited[i][j] = false;
		}
	}
	source.row = source_cell.x / grid->course_cell_size;
	source.col = source_cell.y / grid->course_cell_size;

	// applying BFS on matrix cells starting from source 
	queue<QItem> q;
	q.push(source);
	visited[source.row][source.col] = true;
	while (!q.empty()) {

		QItem p = q.front();
		q.pop();

		// Destination found, check uppermost and lowermost point of player, just to make it work if player has dug half a block and origin point is technically in a disabled block.
		if ((p.row == floor(player->position.x / grid->course_cell_size) && p.col == floor(player->position.y / grid->course_cell_size))
			|| (p.row == floor((player->position.x + player->dimensions.x - 1) / grid->course_cell_size) && p.col == floor((player->position.y + player->dimensions.y - 1) / grid->course_cell_size))) {
			if(p.cells.size() > 1)
				return p.cells[1];
			return Vector2D(-1, -1);
		}
		// moving up 
		if (p.row - 1 >= 0 &&
			visited[p.row - 1][p.col] == false) {
			vector<Vector2D> cells = p.cells;
			cells.push_back(Vector2D(p.row - 1, p.col));
			q.push(QItem(p.row - 1, p.col, p.dist + 1, cells));
			visited[p.row - 1][p.col] = true;
		}

		// moving down 
		if (p.row + 1 < 14 &&
			visited[p.row + 1][p.col] == false) {
			vector<Vector2D> cells = p.cells;
			cells.push_back(Vector2D(p.row + 1, p.col));
			q.push(QItem(p.row + 1, p.col, p.dist + 1, cells));
			visited[p.row + 1][p.col] = true;
		}

		// moving left 
		if (p.col - 1 >= 0 &&
			visited[p.row][p.col - 1] == false) {
			vector<Vector2D> cells = p.cells;
			cells.push_back(Vector2D(p.row, p.col - 1));
			q.push(QItem(p.row, p.col - 1, p.dist + 1, cells));
			visited[p.row][p.col - 1] = true;
		}

		// moving right 
		if (p.col + 1 < 18 &&
			visited[p.row][p.col + 1] == false) {
			vector<Vector2D> cells = p.cells;
			cells.push_back(Vector2D(p.row, p.col + 1));
			q.push(QItem(p.row, p.col + 1, p.dist + 1, cells));
			visited[p.row][p.col + 1] = true;
		}
	}
	return Vector2D(-1, -1);
}
