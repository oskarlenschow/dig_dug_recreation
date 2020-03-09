#pragma once

#include <iostream>
class Game : public GameObject
{
	std::set<GameObject*> game_objects;	// http://www.cplusplus.com/reference/set/set/
	
	AvancezLib* engine;

	ObjectPool<Rock> rock_pool;			// used to instantiate bombs

	ObjectPool<Pookah> pookah_pool;		// used to instantiate pookahs

	ObjectPool<Fygar> fygar_pool;		// used to instantiate fygars

	Grid * grid;

	Player * player;

	Pump * pump;

	Sprite * life_sprite;
	Sprite * level_sprite;
	Sprite* background_sprite;

	bool game_over = false;
	bool wait = false;

	unsigned int score = 0;
	unsigned int level = 1;
	float last_time_bomb = 0;	//Last time since bomb.
	bool flashing_text;

public:

	virtual void Create(AvancezLib* engine)
	{
		SDL_Log("Game::Create");

		this->engine = engine;
		// Create the player and the behaviour + rendering component + collision
		player = new Player();
		pump = new Pump();
	
		PlayerBehaviourComponent* player_behaviour = new PlayerBehaviourComponent();
		player_behaviour->Create(engine, player, &game_objects, pump);
		RenderComponent * player_render = new RenderComponent();
		player_render->Create(engine, player, &game_objects, "data/sprites/player_walk_0.png", 15.f);
		player_render->AddSprite("data/sprites/player_walk_1.png", WALKING);
		player_render->AddSprite("data/sprites/player_digging_0.png", DIGGING);
		player_render->AddSprite("data/sprites/player_digging_1.png", DIGGING);
		player_render->AddSprite("data/sprites/player_walk_1.png", IDLE);
		player_render->AddSprite("data/sprites/player_pumping_0.png", ATTACKING);
		player_render->AddSprite("data/sprites/player_pumping_1.png", ATTACKING);
		
		RenderComponent* pump_render = new RenderComponent();
		pump_render->Create(engine, pump, &game_objects, "data/sprites/pump_0.png", 0.f);
		PumpBehaviourComponent* pump_behaviour = new PumpBehaviourComponent();
		pump_behaviour->Create(engine, pump, &game_objects);

		pump->Create();
		pump->AddComponent(pump_behaviour);
		pump->AddComponent(pump_render);
	

		player->Create();
		player->AddComponent(player_behaviour);
		player->AddComponent(player_render);

		grid = new Grid();

		GridPlayerComponent* grid_player_component = new GridPlayerComponent(); 
		grid_player_component->Create(engine, grid, &game_objects, player);

		GridRenderComponent* grid_render_component = new GridRenderComponent();
		grid_render_component->Create(engine, grid, &game_objects);

		grid->Create(CELL_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT, 4);
		grid->AddComponent(grid_player_component);
		grid->AddComponent(grid_render_component);
		grid->AddReceiver(this);



		player->AddReceiver(this);
		game_objects.insert(grid);
		game_objects.insert(player);

		// Create all pookahs and their behaviours + rendering components
		
		pookah_pool.Create(3);
		for (auto pookah = pookah_pool.pool.begin(); pookah != pookah_pool.pool.end(); pookah++)
		{
			PookahBehaviourComponent* pookah_behaviour_component = new PookahBehaviourComponent();
			pookah_behaviour_component->Create(engine, *pookah, &game_objects);
			RenderComponent* pookah_render_component = new RenderComponent();
			pookah_render_component->Create(engine, *pookah, &game_objects, "data/sprites/pookah_walk_0.png", 3.f);
			pookah_render_component->AddSprite("data/sprites/pookah_walk_1.png", WALKING);
			pookah_render_component->AddSprite("data/sprites/pookah_digging_0.png", DIGGING);
			pookah_render_component->AddSprite("data/sprites/pookah_digging_1.png", DIGGING);

			(*pookah)->Create();
			(*pookah)->AddComponent(pookah_behaviour_component);
			(*pookah)->AddComponent(pookah_render_component);
			(*pookah)->AddReceiver(this);
			AddReceiver(*pookah);  //NOT NEEDED NOW?
			game_objects.insert(*pookah);
		}
		
		// Create all fygars and their behaviours + rendering components

		fygar_pool.Create(1);
		for (auto fygar = fygar_pool.pool.begin(); fygar != fygar_pool.pool.end(); fygar++)
		{
			FygarBehaviourComponent* fygar_behaviour_component = new FygarBehaviourComponent();
			fygar_behaviour_component->Create(engine, *fygar, &game_objects, &rock_pool);
			RenderComponent* fygar_render_component = new RenderComponent();
			fygar_render_component->Create(engine, *fygar, &game_objects, "data/sprites/fygar_walk_0.png", 6.f);
			fygar_render_component->AddSprite("data/sprites/fygar_walk_1.png", WALKING);
			fygar_render_component->AddSprite("data/sprites/fygar_fire_0.png", ATTACKING);
			fygar_render_component->AddSprite("data/sprites/fygar_digging_0.png", DIGGING);
			fygar_render_component->AddSprite("data/sprites/fygar_digging_1.png", DIGGING);

			(*fygar)->Create();
			(*fygar)->AddComponent(fygar_behaviour_component);
			(*fygar)->AddComponent(fygar_render_component);
			(*fygar)->AddReceiver(this);
			AddReceiver(*fygar);  //NOT NEEDED NOW?
			game_objects.insert(*fygar);
		}
		/*
		CollideComponent* pookah_collide_component = new CollideComponent();
		pookah_collide_component->Create(engine, player, &game_objects, (ObjectPool<GameObject>*) &pookah_pool);
		player->AddComponent(pookah_collide_component);

		CollideComponent* fygar_collide_component = new CollideComponent();
		fygar_collide_component->Create(engine, player, &game_objects, (ObjectPool<GameObject>*) &fygar_pool);
		player->AddComponent(fygar_collide_component);
		*/
		vector<ObjectPool<GameObject>*> collision_pools;
		collision_pools.push_back((ObjectPool<GameObject>*) &pookah_pool);
		collision_pools.push_back((ObjectPool<GameObject>*) &fygar_pool);

		GridCollideComponent* grid_collide_component = new GridCollideComponent();
		grid_collide_component->Create(engine, grid, &game_objects, collision_pools, player);

		grid->AddComponent(grid_collide_component);

		/*
		bomb_pool.Create(MAX_NUM_BOMBS);
		for (auto bomb = bomb_pool.pool.begin(); bomb != bomb_pool.pool.end(); bomb++)
		{
			BombBehaviourComponent* behaviour = new BombBehaviourComponent();
			behaviour->Create(engine, *bomb, &game_objects);
			RenderComponent* render = new RenderComponent();
			render->Create(engine, *bomb, &game_objects, "data/bomb.bmp", 0.f);
			(*bomb)->Create();
			(*bomb)->AddComponent(behaviour);
			(*bomb)->AddComponent(render);
		}
		*/
		life_sprite = engine->createSprite("data/sprites/player_digging_0.png");
		level_sprite = engine->createSprite("data/sprites/flower.png");
		background_sprite = engine->createSprite("data/background.bmp");
	}
	//Initialize player and aliens
	virtual void Init()
	{
		player->Init(0,0);
		grid->Init(0,0);
		
		pookah_pool.pool.at(0)->Init(2 * CELL_SIZE, 5 * CELL_SIZE);
		pookah_pool.pool.at(1)->Init(10 * CELL_SIZE, 4 * CELL_SIZE);
		pookah_pool.pool.at(2)->Init(9 * CELL_SIZE, 12 * CELL_SIZE);
		
		fygar_pool.pool.at(0)->Init(2 * CELL_SIZE, 12 * CELL_SIZE);

		enabled = true;
	}
	//Generate the next level
	void nextLevel() {
		level++;

		player->position.x = 6 * CELL_SIZE;
		player->position.y = 9 * CELL_SIZE;


		grid->Reset();
		
		POOKAH_SPEED *= 1.2f;
		FYGAR_SPEED *= 1.2f;
	}
	//Update the game
	virtual void Update(float dt)
	{
		AvancezLib::KeyStatus keys;

		int elapsed_time = engine->getElapsedTime() * 1000.f;

		if (elapsed_time % 1000 == 0)
			cout << elapsed_time << endl;
			//flashing_text = !flashing_text;

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

		background_sprite->draw(0, 0);

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Update(dt * running);

		for (auto go = fygar_pool.pool.begin(); go != fygar_pool.pool.end(); go++)
			(*go)->Update(0);

		for (auto go = pookah_pool.pool.begin(); go != pookah_pool.pool.end(); go++)
			(*go)->Update(0);

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
		SDL_Color red = { 255, 0, 0 };
		char text[100];

		if(!flashing_text)
			engine->drawText(60, 17, "1UP", { 255, 0, 0 }, 16);

		snprintf(text, 100, "%d", score);
		engine->drawText(70, 34, text, { 255, 255, 255 }, 16);

		engine->drawText(SCREEN_WIDTH/2 - 80, 17, "HIGH SCORE", { 255, 0, 0 }, 16);


		for (int i = 0; i < player->lives; i++) {
			life_sprite->draw(CELL_SIZE * i, SCREEN_HEIGHT - CELL_SIZE);
		}

		for (int i = level; i > 0; i--) {
			level_sprite->draw(SCREEN_WIDTH - CELL_SIZE * i,  CELL_SIZE * 2);
		}

		if (game_over) {
			snprintf(text, 100, "*** G A M E   O V E R ***");
			engine->drawText(250, 10, text, { 255, 255, 255 }, 25);
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
		if (m == SCORE_UP) {
			score += 10;
			if (score >= 300) {
				nextLevel();
				score = 0;
			}
		}
	}
	//Destroy everything
	virtual void Destroy()
	{
		SDL_Log("Game::Destroy");

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Destroy();

		life_sprite->destroy();
	
		rock_pool.Destroy();

		fygar_pool.Destroy();
		pookah_pool.Destroy();

		delete player;
		delete pump;
		delete grid;
	}
};