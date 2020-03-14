#pragma once

#include <iostream>
#include <map>

using namespace std;
class Game : public GameObject
{
	std::set<GameObject*> game_objects;	// http://www.cplusplus.com/reference/set/set/
	
	AvancezLib* engine;

	ObjectPool<Rock> rock_pool;			// used to instantiate bombs

	ObjectPool<Flame> flame_pool;		// used to instantiate bombs

	ObjectPool<Pookah> pookah_pool;		// used to instantiate pookahs

	ObjectPool<Fygar> fygar_pool;		// used to instantiate fygars

	Grid * grid;

	Player * player;

	Pump * pump;

	Sprite * life_sprite;
	Sprite * level_sprite;
	Sprite* background_sprite;

	map < string, Mix_Music*> music;
	map < string, Mix_Chunk*> sounds;

	bool game_over = false;
	bool wait = false;

	unsigned int score = 0;
	unsigned int highscore = 0;
	unsigned int enemies = 0;
	unsigned int level = 1;
	unsigned int health_up = 0;
	float timer = 0;	//Timer for flashing text
	bool flashing_text, space_pressed = false;

public:

	virtual void Create(AvancezLib* engine)
	{
		SDL_Log("Game::Create");

		this->engine = engine;


		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
			cout << "Mixer init error: " << Mix_GetError() << endl;
		}
		
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
		player_render->AddSprite("data/sprites/player_death_0.png", DYING);
		player_render->AddSprite("data/sprites/player_death_1.png", DYING);
		player_render->AddSprite("data/sprites/player_death_2.png", DYING);
		player_render->AddSprite("data/sprites/player_death_3.png", DYING);
		player_render->AddSprite("data/sprites/player_death_4.png", DYING);

		CollideComponent* flame_collide_component = new CollideComponent();
		flame_collide_component->Create(engine, player, &game_objects, (ObjectPool<GameObject>*) & flame_pool);
		
		CollideComponent* pookah_player_collide = new CollideComponent();
		pookah_player_collide->Create(engine, player, &game_objects, (ObjectPool<GameObject>*) & pookah_pool);

		CollideComponent* fygar_player_collide = new CollideComponent();
		fygar_player_collide->Create(engine, player, &game_objects, (ObjectPool<GameObject>*) & fygar_pool);

		player->Create();
		player->AddComponent(player_behaviour);
		player->AddComponent(player_render);
		player->AddComponent(flame_collide_component);
		player->AddComponent(pookah_player_collide);
		player->AddComponent(fygar_player_collide);

		RenderComponent* pump_render = new RenderComponent();
		pump_render->Create(engine, pump, &game_objects, "data/sprites/pump_0.png", 0.f);
		PumpBehaviourComponent* pump_behaviour = new PumpBehaviourComponent();
		pump_behaviour->Create(engine, pump, &game_objects);

		pump->Create();
		pump->AddComponent(pump_behaviour);
		pump->AddComponent(pump_render);

		game_objects.insert(pump);

		

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

		// Create all rocks and their behaviours + rendering components
		flame_pool.Create(10);
		for (auto flame = flame_pool.pool.begin(); flame != flame_pool.pool.end(); flame++)
		{
			FlameBehaviourComponent* behaviour_component = new FlameBehaviourComponent();
			behaviour_component->Create(engine, *flame, &game_objects);
			RenderComponent* render_component = new RenderComponent();
			render_component->Create(engine, *flame, &game_objects, "data/sprites/flame_0.png", 0.f);
			render_component->AddSprite("data/sprites/flame_0.png", DYING);
			render_component->AddSprite("data/sprites/flame_1.png", DYING);
			render_component->AddSprite("data/sprites/flame_2.png", DYING);

			GridCollideComponent* grid_collide_component = new GridCollideComponent();
			grid_collide_component->Create(engine, grid, &game_objects, *flame, player);

			(*flame)->Create();
			(*flame)->AddComponent(behaviour_component);
			(*flame)->AddComponent(render_component);
			(*flame)->AddComponent(grid_collide_component);
			(*flame)->AddReceiver(this);
			(*flame)->AddReceiver(pump);
			game_objects.insert(*flame);
		}
		
		// Create all rocks and their behaviours + rendering components
		rock_pool.Create(1);
		for (auto rock = rock_pool.pool.begin(); rock != rock_pool.pool.end(); rock++)
		{
			RockBehaviourComponent* rock_behaviour_component = new RockBehaviourComponent();
			rock_behaviour_component->Create(engine, *rock, &game_objects);
			RenderComponent* rock_render_component = new RenderComponent();
			rock_render_component->Create(engine, *rock, &game_objects, "data/sprites/rock.png", 0.f);
			rock_render_component->AddSprite("data/sprites/rock.png", CRUSHED);

			GridRockCollideComponent* grid_collide_component = new GridRockCollideComponent();
			grid_collide_component->Create(engine, grid, &game_objects, *rock);


			(*rock)->Create();
			(*rock)->AddComponent(rock_behaviour_component);
			(*rock)->AddComponent(rock_render_component);
			(*rock)->AddComponent(grid_collide_component);
			(*rock)->AddReceiver(this);
			(*rock)->AddReceiver(pump);
			player->AddReceiver(*rock);
			AddReceiver(*rock);
			game_objects.insert(*rock);
		}
		// Create all pookahs and their behaviours + rendering components
		pookah_pool.Create(3);
		for (auto pookah = pookah_pool.pool.begin(); pookah != pookah_pool.pool.end(); pookah++)
		{
			PookahBehaviourComponent* behaviour_component = new PookahBehaviourComponent();
			behaviour_component->Create(engine, *pookah, &game_objects);
			RenderComponent* render_component = new RenderComponent();
			render_component->Create(engine, *pookah, &game_objects, "data/sprites/pookah_walk_0.png", 3.f);
			render_component->AddSprite("data/sprites/pookah_walk_1.png", WALKING);
			render_component->AddSprite("data/sprites/pookah_digging_0.png", DIGGING);
			render_component->AddSprite("data/sprites/pookah_digging_1.png", DIGGING);
			render_component->AddSprite("data/sprites/pookah_inflate_0.png", DYING);
			render_component->AddSprite("data/sprites/pookah_inflate_1.png", DYING);
			render_component->AddSprite("data/sprites/pookah_inflate_2.png", DYING);
			render_component->AddSprite("data/sprites/pookah_inflate_3.png", DYING);
			render_component->AddSprite("data/sprites/pookah_crushed.png", CRUSHED);

			GridPathComponent* grid_path_component = new GridPathComponent();
			grid_path_component->Create(engine, grid, &game_objects, *pookah, player);

			GridCollideComponent* grid_collide_component = new GridCollideComponent();
			grid_collide_component->Create(engine, grid, &game_objects, *pookah, player);

			CollideComponent* collide_component = new CollideComponent();
			collide_component->Create(engine, *pookah, &game_objects, (ObjectPool<GameObject>*) & rock_pool);


			(*pookah)->Create();
			(*pookah)->AddComponent(behaviour_component);
			(*pookah)->AddComponent(render_component);
			(*pookah)->AddComponent(collide_component);
			(*pookah)->AddComponent(grid_collide_component);
			(*pookah)->AddComponent(grid_path_component);
			(*pookah)->AddReceiver(this);
			(*pookah)->AddReceiver(pump);
			player->AddReceiver(*pookah);
			AddReceiver(*pookah);
			game_objects.insert(*pookah);
			enemies++;
		}
		
		// Create all fygars and their behaviours + rendering components
		
		fygar_pool.Create(1);
		for (auto fygar = fygar_pool.pool.begin(); fygar != fygar_pool.pool.end(); fygar++)
		{
			FygarBehaviourComponent* behaviour_component = new FygarBehaviourComponent();
			behaviour_component->Create(engine, *fygar, &game_objects, &flame_pool);
			RenderComponent* render_component = new RenderComponent();
			render_component->Create(engine, *fygar, &game_objects, "data/sprites/fygar_walk_0.png", 6.f);
			render_component->AddSprite("data/sprites/fygar_walk_1.png", WALKING);
			render_component->AddSprite("data/sprites/fygar_fire_0.png", ATTACKING);
			render_component->AddSprite("data/sprites/fygar_digging_0.png", DIGGING);
			render_component->AddSprite("data/sprites/fygar_digging_1.png", DIGGING);
			render_component->AddSprite("data/sprites/fygar_inflate_0.png", DYING);
			render_component->AddSprite("data/sprites/fygar_inflate_1.png", DYING);
			render_component->AddSprite("data/sprites/fygar_inflate_2.png", DYING);
			render_component->AddSprite("data/sprites/fygar_inflate_3.png", DYING);
			render_component->AddSprite("data/sprites/fygar_crushed.png", CRUSHED);
			GridPathComponent* grid_path_component = new GridPathComponent();
			grid_path_component->Create(engine, grid, &game_objects, *fygar, player);

			GridCollideComponent* grid_collide_component = new GridCollideComponent();
			grid_collide_component->Create(engine, grid, &game_objects, *fygar, player);

			CollideComponent* collide_component = new CollideComponent();
			collide_component->Create(engine, *fygar, &game_objects, (ObjectPool<GameObject>*) & rock_pool);

			(*fygar)->Create();
			(*fygar)->AddComponent(behaviour_component);
			(*fygar)->AddComponent(render_component);
			(*fygar)->AddComponent(collide_component);
			(*fygar)->AddComponent(grid_collide_component);
			(*fygar)->AddComponent(grid_path_component);
			(*fygar)->AddReceiver(this);
			(*fygar)->AddReceiver(pump);
			player->AddReceiver(*fygar);
			AddReceiver(*fygar);
			game_objects.insert(*fygar);
			enemies++;
		}
		
		
		PumpCollideComponent* pookah_pump_collide = new PumpCollideComponent();
		pookah_pump_collide->Create(engine, pump, &game_objects, (ObjectPool<GameObject>*) &pookah_pool);
		pump->AddComponent(pookah_pump_collide);

		PumpCollideComponent* fygar_pump_collide = new PumpCollideComponent();
		fygar_pump_collide->Create(engine, pump, &game_objects, (ObjectPool<GameObject>*) &fygar_pool);
		pump->AddComponent(fygar_pump_collide);
		
		GridCollideComponent* pump_grid_collide = new GridCollideComponent();
		pump_grid_collide->Create(engine, grid, &game_objects, pump, player);
		pump->AddComponent(pump_grid_collide);

		life_sprite = engine->createSprite("data/sprites/player_digging_0.png");
		level_sprite = engine->createSprite("data/sprites/flower.png");
		background_sprite = engine->createSprite("data/background.bmp");


		LoadMusic("song.wav");
		PlayMusic(music["song.wav"]);
		LoadSound("level_clear.wav");
		LoadSound("pump.wav");
		LoadSound("monster_burst.wav");
		LoadSound("rock_fall.wav");
		LoadSound("game_over.wav");
		LoadSound("fygar_fire.wav");
		LoadSound("death.wav");

	}
	virtual void LoadSound(string path) {
		string fullpath = "data/sounds/";
		fullpath.append(path);
		sounds[path] = Mix_LoadWAV(fullpath.c_str());
		if (sounds[path] == NULL)
			cout << "Failed to load " << fullpath.c_str() << ", " << Mix_GetError() << endl;
	}
	virtual void LoadMusic(string path) {
		string fullpath = "data/sounds/";
		fullpath.append(path);
		music[path] = Mix_LoadMUS(fullpath.c_str());
		if (music[path] == NULL)
			cout << "Failed to load " << fullpath.c_str() << ", " << Mix_GetError() << endl;
	}
	virtual void PlayMusic(Mix_Music* music) {
		Mix_PlayMusic(music, -1);
	}
	virtual void PauseMusic() {
		if (Mix_PlayingMusic())
			Mix_PauseMusic();
	}
	virtual void ResumeMusic() {
		if (Mix_PausedMusic())
			Mix_ResumeMusic();
	}

	virtual void PlaySound(Mix_Chunk* sound, int channel, int iterations) {
		Mix_PlayChannel(channel, sound, iterations);
	}
	virtual void StopSound(int channel) {
		Mix_HaltChannel(channel);
	}
	//Initialize player and aliens
	virtual void Init()
	{
		player->Init(6 * CELL_SIZE, 9 * CELL_SIZE);
		grid->Init(0,0);
		
		pookah_pool.pool.at(0)->Init(2 * CELL_SIZE, 5 * CELL_SIZE);
		pookah_pool.pool.at(0)->direction = DIRECTION::DOWN;
		pookah_pool.pool.at(1)->Init(10 * CELL_SIZE, 4 * CELL_SIZE);
		pookah_pool.pool.at(1)->direction = DIRECTION::RIGHT;
		pookah_pool.pool.at(2)->Init(9 * CELL_SIZE, 12 * CELL_SIZE);
		pookah_pool.pool.at(2)->direction = DIRECTION::DOWN;
		
		fygar_pool.pool.at(0)->Init(2 * CELL_SIZE, 12 * CELL_SIZE);
		fygar_pool.pool.at(0)->direction = DIRECTION::RIGHT;

		rock_pool.pool.at(0)->Init(7 * CELL_SIZE, 7 * CELL_SIZE);
		rock_pool.pool.at(0)->direction = DIRECTION::NONE;

		pump->enabled = false;
		enabled = true;
	}
	//Generate the next level
	virtual void nextLevel() {
		level++;
		enemies = 4;

		PlaySound(sounds["level_clear.wav"], 2, 0);
		player->ResetPosition();

		grid->Reset();

		pookah_pool.pool.at(0)->Init(2 * CELL_SIZE, 5 * CELL_SIZE);
		pookah_pool.pool.at(0)->direction = DIRECTION::DOWN;
		pookah_pool.pool.at(1)->Init(10 * CELL_SIZE, 4 * CELL_SIZE);
		pookah_pool.pool.at(1)->direction = DIRECTION::RIGHT;
		pookah_pool.pool.at(2)->Init(9 * CELL_SIZE, 12 * CELL_SIZE);
		pookah_pool.pool.at(2)->direction = DIRECTION::DOWN;

		fygar_pool.pool.at(0)->Init(2 * CELL_SIZE, 12 * CELL_SIZE);
		fygar_pool.pool.at(0)->direction = DIRECTION::RIGHT;

		rock_pool.pool.at(0)->Init(7 * CELL_SIZE, 7 * CELL_SIZE);
		rock_pool.pool.at(0)->direction = DIRECTION::NONE;

		pump->enabled = false;
		
		POOKAH_SPEED *= 1.2f;
		FYGAR_SPEED *= 1.2f;
	}
	virtual void reset() {
		level = 1;
		enemies = 4;
		pump->enabled = false;
		player->lives = NUM_LIVES;
		player->ResetPosition();

		grid->Reset();

		pookah_pool.pool.at(0)->Init(2 * CELL_SIZE, 5 * CELL_SIZE);
		pookah_pool.pool.at(0)->direction = DIRECTION::DOWN;
		pookah_pool.pool.at(1)->Init(10 * CELL_SIZE, 4 * CELL_SIZE);
		pookah_pool.pool.at(1)->direction = DIRECTION::RIGHT;
		pookah_pool.pool.at(2)->Init(9 * CELL_SIZE, 12 * CELL_SIZE);
		pookah_pool.pool.at(2)->direction = DIRECTION::DOWN;

		fygar_pool.pool.at(0)->Init(2 * CELL_SIZE, 12 * CELL_SIZE);
		fygar_pool.pool.at(0)->direction = DIRECTION::RIGHT;

		rock_pool.pool.at(0)->Init(7 * CELL_SIZE, 7 * CELL_SIZE);
		rock_pool.pool.at(0)->direction = DIRECTION::NONE;
		POOKAH_SPEED = 80.f;
		FYGAR_SPEED = 80.f;
		score = 0;

	}
	virtual void playerDeath() {
		

		for (auto go = fygar_pool.pool.begin(); go != fygar_pool.pool.end(); go++) {
			if ((*go)->enabled)
				(*go)->ResetPosition();
		}
		for (auto go = pookah_pool.pool.begin(); go != pookah_pool.pool.end(); go++) {
			if ((*go)->enabled)
				(*go)->ResetPosition();
		}

		player->ResetPosition();
		pump->enabled = false;
		
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
		else if (keys.enter && game_over) {
			reset();
			game_over = false;
		}
		else if (!space_pressed && pump->enabled && !game_over) {
			PauseMusic();
			PlaySound(sounds["pump.wav"], 1, -1);
			space_pressed = true;
		}
		else if (game_over || !pump->enabled) {
			StopSound(1);
			space_pressed = false;
		}
		if (player->moving && !game_over) {
			ResumeMusic();
			space_pressed = false;
			StopSound(1);
		}
		else{
			PauseMusic();
		}
		
		float running = 0;  //Used to make the game stop, but still keep the score.

		if (game_over)
			running = 0.0;
		else
			running = 1.0;

		background_sprite->draw(0, 0);

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Update(dt * running);


		// All of these are just to ensure they render on top of everything else... Ugly solution!
		for (auto go = rock_pool.pool.begin(); go != rock_pool.pool.end(); go++)
			(*go)->Update(0);
		for (auto go = fygar_pool.pool.begin(); go != fygar_pool.pool.end(); go++)
			(*go)->Update(0);
		for (auto go = pookah_pool.pool.begin(); go != pookah_pool.pool.end(); go++)
			(*go)->Update(0);
		for (auto go = flame_pool.pool.begin(); go != flame_pool.pool.end(); go++)
			(*go)->Update(0);

		pump->Update(0);

		player->Update(0);

		bool reset = true;
		for (auto go = fygar_pool.pool.begin(); go != fygar_pool.pool.end(); go++) {
			if ((*go)->enabled)
				reset = false;
		}
		for (auto go = pookah_pool.pool.begin(); go != pookah_pool.pool.end(); go++) {
			if ((*go)->enabled)
				reset = false;
		}
		//Instead of having it message based. This is because messages were handled in game.h before they were handled in enemies. So game got reset, 
		//but then enemies got disabled afterwards, because the message was handled by them too late
		if (reset)
			nextLevel();

		timer -= dt * game_speed;
		if (timer < 0) {
			timer = 1.5;
			flashing_text = false;
		}
		else if(timer < 0.75f)
			flashing_text = true;
		
	
	}
	//Draws the text and health sprite
	virtual void Draw()
	{
		SDL_Color red = { 255, 0, 0 };
		char text[100];

		if(flashing_text)
			engine->drawText(60, 17, "1UP", { 255, 0, 0 }, 16);

		snprintf(text, 100, "%d", score);
		engine->drawText(70, 34, text, { 255, 255, 255 }, 16);

		engine->drawText(SCREEN_WIDTH/2 - 80, 17, "HIGH SCORE", { 255, 0, 0 }, 16);

		snprintf(text, 100, "%d", highscore);
		engine->drawText(SCREEN_WIDTH / 2 - 50, 34, text, { 255, 255, 255 }, 16);

		engine->drawText(SCREEN_WIDTH - 85, SCREEN_HEIGHT - CELL_SIZE, "ROUND", { 255, 255, 255 }, 16);

		snprintf(text, 100, "%d", level);
		engine->drawText(SCREEN_WIDTH - CELL_SIZE, SCREEN_HEIGHT - CELL_SIZE/2, text, { 255, 255, 255 }, 16);


		for (int i = 0; i < player->lives; i++) {
			life_sprite->draw(CELL_SIZE * i, SCREEN_HEIGHT - CELL_SIZE);
		}

		for (int i = level; i > 0; i--) {
			level_sprite->draw(SCREEN_WIDTH - CELL_SIZE * i,  CELL_SIZE * 2);
		}

		if (game_over) {
			snprintf(text, 100, "*** G A M E   O V E R ***");
			engine->drawText(13, SCREEN_HEIGHT / 2 - 20, text, { 255, 255, 255 }, 17);
			engine->drawText(75, SCREEN_HEIGHT / 2, "Press enter to restart", { 255, 255, 255 }, 14);
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
			PauseMusic();
			PlaySound(sounds["game_over.wav"], 6, 0);
		}
		if (m == PLAYER_HIT) {
			PlaySound(sounds["death.wav"], 7, 0);
		}
		if (m == PLAYER_DEAD) {
			playerDeath();
		}
		if (m == WALL) {
			Send(WALL);
		}
		if (m == SCORE_UP) {
			score += 10;
			health_up += 10;
		}
		if (m == POOKAH_BURST) {
			score += 200;
			health_up += 200;
			PlaySound(sounds["monster_burst.wav"], 3, 0);
		}
		if (m == FYGAR_BURST) {
			score += 400;
			health_up += 400;
			PlaySound(sounds["monster_burst.wav"], 4, 0);
			
		}
		if (m == ROCK_LOOSE) {
			PlaySound(sounds["rock_fall.wav"], 5, 0);
		}
		if (m == FYGAR_FIRE) {
			cout << "wooot" << endl;
			PlaySound(sounds["fygar_fire.wav"], 6, 0);
		}
		if (score > highscore)
			highscore = score;

		if (health_up >= 2000) {
			player->lives++;
			health_up = 0;
		}

			

	}
	//Destroy everything
	virtual void Destroy()
	{
		SDL_Log("Game::Destroy");

		Mix_Quit();

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Destroy();

		life_sprite->destroy();
	
		rock_pool.Destroy();

		fygar_pool.Destroy();

		pookah_pool.Destroy();

		flame_pool.Destroy();

		delete player;
		delete pump;
		delete grid;
	}
};