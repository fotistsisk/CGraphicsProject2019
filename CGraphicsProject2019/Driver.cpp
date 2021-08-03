#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <iostream>
#include <chrono>
#include "GLEW\glew.h"
#include "Renderer.h"
#include <stdio.h>



/*
for later

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
*/

//Screen attributes
SDL_Window * window;

//OpenGL context 
SDL_GLContext gContext;
const int SCREEN_WIDTH = 1380;	//800;	//640;
const int SCREEN_HEIGHT = 1024;	//600;	//480;

//Event handler
SDL_Event event;

Renderer * renderer = nullptr;

Level* level = nullptr;


void func()
{
	system("pause");
}

// initialize SDL and OpenGL
bool init()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		return false;
	}

	// Initialize SDL TTF
	if (TTF_Init() == -1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		return false;
	}

	// use Double Buffering
	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0)
		std::cout << "Error: No double buffering" << std::endl;

	// set OpenGL Version (3.3)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Create Window
	window = SDL_CreateWindow("Graphics Project 2019", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL)
	{
		printf("Could not create window: %s\n", SDL_GetError());
		return false;
	}

	//Create OpenGL context
	gContext = SDL_GL_CreateContext(window);
	if (gContext == NULL)
	{
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	// Disable Vsync
	if (SDL_GL_SetSwapInterval(0) == -1)
		printf("Warning: Unable to disable VSync! SDL Error: %s\n", SDL_GetError());

	// Initialize GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		printf("Error loading GLEW\n");
		return false;
	}
	// some versions of glew may cause an opengl error in initialization
	glGetError();

	renderer = new Renderer();

	//Initialize Game Level
	level = new Level();
	renderer->SetLevel(level);

	

	bool engine_initialized = renderer->Init(SCREEN_WIDTH, SCREEN_HEIGHT);
	
	//atexit(func);
	return engine_initialized;
}


void clean_up()
{
	delete renderer;
	delete level;

	SDL_GL_DeleteContext(gContext);
	SDL_DestroyWindow(window);

	TTF_Quit();

	SDL_Quit();
}

int main(int argc, char *argv[])
{
	//Initialize
	if (init() == false)
	{
		system("pause");
		return EXIT_FAILURE;
	}

	//Quit flag
	bool quit = false;
	bool mouse_button_pressed = false;
	glm::vec2 prev_mouse_position(0);
	

	//timers for the towers
	//auto add_tower_timer_start = std::chrono::steady_clock::now();
	//auto change_tower_timer_start = std::chrono::steady_clock::now();
	bool shouldChange = false;

	//timer for cannonballs
	//auto add_cannonballs_timer_start = std::chrono::steady_clock::now();

	//pirate movement
	auto pirate_timer_start = std::chrono::steady_clock::now();

	//auto pirate_timer_enter_start = std::chrono::steady_clock::now();
	

	auto simulation_start = std::chrono::steady_clock::now();

	unsigned int  killPiratesTime = 0, currentTime, timePaused, timeRender = 0;
	unsigned int deletePiratesTime = 0, resetHitTime = 0, towersShootTime = 0, goldParticlesTime = 0, addTowerTime = 0, changeTowerTime = 0, pirateAddTime = 0 ;

	// Wait for user exit
	while (quit == false)
	{

		if (level->getGO())
		{
			level->setPaused(true);
		}
		// While there are events to handle
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				// Key down events
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
				else if (event.key.keysym.sym == SDLK_r) {
					//check secs!!
					if (shouldChange) {
						level->changeTileOfTower();
						printf("Changed!\n");
						shouldChange = false;
					}
				}
				else if (event.key.keysym.sym == SDLK_h)
				{
					renderer->ReloadShaders();
				}
				else if (event.key.keysym.sym == SDLK_t) {
					// check secs !!!!
					level->addTower();
				}
				else if (event.key.keysym.sym == SDLK_u)
				{
					level->upgradeTower();
					level->setUpgradeTower(false);
				}

				else if (event.key.keysym.sym == SDLK_l) renderer->SetRenderingMode(Renderer::RENDERING_MODE::LINES);
				else if (event.key.keysym.sym == SDLK_p) renderer->SetRenderingMode(Renderer::RENDERING_MODE::POINTS);
				else if (event.key.keysym.sym == SDLK_w)
				{
					renderer->CameraMoveForward(true);
				}
				else if (event.key.keysym.sym == SDLK_s)
				{
					renderer->CameraMoveBackWard(true);
				}
				else if (event.key.keysym.sym == SDLK_a)
				{
					renderer->CameraMoveLeft(true);
				}
				else if (event.key.keysym.sym == SDLK_d)
				{
					renderer->CameraMoveRight(true);
				}
				else if (event.key.keysym.sym == SDLK_UP && level->getSelectionTileZ() + 1 < 10) {
					level->setSelectionTileZ(level->getSelectionTileZ() + 1);
					//level->checkSelectionTile();
				}
				else if (event.key.keysym.sym == SDLK_DOWN && level->getSelectionTileZ() - 1 >= 0) {
					level->setSelectionTileZ(level->getSelectionTileZ() - 1);
				}
				else if (event.key.keysym.sym == SDLK_LEFT && level->getSelectionTileX() + 1 < 10) {
					level->setSelectionTileX(level->getSelectionTileX() + 1);
				}
				else if (event.key.keysym.sym == SDLK_RIGHT && level->getSelectionTileX() - 1 >= 0) {
					level->setSelectionTileX(level->getSelectionTileX() - 1);
				}
			}
			else if (event.type == SDL_KEYUP)
			{
				if (event.key.keysym.sym == SDLK_w)
				{
					renderer->CameraMoveForward(false);
				}
				else if (event.key.keysym.sym == SDLK_s)
				{
					renderer->CameraMoveBackWard(false);
				}
				else if (event.key.keysym.sym == SDLK_a)
				{
					renderer->CameraMoveLeft(false);
				}
				else if (event.key.keysym.sym == SDLK_d)
				{
					renderer->CameraMoveRight(false);
				}

			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				int x = event.motion.x;
				int y = event.motion.y;
				if (mouse_button_pressed)
				{
					renderer->CameraLook(glm::vec2(x, y) - prev_mouse_position);
					prev_mouse_position = glm::vec2(x, y);
				}
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					int x = event.button.x;
					int y = event.button.y;
					mouse_button_pressed = (event.type == SDL_MOUSEBUTTONDOWN);
					prev_mouse_position = glm::vec2(x, y);
				}
			}
			else if (event.type == SDL_MOUSEWHEEL)
			{
				int x = event.wheel.x;
				int y = event.wheel.y;
			}
			else if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					renderer->ResizeBuffers(event.window.data1, event.window.data2);
				}
			}
		}



		float dt = 0.0f;

		if(!level->isPaused())
		{

			if (level->getWasPaused()) {
				timePaused = SDL_GetTicks() - timePaused;
				deletePiratesTime  += timePaused;
				resetHitTime += timePaused;
				towersShootTime  += timePaused;
				goldParticlesTime  += timePaused;
				addTowerTime += timePaused;
				changeTowerTime += timePaused;
				pirateAddTime += timePaused;
			}
			else {
				timePaused = 0;
			}

			// Compute the ellapsed time
			auto simulation_end = std::chrono::steady_clock::now();
			dt = std::chrono::duration <float>(simulation_end - simulation_start).count(); // in seconds
			simulation_start = std::chrono::steady_clock::now();

			//check timers for towers
			//auto add_tower_timer_end = std::chrono::steady_clock::now();
			//float dtAddTower = std::chrono::duration <float>(add_tower_timer_end - add_tower_timer_start).count(); // in seconds
			//std::cout<<"Seconds for next tower: " << dtAddTower <<std::endl;


			//auto change_tower_timer_end = std::chrono::steady_clock::now();
			//float dtChangeTower = std::chrono::duration <float>(change_tower_timer_end - change_tower_timer_start).count(); // in seconds
			//std::cout << "Seconds for next change: " << dtChangeTower << std::endl;

			//every 2 minutes you can have a new tower in the board
			currentTime = SDL_GetTicks();
			if (currentTime > addTowerTime + 120000) {
				//Tower * t = new Tower();
				level->addTowerToAvailableTowers(new Tower());
				//std::cout << "Size of vector is: "<<level->getAvailableTowers().size() << std::endl;
				//add_tower_timer_start = std::chrono::steady_clock::now();
				addTowerTime = currentTime;
			}

			//every 30 secs you can re-arrange a tower
			currentTime = SDL_GetTicks();
			if (currentTime > changeTowerTime + 30000) { //30000
				shouldChange = true;
				printf("You can change now\n");
				//change_tower_timer_start = std::chrono::steady_clock::now();
				changeTowerTime = currentTime;
			}

			//check timers for cannonball
			//auto add_cannonballs_timer_end = std::chrono::steady_clock::now();
			//float dtAddCannonballs = std::chrono::duration <float>(add_cannonballs_timer_end - add_cannonballs_timer_start).count(); // in seconds

			currentTime = SDL_GetTicks();
			if (currentTime > towersShootTime + 100) {
				level->towersShoot();
				towersShootTime = currentTime;
				//add_cannonballs_timer_start = std::chrono::steady_clock::now();
			}


			

			//check pirate timer
			//auto pirate_timer_end = std::chrono::steady_clock::now();
			//float dtPirate = std::chrono::duration <float>(pirate_timer_end - pirate_timer_start).count(); // in seconds


			/*currentTime = SDL_GetTicks();
			if (currentTime > pirateAddTime+3000) { // need change!!!!!!!!!!!!!!!!!!!!!!!!!!!
				Pirate* p = new Pirate();
				level->addPirate(p);
				pirateAddTime = currentTime;
			} */

			currentTime = SDL_GetTicks();
			if (currentTime > killPiratesTime + 2000) { // every 2 sec 
				//if (level->getReachedChest()) level->setReachedChest(false); // stop chest particles!!
				//level->checkChestCollision();
				level->killPiratesForGood();
				killPiratesTime = currentTime;
			}

			currentTime = SDL_GetTicks();
			if (currentTime > deletePiratesTime + 1000)
			{
				//if (level->getReachedChest()) level->setReachedChest(false); // stop chest particles!!
				level->deleteDeadPirates();
				deletePiratesTime = currentTime;
			}

			currentTime = SDL_GetTicks();
			if (currentTime > goldParticlesTime + 500)
			{
				if (level->getReachedChest()) level->setReachedChest(false); // stop chest particles!!
				level->checkChestCollision(); //check for chest collision
				goldParticlesTime = currentTime;
			}
			/*currentTime = SDL_GetTicks();
			if (currentTime > resetHitTime + 1500)
			{
				level->resetHitPirates();
				resetHitTime = currentTime;
			} */


			// Update
			renderer->Update(dt);

			level->setWasPaused(false);
		}
		else
		{
			// pause
			//renderer->Update(0.0f);
			if (!level->getWasPaused()) {
				timePaused = SDL_GetTicks();
				level->setWasPaused(true);
			}
		}


		

		// Draw
		renderer->Render();

		SDL_Color color = { 170,40,20 };
		renderer->RenderText("Score: " + std::to_string(level->getScore()), color, 10, 10, 72);
		renderer->RenderText("Gold: " + std::to_string(level->getGold()), color, 10, 50, 72);
		renderer->RenderText("Wave: " + std::to_string(level->getWave()-1), color, 10, 110, 72);
		renderer->setFontSize(72);

		if (level->getGO()) {
			renderer->RenderText("GAME OVER", color, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 3, 240);
			renderer->setFontSize(240);
		}

		//Update screen (swap buffer for double buffering)
		SDL_GL_SwapWindow(window);
	}

	//Clean up
	clean_up();

	return 0;
}