#pragma once

#include <iostream>
class Game : public GameObject
{
	std::set<GameObject*> game_objects;	// http://www.cplusplus.com/reference/set/set/
	
	AvancezLib* engine;

	ObjectPool<Rocket> rockets_pool;	// used to instantiate rockets

	ObjectPool<Bomb> bomb_pool;			// used to instantiate bombs

	//ObjectPool<Alien> alien_pool;		// used to instantiate aliens

	Grid * grid;

	Player * player;

	Sprite * life_sprite;
	Sprite* background_sprite;
	bool game_over = false;

	unsigned int score = 0;
	int aliens = 0;				//Tracks how many aliens are left. Can I count the pool somehow?
	float last_time_bomb = 0;	//Last time since bomb.

public:

	virtual void Create(AvancezLib* engine)
	{
		SDL_Log("Game::Create");

		this->engine = engine;
		// Create the player and the behaviour + rendering component + collision
		player = new Player();
	
		PlayerBehaviourComponent* player_behaviour = new PlayerBehaviourComponent();
		player_behaviour->Create(engine, player, &game_objects, &rockets_pool);
		RenderComponent * player_render = new RenderComponent();
		player_render->Create(engine, player, &game_objects, "data/player.bmp");
		//CollideComponent* player_collide = new CollideComponent();
		//CollideComponent* player_collide_2 = new CollideComponent();

		player->Create();
		player->AddComponent(player_behaviour);
		player->AddComponent(player_render);

		grid = new Grid();

		GridComponent* grid_component = new GridComponent(); 
		std::vector<GameObject*> * placeholder = new std::vector<GameObject*>;
		grid_component->Create(engine, grid, &game_objects, player, placeholder /*CHANGE*/, CELL_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT);
		grid->Create();
		grid->AddComponent(grid_component);

		player->AddReceiver(this);

		
		game_objects.insert(grid);
		game_objects.insert(player);

		// Create all rockets and their behaviours + rendering components
		rockets_pool.Create(MAX_NUM_ROCKETS);
		for (auto rocket = rockets_pool.pool.begin(); rocket != rockets_pool.pool.end(); rocket++)
		{
			RocketBehaviourComponent * behaviour = new RocketBehaviourComponent();
			behaviour->Create(engine, *rocket, &game_objects);
			RenderComponent * render = new RenderComponent();
			render->Create(engine, *rocket, &game_objects, "data/rocket.bmp");
			(*rocket)->Create();
			(*rocket)->AddComponent(behaviour);
			(*rocket)->AddComponent(render);
		}
		/*
		// Create all aliens and their behaviours + rendering components + collisions
		alien_pool.Create(55);
		for (auto alien = alien_pool.pool.begin(); alien != alien_pool.pool.end(); alien++)
		{
			AlienBehaviourComponent* alien_behaviour = new AlienBehaviourComponent();
			alien_behaviour->Create(engine, *alien, &game_objects, &bomb_pool);
			RenderComponent* alien_render = new RenderComponent();
			alien_render->Create(engine, *alien, &game_objects, "data/enemy_0.bmp");
			CollideComponent* alien_collide = new CollideComponent();
			alien_collide->Create(engine, *alien, &game_objects, (ObjectPool<GameObject> *) &rockets_pool);

			(*alien)->Create();
			(*alien)->AddComponent(alien_behaviour);
			(*alien)->AddComponent(alien_render);
			(*alien)->AddComponent(alien_collide);
			(*alien)->AddReceiver(this);
			AddReceiver(*alien);
			game_objects.insert(*alien);
		}
		*/
		// Create all bombs and their behaviours + rendering components
		bomb_pool.Create(MAX_NUM_BOMBS);
		for (auto bomb = bomb_pool.pool.begin(); bomb != bomb_pool.pool.end(); bomb++)
		{
			BombBehaviourComponent* behaviour = new BombBehaviourComponent();
			behaviour->Create(engine, *bomb, &game_objects);
			RenderComponent* render = new RenderComponent();
			render->Create(engine, *bomb, &game_objects, "data/bomb.bmp");
			(*bomb)->Create();
			(*bomb)->AddComponent(behaviour);
			(*bomb)->AddComponent(render);
		}
		life_sprite = engine->createSprite("data/player.bmp");
		background_sprite = engine->createSprite("data/background.bmp");
	}
	//Initialize player and aliens
	virtual void Init()
	{
		player->Init();
		grid->Init();
		/*
		int x = 0, y = 25, counter = 0;
		for (auto alien = alien_pool.pool.begin(); alien != alien_pool.pool.end(); alien++)
		{
			x += 40;
			aliens++;
			if (!((counter++) % 11)) {
				y += 32;
				x = 100;
			}
			(*alien)->Init(x, y);
		}
		*/
		enabled = true;
	}
	//Generate the next level
	void nextLevel() {
		/*
		int x = 0, y = 25, counter = 0;
		for (auto alien = alien_pool.pool.begin(); alien != alien_pool.pool.end(); alien++)
		{
			x += 40;
			aliens++;
			if (!((counter++) % 11)) {
				y += 32;
				x = 100;
			}

			(*alien)->Init(x, y);
		}
		*/
		game_speed *= 1.2f;
	}
	//Update the game
	virtual void Update(float dt)
	{
		AvancezLib::KeyStatus keys;
		engine->getKeyStatus(keys);
		if (keys.esc) {
			Destroy();
			engine->quit();
		}
		//std::cout << "hej" << endl;
		float running = 0;  //Used to make the game stop, but still keep the score.
		if (game_over)
			running = 0.0;
		else
			running = 1.0;

		background_sprite->draw(0, 58);

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Update(dt * running);

		player->Update(0);
		/*
		if ((engine->getElapsedTime() - last_time_bomb) >= (BOMB_TIME_INTERVAL / game_speed)) {
			alien_pool.SelectRandom()->fire = true;
			last_time_bomb = engine->getElapsedTime();
		}
		*/
	}
	//Draws the text and health sprite
	virtual void Draw()
	{
		

		char text[100];

		snprintf(text, 100, "%07d", score);
		engine->drawText(300, 32, text);

		snprintf(text, 100, "BONUS: %.1fX", game_speed);
		engine->drawText(510, 32, text);

		for (int i = 0; i < player->lives; i++) {
			life_sprite->draw(32 * i, SCREEN_HEIGHT - 32);
		}

		if (game_over) {
			snprintf(text, 100, "*** G A M E   O V E R ***");
			engine->drawText(250, 10, text);
		}
		
		engine->swapBuffers();
		engine->clearWindow();
	}
	//Handle messages
	virtual void Receive(Message m)
	{
		if (m == GAME_OVER)
		{
			game_over = true;
		}


		if (m == WALL) {
			Send(WALL);
		}
	}
	//Destroy everything
	virtual void Destroy()
	{
		SDL_Log("Game::Destroy");

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Destroy();

		life_sprite->destroy();
	
		rockets_pool.Destroy();
		//alien_pool.Destroy();
		bomb_pool.Destroy();
		delete player;
	}
};