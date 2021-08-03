#include "Renderer.h"
#include "GeometryNode.h"
#include "Tools.h"
#include <algorithm>
#include "ShaderProgram.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "OBJLoader.h"
#include "Tower.h"
#include <iostream>




Renderer::Renderer()
{
	//initialize objects
	this->m_vbo_fbo_vertices = 0;
	this->m_vao_fbo = 0;

	this->m_fbo = 0;
	this->m_fbo_texture = 0;

	this->m_rendering_mode = RENDERING_MODE::TRIANGLES;
	this->m_continous_time = 0.0;
	//this->rythm = 0.0;

	this->loseCoinsDuration = 15; //15 frames.
	this->loseCoins = 0;
	this->loseCoinsCurrentFrame = 0;

	// Terrain
	this->terrain = nullptr;

	//Road
	for (int i = 0; i < NUMBER_OF_ROAD_TILES; i++) {
		this->roadTiles[i] = nullptr;
	}

	//Chest
	this->renderChest = nullptr;

	//cubes
	for (int i = 0; i < NUMBER_OF_ROAD_TILES; i++)
	{
		this->cube[i] = nullptr;
	}

	this->explosionsParticles = std::vector<ParticleEmitter>();

	this->fontSize = 72;
	//font = TTF_OpenFont("../Data/Fonts/OpenSans-Regular.ttf", font_size);
	this->font = TTF_OpenFont("../Data/Fonts/xbones.ttf", this->fontSize);

	

	// initialize the camera parameters
	this->m_camera_position = glm::vec3(0.720552, 18.1377, -11.3135);
	this->m_camera_target_position = glm::vec3(4.005, 12.634, -5.66336);
	this->m_camera_up_vector = glm::vec3(0, 1, 0);
}

Renderer::~Renderer()
{
	// delete g_buffer
	glDeleteTextures(1, &m_fbo_texture);
	glDeleteFramebuffers(1, &m_fbo);

	// delete common data
	glDeleteVertexArrays(1, &m_vao_fbo);
	glDeleteBuffers(1, &m_vbo_fbo_vertices);

	delete this->terrain;
	for (int i = 0; i < NUMBER_OF_ROAD_TILES; i++) {
		delete this->roadTiles[i];
	}
	delete this->roadTiles;
	
	delete this->renderChest;

	TTF_CloseFont(this->font);
}


bool Renderer::Init(int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
	this->m_screen_width = SCREEN_WIDTH;
	this->m_screen_height = SCREEN_HEIGHT;

	// Initialize OpenGL functions

	//Set clear color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//This enables depth and stencil testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);
	// glClearDepth sets the value the depth buffer is set at, when we clear it

	// Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// open the viewport
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); //we set up our viewport

	bool techniques_initialization = InitRenderingTechniques();
	bool buffers_initialization = InitIntermediateShaderBuffers();
	bool items_initialization = InitCommonItems();
	bool lights_sources_initialization = InitLightSources();
	bool meshes_initialization = InitGeometricMeshes();

	//If there was any errors
	if (Tools::CheckGLError() != GL_NO_ERROR)
	{
		printf("Exiting with error at Renderer::Init\n");
		return false;
	}

	//If everything initialized
	return techniques_initialization && items_initialization && buffers_initialization && meshes_initialization && lights_sources_initialization;
}


void Renderer::Update(float dt)
{
	float movement_speed = 10.0f;
	int render_speed = 1;
	if (this->level->getWasPaused()) render_speed = 0;
	// compute the direction of the camera
	glm::vec3 direction = glm::normalize(this->m_camera_target_position - this->m_camera_position);

	// move the camera towards the direction of the camera
	this->m_camera_position += this->m_camera_movement.x *  movement_speed * direction * dt;
	this->m_camera_target_position += this->m_camera_movement.x * movement_speed * direction * dt;

	// move the camera sideways
	glm::vec3 right = glm::normalize(glm::cross(direction, this->m_camera_up_vector));
	this->m_camera_position += this->m_camera_movement.y *  movement_speed * right * dt;
	this->m_camera_target_position += this->m_camera_movement.y * movement_speed * right * dt;

	glm::mat4 rotation = glm::mat4(1.0f);
	float angular_speed = glm::pi<float>() * 0.0025f;

	// compute the rotation of the camera based on the mouse movement
	rotation *= glm::rotate(glm::mat4(1.0), this->m_camera_look_angle_destination.y * angular_speed, right);
	rotation *= glm::rotate(glm::mat4(1.0), -this->m_camera_look_angle_destination.x  * angular_speed, this->m_camera_up_vector);
	this->m_camera_look_angle_destination = glm::vec2(0);

	// rotate the camera direction
	direction = rotation * glm::vec4(direction, 0);
	float dist = glm::distance(this->m_camera_position, this->m_camera_target_position);
	this->m_camera_target_position = this->m_camera_position + direction * dist;

	// compute the view matrix
	this->m_view_matrix = glm::lookAt(this->m_camera_position, this->m_camera_target_position, this->m_camera_up_vector);

	//compute the projection matrix 

	this->m_projection_matrix = glm::perspective(glm::radians(60.f), this->m_screen_width / (float)this->m_screen_height, 0.1f, 1500.0f);
	
	

	this->m_continous_time += dt;
	//std::cout << m_continous_time << std::endl;

	//create waves.
	this->currentTime = SDL_GetTicks();
	if (this->level->getWave() == 1) //first wave wait 5 secs
	{
		if (this->currentTime > this->waveTimer + 5000)
		{
			printf("THE WAVE IS: %d\n", this->level->getWave());
			this->wave.numOfPirates = 3;
			this->wave.types.clear();
			for (int i = 0; i < this->wave.numOfPirates; i++)
			{
				this->wave.types.push_back(0);
			}

			this->waveTimer = this->currentTime;
			this->level->setWave(this->level->getWave() + 1);
		}
	}
	else if (this->level->getWave() % 3 == 0) //every 3 waves get boss.
	{
		if (this->currentTime > this->waveTimer + 20000) //every 20 seconds new wave
		{
			printf("THE WAVE IS: %d\n", this->level->getWave());
			this->wave.numOfPirates = 1; //only boss
			this->wave.types.clear();
			this->level->setBoss(true);
			this->wave.types.push_back(1);

			this->waveTimer = this->currentTime;
			this->level->setWave(this->level->getWave() + 1);
			this->level->setStopWaves(true);
		}
	}
	else
	{
		if (this->currentTime > this->waveTimer + 20000 && !this->level->getStopWaves())
		{
			printf("THE WAVE IS: %d\n", this->level->getWave());
			this->wave.numOfPirates = (this->level->getWave() * 2 > 10) ? 10 : (this->level->getWave() * 2);
			this->wave.types.clear();
			for (int i = 0; i < this->wave.numOfPirates; i++)
			{
				this->wave.types.push_back(0);

			}

			this->waveTimer = this->currentTime;
			this->level->setWave(this->level->getWave() + 1);

		}
	}

	//create pirates in waves.
	this->currentTime = SDL_GetTicks();
	if (this->currentTime > this->piratesInWaveTimer + this->level->getPirateRate())
	{
		if (this->wave.numOfPirates > 0)
		{
			this->wave.numOfPirates = this->wave.numOfPirates - 1;
			if (this->wave.numOfPirates <= this->wave.types.size()) {
				int type = this->wave.types[this->wave.numOfPirates];
				this->level->addPirate(this->m_continous_time, type);
				// Change rate that pirates are placed according to their type
				// in order to avoid collisions
				switch (type) {
				case 0:
					this->level->setPirateRate(800); //every 2 secs new pirate in wave.
					break;
				case 1:
					this->level->setPirateRate(800);
					break;
				}
			}
			this->piratesInWaveTimer = currentTime;

		}
	}



	for (Pirate* p : this->level->getPirates())
	{
		float tmp = p->getRythm();
		tmp = tmp + dt * p->getSpeed(); // THIS IS ONLY FOR GLM::MIX...
		p->setRythm(tmp);

		float tmp2 = p->getReachChangeTileTime();
		tmp2 = tmp2 + dt;
		p->setReachChangeTileTime(tmp2);
	} 



	//terrain
	this->terrain_transformation_matrix =
		glm::translate(glm::mat4(1.0), glm::vec3(18, 0, 18)) * // good for the camera
		glm::scale(glm::mat4(1.0), glm::vec3(20.0f)); // scaling to [-20,20]
	this->terrain_transformation_normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(this->terrain_transformation_matrix))));

	//road

	for (int i = 0; i < NUMBER_OF_ROAD_TILES; i++) {

		float x = (float)GetLevel()->getRoadTileX(i);
		float z = (float)GetLevel()->getRoadTileZ(i);

		
		
		this->roadTiles_transformation_matrix[i] =
			glm::translate(glm::mat4(1.0), glm::vec3(x * 4, 1.4f, z * 4))* // translate the coordinates x4 because we have a 40x40 board and the coordinates are based on a 10x10 board 
			glm::scale(glm::mat4(1.0), glm::vec3(2.0f)); // scaling to [-2,2] in order for each tile to be 4x4
		this->roadTiles_transformation_normal_matrix[i] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(this->roadTiles_transformation_matrix[i]))));
	} 

	//chest
	float chestX = this->level->getChest()->getX();
	float chestZ = this->level->getChest()->getZ();
	//we don't need Y
	this->chest_transformation_matrix =
		glm::translate(glm::mat4(1.0), glm::vec3(chestX * 4.0f, 1.4f, chestZ * 6.5f)) * 
		glm::scale(glm::mat4(1.0), glm::vec3(0.09f)); // scaling 0.09
	this->chest_transformation_normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(this->chest_transformation_matrix))));
	

	//green & red Tiles
	float x = (float)this->level->getSelectionTileX();
	float z = (float)this->level->getSelectionTileZ();

	//maybe change this Y to Z because Y is not needed


	if (this->level->checkSelectionTileInRoad())
	{
		
		//if true then red Tile
		this->redTile_transformation_matrix =
			glm::translate(glm::mat4(1.0), glm::vec3(x * 4, 1.4f, z * 4)) * //use height because of the road --> need ground if not in road
			glm::scale(glm::mat4(1.0), glm::vec3(2.0f)); 
		this->redTile_transformation_normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(this->redTile_transformation_matrix))));
	}
	else
	{
		//if false then green Tile
		this->greenTile_transformation_matrix =
			glm::translate(glm::mat4(1.0), glm::vec3(x*4, 0.01f, z*4)) * 
			glm::scale(glm::mat4(1.0), glm::vec3(2.0f));
		this->greenTile_transformation_normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(this->greenTile_transformation_matrix))));
	}

	

	if (this->level->checkSelectionTileInTower())
	{

		//if true then red Tile
		this->redTile_transformation_matrix =
			glm::translate(glm::mat4(1.0), glm::vec3(x * 4, 0.01f, z * 4)) * //use height because of the road --> need ground if not in road
			glm::scale(glm::mat4(1.0), glm::vec3(2.0f));
		this->redTile_transformation_normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(this->redTile_transformation_matrix))));
	}
	else
	{
		//if false then green Tile
		this->greenTile_transformation_matrix =
			glm::translate(glm::mat4(1.0), glm::vec3(x * 4, 0.01f, z * 4)) *
			glm::scale(glm::mat4(1.0), glm::vec3(2.0f));
		this->greenTile_transformation_normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(this->greenTile_transformation_matrix))));
	}

	


	//towers
	
	for (Tower * t : this->level->getBuiltTowers())
	{
		//if (t->getToBeChanged()) continue;

		t->setTransformationMatrix(
			glm::translate(glm::mat4(1.0), glm::vec3(t->getX()*4, 0.01f,t->getZ()*4)) * 
			glm::scale(glm::mat4(1.0), glm::vec3(0.4f))); 
		t->setTransformationNormalMatrix(glm::mat4(glm::transpose(glm::inverse(glm::mat3(t->getTransformationMatrix())))));
	}

	for (int i = 0; i < this->level->getBuiltTowers().size(); i++)
	{
		if (this->level->getBuiltTowers()[i]->getToBeChanged())
		{
			//Tower* changed = this->level->getBuiltTowers()[i];
			//this->level->getAvailableTowers().insert(this->level->getAvailableTowers().begin(), this->level->getBuiltTowers()[i]);
			this->level->getBuiltTowers()[i]->goUnderGround(dt);
		}
	} 

	this->level->deleteUndergroundTowers();



	//cannonballs
	for (CannonBall * c : this->level->getCannonBalls())
	{
		float x = c->getX();
		float y = c->getY();
		float z = c->getZ();
		Pirate * target = c->getPirate();
		if (target->playingDead() || target->getDead())
		{
			this->level->deleteCannonball(c);
		}
		float pirate_x = 4*target->getContX();
		float pirate_y = 3.1f;
		//if (target->getType() == 1)
		//{
			//pirate_y = 5.f;
		//}
		float pirate_z = 4*target->getContZ();


		float dX = pirate_x - x;
		float dY = pirate_y - y;
		float dZ = pirate_z - z;

		auto distance = sqrt(pow(pirate_x - x, 2) + pow(pirate_y - y, 2) + pow(pirate_z - z, 2));

		float directionX = dX / distance;
		float directionY = dY / distance;
		float directionZ = dZ / distance;

		float next_x = x + directionX * c->getSpeed()/90*render_speed;
		float next_y = y + directionY * c->getSpeed()/90*render_speed;
		float next_z = z + directionZ * c->getSpeed()/90*render_speed;

		c->setX(next_x);
		c->setY(next_y);
		c->setZ(next_z);


		if (c->isEnergizedBall())
		{
			glm::vec3 center = glm::vec3(c->getX(), c->getY(), c->getZ());
			c->getParticles()->setCenter(center);
			c->getParticles()->Update(dt * 5);
			this->isEnergy = true;
		}
		else this->isEnergy = false;

		//intersection!!!!
		
		
		
		SphereIntersection * sphere = c->getSphere();
		sphere->setX(c->getX());
		sphere->setY(c->getY());
		sphere->setZ(c->getZ());
		c->setSphere(sphere);


		if(sphere->isIntersection(target->getSphere()) && !target->getDead() && !target->playingDead()){
			//c->setHit(true);
			
			std::cout<<"HIT!"<<std::endl;
			this->level->deleteCannonball(c);

			this->explosionPos = glm::vec3(target->getContX()*4,c->getY(), target->getContZ()*4);
			//this->explosionPos = glm::vec3(c->getX(),c->getY(), c->getZ());
			ParticleEmitter hitParticles = ParticleEmitter(this->explosionPos); //stack object
			this->explosionsParticles.emplace_back(hitParticles);
			this->explosionsParticles[this->explosionsParticles.size() - 1].Init();



			target->setHealth(target->getHealth() - c->getDamage());
			//std::cout << "Balls coords: " << c->getX() << " " << c->getZ() << std::endl;
			if (target->getHealth() <= 0) {
				if (target->getType() == 1)
				{
					this->level->setStopWaves(false);
					this->level->setGold(this->level->getGold() + 5 + this->level->getWave());
				}
				this->level->setScore(this->level->getScore()+1);
				c->getTower()->setKills(c->getTower()->getKills() + 1);
				//std::cout << "SCORE OF TOWER IS: " << c->getTower()->getKills() << std::endl;
				if (c->getTower()->getKills()==5)
				{
					//this->level->setUpgradeTower(true);
					c->getTower()->setToBeUpgraded(true); // upgrade tower.
				}
				//std::cout << "Target's coords: " << target->getX() << " " << target->getZ() << std::endl;
				target->playDead();
				//target->setDead(true);
				//this->level->deletePirate(target);

				// i want dummy for the particles or not removal !!
			}
			
			
		}


		c->setCannonBallTransformationMatrix(
			glm::translate(glm::mat4(1.0), glm::vec3(c->getX(), c->getY(), c->getZ())) *
			glm::scale(glm::mat4(1.0), glm::vec3(0.1f)));
		c->setCannonBallTransformationNormalMatrix(glm::mat4(glm::transpose(glm::inverse(glm::mat3(c->getCannonBallTransformationMatrix())))));

	}

	for (size_t i = 0; i < this->explosionsParticles.size(); i++)
	{
		this->explosionsParticles[i].Update(dt);
	}

	for (size_t i = 0; i <this->explosionsParticles.size(); i++)
	{
		if (this->explosionsParticles[i].getContTime() > 0.3) //delete particles after 0.3 seconds
		{
			this->explosionsParticles.erase(this->explosionsParticles.begin() + i);
		}
	}

	

	//CUBES!!!

	for (int i = 0; i < NUMBER_OF_ROAD_TILES; i++) {

		float x = (float)GetLevel()->getRoadTileX(i);
		float z = (float)GetLevel()->getRoadTileZ(i);


		

		this->cube_transformation_matrix[i] =
			glm::translate(glm::mat4(1.0), glm::vec3(x*4, 0.7f, z*4)) * 
			glm::scale(glm::mat4(1.0), glm::vec3(0.0265f,0.009f,0.0265f)); 
		this->cube_transformation_normal_matrix[i] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(this->cube_transformation_matrix[i]))));
	} 

	//pirates
	


	for (Pirate * p : this->level->getPirates())
	{	

		//particles when dead!
		if (p->getDead())
		{
			float x = p->getContX();
			float z = p->getContZ();

			glm::vec3 center = glm::vec3(x*4.f, 1.45f, z*4.f); // for particles!
			p->getParticles()->setCenter(center);
			p->getParticles()->Update(dt);
			
			continue;
		}

		if (p->playingDead()) continue;

		//p->setRythm(p->getRythm() + dt );
		

		float x = p->getX();
		float z = p->getZ();

		float cx = p->getContX();
		float cz = p->getContZ();

		// find the new location as the pirate follows the path
		float next_x = p->getNextX();
		float next_z = p->getNextZ();

		float deltaX = next_x - cx;
		float deltaZ = next_z - cz;

		auto displacement_x = (p->getSpeed() *(deltaX > 0)*dt - p->getSpeed() * (deltaX <= 0)*dt);
		auto displacement_z = (p->getSpeed() *(deltaZ > 0)*dt - p->getSpeed() * (deltaZ <= 0)*dt);

		float x_new;
		float z_new;

		//float x_new = x + ((abs(deltaX) < 0.15) ? 0 : (p->getSpeed() * 0.01*(deltaX > 0) - p->getSpeed() * 0.01*(deltaX <= 0)))*dt*render_speed;
		//float z_new = z + ((abs(deltaZ) < 0.15) ? 0 : (p->getSpeed() * 0.01*(deltaZ > 0) - p->getSpeed() * 0.01*(deltaZ <= 0)))*dt*render_speed;

		float param, fractpart, intpart;
		param = (p->getRythm() - p->getEnterTime());
		fractpart = modf(param, &intpart);
		//float a = ((m_continous_time - p->getEnterTime()) - floor(m_continous_time - p->getEnterTime()));
		//float a = fractpart;
		p->setInterpolant(fractpart); //a
		//abs(p->getContX() - p->getNextX()) < 0.001 && abs(p->getContZ() - p->getNextZ()) < 0.001

		//TWO WAYS OF MOVING... --> MIX GETS SOME GLITCHES ON THE RENDERING, PROBABALY BECAUSE OF dt

		//x_new = glm::mix(x, next_x, p->getInterpolant());
		//z_new = glm::mix(z, next_z, p->getInterpolant());

		x_new = cx + displacement_x *render_speed;
		z_new = cz + displacement_z *render_speed;

		//---------------------------------------------------------------------------------------------

		p->setContX(x_new);
		p->setContZ(z_new);
		
		if (p->getReachChangeTileTime() >= p->getChangeTileTime())
		{
			this->level->updateNextTileOfPirate(p);
			//std::cout << " time is: " << m_continous_time << " and changeTileTime is: "  << changeTileTime << " and pirate's change time is: "  << p->getChangeTileTime() << std::endl;
			p->setReachChangeTileTime(0.f);
			//std::cout << a << std::endl;
		}

		//printf(" time is: %f and pirate entered in: %f and diff is: %f and a is: %f \n", m_continous_time,p->getEnterTime(),param,p->getInterpolant()); 

		
		//std::cout << "z_new is: " << z_new << std::endl;
		

		/*if (abs(deltaX)<0.01) {
			x_new = x ;
		}
		else
		{
			x_new = x + displacement_x*dt*render_speed;

			//x_new = glm::mix(x, x+displacement_x, dt);
		}

		//float a = ( ( m_continous_time-p->getEnterTime() ) - floor( m_continous_time - p->getEnterTime()) ) * p->getSpeed() 

		
		if (abs(deltaZ) <0.01)
		{
			z_new = z ;
		}
		else
		{
			z_new = z + displacement_z*dt*render_speed;
			//z_new = glm::mix(z, z+displacement_z, dt);
		} */

		
		//std::cout<< "a: "<< ((m_continous_time - p->getEnterTime()) - floor(m_continous_time - p->getEnterTime())) << std::endl;

		auto directionX = next_x - x;
		auto directionZ = next_z - z;
		auto angles = atan2(directionZ, -directionX);

		

		

		glm::mat4 pirateRotation;

		/*if (p->getRoadID() == 1) pirateRotation = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0, 1, 0));
		else if (p->getRoadID() == 29) {
			pirateRotation = glm::rotate(glm::mat4(1.0f), 0.f, glm::vec3(0, 1, 0));
		}
		else {
			pirateRotation = glm::rotate(glm::mat4(1.0f), angles + glm::pi<float>() / 2, glm::vec3(0, 1, 0));
		} */

		
		pirateRotation = glm::rotate(glm::mat4(1.0f), angles + glm::pi<float>() / 2, glm::vec3(0, 1, 0)); // adding 90 degrees 
		


		
		//body movement
		p->setPirateBodyTransformationMatrix(
			glm::translate(glm::mat4(1.0), glm::vec3(p->getContX() * 4, 1.41f, p->getContZ() * 4)) *
			pirateRotation*
			glm::scale(glm::mat4(1.0), glm::vec3(p->getSize()*0.09f)));
			//glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0, 1, 0))); // SHOULD CHANGE LATERR!!!
		p->setPirateBodyTransformationNormalMatrix(glm::mat4(glm::transpose(glm::inverse(glm::mat3(p->getPirateBodyTransformationMatrix())))));

		glm::mat4 pivotArmRotation = glm::rotate(glm::mat4(1.0f), cos(p->getSpeed()* 5.f*m_continous_time) * (glm::pi<float>()/4), glm::vec3(1, 0, 0));
		
		//sword movement
		p->setPirateArmTransformationMatrix(
			glm::translate(glm::mat4(1.0), glm::vec3(p->getContX()*4, 1.45f,p->getContZ()*4)) *
			glm::scale(glm::mat4(1.0), glm::vec3(0.09f)) *
			//glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0, 1, 0))* // SHOULD CHANGE LATER!!!!
			pirateRotation*
			glm::translate(glm::mat4(1.0), glm::vec3(p->getSize() * 4.5f, p->getSize()*9.0f,0.0f))* // hand should be 3 units below shoulder which is in (4.5,12,0)
			pivotArmRotation*//rotate
			glm::translate(glm::mat4(1.0), glm::vec3(0.0f, p->getSize() * -3.0f,0.0f))); // translate hand in order to rotate not at (0,0,0) but 3 units down
		p->setPirateArmTransformationNormalMatrix(glm::mat4(glm::transpose(glm::inverse(glm::mat3(p->getPirateArmTransformationMatrix())))));

		//right foot movement
		//glm::mat4 rightFootMovement = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f, -sin(2.5f*m_continous_time * glm::pi<float>())));
		glm::mat4 rightFootMovement = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0,(-2 * 0.09f - sin(p->getSpeed() * 10 * m_continous_time) * 2)));

		p->setPirateRightFootTransformationMatrix(
			glm::translate(glm::mat4(1.0), glm::vec3(p->getContX() * 4, 1.44f, p->getContZ() * 4)) *
			glm::scale(glm::mat4(1.0), glm::vec3(p->getSize()*0.09f))*
			//glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0, 1, 0))*
			pirateRotation*
			rightFootMovement*
			glm::translate(glm::mat4(1.0), glm::vec3(4.0f, 0.0f, -2.0f))
			//rightFootMovement *
			); // SHOULD CHANGE LATEEER!!!
			
		p->setPirateRightFootTransformationNormalMatrix(glm::mat4(glm::transpose(glm::inverse(glm::mat3(p->getPirateRightFootTransformationMatrix())))));

		//left foot movement.
		//glm::mat4 leftFootMovement = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f, sin(2.5f*m_continous_time * glm::pi<float>())));
		glm::mat4 leftFootMovement = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, (2 * 0.09f + sin(p->getSpeed() * 10 * m_continous_time) * 2)));

		p->setPirateLeftFootTransformationMatrix(
			glm::translate(glm::mat4(1.0f), glm::vec3(p->getContX()*4, 1.44f, 4*p->getContZ())) *
			//pirateRotation * 
			glm::scale(glm::mat4(1.0), glm::vec3(p->getSize()*0.09f))*
			//glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0, 1, 0))*
			pirateRotation*
			leftFootMovement*
			glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 0, -2.0f)) 
			//leftFootMovement *
			);
		p->setPirateLeftFootTransformationNormalMatrix(glm::mat4(glm::transpose(glm::inverse(glm::mat3(p->getPirateLeftFootTransformationMatrix()))))); 

		//sphere!
		SphereIntersection * sphere = p->getSphere();
		sphere->setX(p->getContX() * 4);
		sphere->setY(1.44f);
		sphere->setZ(p->getContZ() * 4);
		p->setSphere(sphere);


		//this->level->checkChestCollision(p);
		
		
	
	}
	
	
	this->particleEmitter.Update(dt);

}




bool Renderer::InitCommonItems()
{
	glGenVertexArrays(1, &m_vao_fbo);
	glBindVertexArray(m_vao_fbo);

	GLfloat fbo_vertices[] = {
		-1, -1,
		1, -1,
		-1, 1,
		1, 1,
	};

	glGenBuffers(1, &m_vbo_fbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_fbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	return true;
}



bool Renderer::InitRenderingTechniques()
{
	bool initialized = true;

	// Geometry Rendering Program
	std::string vertex_shader_path = "../Data/Shaders/basic_rendering.vert";
	std::string fragment_shader_path = "../Data/Shaders/basic_rendering.frag";
	m_geometry_rendering_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_geometry_rendering_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	initialized = m_geometry_rendering_program.CreateProgram();
	m_geometry_rendering_program.LoadUniform("uniform_projection_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_view_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_model_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_normal_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_diffuse");
	m_geometry_rendering_program.LoadUniform("uniform_specular");
	m_geometry_rendering_program.LoadUniform("uniform_shininess");
	m_geometry_rendering_program.LoadUniform("uniform_has_texture");
	m_geometry_rendering_program.LoadUniform("diffuse_texture");
	m_geometry_rendering_program.LoadUniform("uniform_camera_position");
	// Light Source Uniforms
	m_geometry_rendering_program.LoadUniform("uniform_light_projection_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_light_view_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_light_position");
	m_geometry_rendering_program.LoadUniform("uniform_light_direction");
	m_geometry_rendering_program.LoadUniform("uniform_light_color");
	m_geometry_rendering_program.LoadUniform("uniform_light_umbra");
	m_geometry_rendering_program.LoadUniform("uniform_light_penumbra");
	m_geometry_rendering_program.LoadUniform("uniform_cast_shadows");
	m_geometry_rendering_program.LoadUniform("shadowmap_texture");

	
	// Post Processing Program
	vertex_shader_path = "../Data/Shaders/postproc.vert";
	fragment_shader_path = "../Data/Shaders/postproc.frag";
	m_postprocess_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_postprocess_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	initialized = initialized && m_postprocess_program.CreateProgram();
	m_postprocess_program.LoadUniform("uniform_texture");
	m_postprocess_program.LoadUniform("uniform_time");
	m_postprocess_program.LoadUniform("uniform_depth");
	m_postprocess_program.LoadUniform("uniform_projection_inverse_matrix"); 

	//our effects...
	m_postprocess_program.LoadUniform("GAME_OVER");
	m_postprocess_program.LoadUniform("LOSE_COINS");
	m_postprocess_program.LoadUniform("LOSE_COINS_FRAME");
	m_postprocess_program.LoadUniform("LOSE_COINS_DURATION");


	auto vertex_path = "../Data/Shaders/textshader.vert";
	auto fragment_path = "../Data/Shaders/textshader.frag";
	this->m_text_program.LoadVertexShaderFromFile(vertex_path);
	this->m_text_program.LoadFragmentShaderFromFile(fragment_path);
	initialized = initialized && this->m_text_program.CreateProgram();
	this->m_text_program.LoadUniform("uniform_texture");

	// Shadow mapping Program
	vertex_shader_path = "../Data/Shaders/shadow_map_rendering.vert";
	fragment_shader_path = "../Data/Shaders/shadow_map_rendering.frag";
	m_spot_light_shadow_map_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_spot_light_shadow_map_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	initialized = initialized && m_spot_light_shadow_map_program.CreateProgram();
	m_spot_light_shadow_map_program.LoadUniform("uniform_projection_matrix");
	m_spot_light_shadow_map_program.LoadUniform("uniform_view_matrix");
	m_spot_light_shadow_map_program.LoadUniform("uniform_model_matrix");


	//particles program
	vertex_shader_path = "../Data/Shaders/particles_rendering.vert";
	fragment_shader_path = "../Data/Shaders/particles_rendering.frag";
	m_particles_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_particles_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	initialized = initialized && m_particles_program.CreateProgram();
	m_particles_program.LoadUniform("uniform_view_matrix");
	m_particles_program.LoadUniform("uniform_projection_matrix");
	m_particles_program.LoadUniform("uniform_color");
	m_particles_program.LoadUniform("particles_type");


	


	return initialized;
}

bool Renderer::ReloadShaders()
{
	bool reloaded = true;

	reloaded = reloaded && m_geometry_rendering_program.ReloadProgram();
	reloaded = reloaded && m_postprocess_program.ReloadProgram();
	reloaded = reloaded && m_spot_light_shadow_map_program.ReloadProgram();
	
	

	return reloaded;
}


bool Renderer::InitIntermediateShaderBuffers()
{
	// generate texture handles 
	glGenTextures(1, &m_fbo_texture);
	glGenTextures(1, &m_fbo_depth_texture);

	// framebuffer to link everything together
	glGenFramebuffers(1, &m_fbo);

	return ResizeBuffers(m_screen_width, m_screen_height);
}


bool Renderer::ResizeBuffers(int width, int height)
{
	m_screen_width = width;
	m_screen_height = height;

	// texture
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_screen_width, m_screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// depth texture
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_screen_width, m_screen_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// framebuffer to link to everything together
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_fbo_depth_texture, 0);

	GLenum status = Tools::CheckFramebufferStatus(m_fbo);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//CHECK LATER

	/*
	m_projection_matrix = glm::perspective(glm::radians(60.f), width / (float)height, 0.1f, 1500.0f);
	m_view_matrix = glm::lookAt(m_camera_position, m_camera_target_position, m_camera_up_vector);
	*/
	

	return true;
}


bool Renderer::InitLightSources()
{
	// Initialize the spot light
	m_spotlight_node.SetPosition(glm::vec3(16, 30, 16));
	m_spotlight_node.SetTarget(glm::vec3(16.4, 0.16, 16));
	m_spotlight_node.SetColor(glm::vec3(140, 140, 140));
	m_spotlight_node.SetConeSize(73, 80);
	m_spotlight_node.CastShadow(true);

	return true;
}

bool Renderer::InitGeometricMeshes()
{
	bool initialized = true;
	OBJLoader loader;

	// load terrain
	auto mesh = loader.load("../Data/Terrain/terrain.obj");
	if (mesh != nullptr)
	{
		this->terrain = new GeometryNode();
		this->terrain->Init(mesh);
	}
	else
		initialized = false;
	delete mesh;

	//load road tiles
	mesh = loader.load("../Data/Terrain/road.obj");
	if (mesh != nullptr)
	{
		for (int i = 0; i < NUMBER_OF_ROAD_TILES; i++)
		{
			this->roadTiles[i] = new GeometryNode();
			this->roadTiles[i]->Init(mesh);
		}
	}
	else initialized = false;
	delete mesh;

	//loat chest
	mesh = loader.load("../Data/Treasure/NewChest/treasure.obj");
	if (mesh != nullptr)
	{
		this->renderChest = new GeometryNode();
		this->renderChest->Init(mesh);
	}
	else initialized = false;
	delete mesh;

	//load greenTile
	mesh = loader.load("../Data/Various/plane_green.obj");
	if (mesh != nullptr)
	{
		this->greenTile = new GeometryNode();
		this->greenTile->Init(mesh);
	}
	else initialized = false;
	delete mesh;

	
	//loat redTile
	mesh = loader.load("../Data/Various/plane_red.obj");
	if (mesh != nullptr)
	{
		this->redTile = new GeometryNode();
		this->redTile->Init(mesh);
	}
	else initialized = false;
	delete mesh;

	//load tower

	mesh = loader.load("../Data/MedievalTower/tower.obj");
	if (mesh != nullptr)
	{
		for (Tower * t : this->level->getAvailableTowers())
		{
			if (t->getLevel() == 2)
			{
				mesh = loader.load("../Data/MedievalTowerLevel2/tower.obj");
			}
			t->setRenderTower(new GeometryNode());
			t->getRenderTower()->Init(mesh);
		}
	}
	else initialized = false;
	delete mesh;

	//load pirate body

	/* = loader.load("../Data/Pirate/pirate_body.obj");
	if (mesh != nullptr)
	{
		for (Pirate * p : this->level->getPirates())
		{
			p->setRenderPirateBody(new GeometryNode());
			p->getRenderPirateBody()->Init(mesh);
			
		}
	}
	else initialized = false;
	delete mesh;

	//load pirate arm

	mesh = loader.load("../Data/Pirate/pirate_arm.obj");
	if (mesh != nullptr)
	{
		for (Pirate * p : this->level->getPirates())
		{
			p->setRenderPirateArm(new GeometryNode());
			p->getRenderPirateArm()->Init(mesh);
		}
	}
	else initialized = false;
	delete mesh;

	//load pirate left foot

	mesh = loader.load("../Data/Pirate/pirate_left_foot.obj");
	if (mesh != nullptr)
	{
		for (Pirate * p : this->level->getPirates())
		{
			p->setRenderPirateLeftFoot(new GeometryNode());
			p->getRenderPirateLeftFoot()->Init(mesh);
		}
	}
	else initialized = false;
	delete mesh;

	//load pirate right foot

	mesh = loader.load("../Data/Pirate/pirate_right_foot.obj");
	if (mesh != nullptr)
	{
		for (Pirate * p : this->level->getPirates())
		{
			p->setRenderPirateRightFoot(new GeometryNode());
			p->getRenderPirateRightFoot()->Init(mesh);


		}
	}
	else initialized = false;
	delete mesh; */


	//load road cube
	mesh = loader.load("../Data/Terrain/cube.obj");
	if (mesh != nullptr)
	{
		for (int i = 0; i < NUMBER_OF_ROAD_TILES; i++)
		{
			this->cube[i] = new GeometryNode();
			this->cube[i]->Init(mesh);
		}
	}
	else initialized = false;
	delete mesh;


	// load cannonball
	/*mesh = loader.load("../Data/Various/cannonball.obj");
	if (mesh != nullptr)
	{
		//TODO
	}
	else initialized = false;
	delete mesh; */

	//particles!
	this->particleEmitter.Init(); //chest


	return initialized;
}


void Renderer::SetRenderingMode(RENDERING_MODE mode)
{
	this->m_rendering_mode = mode;
}

void Renderer::Render()
{
	// Draw the geometry to the shadow maps
	RenderShadowMaps();

	// Draw the geometry
	RenderGeometry();

	

	// Render to screen
	RenderToOutFB(); 

	

	GLenum error = Tools::CheckGLError();
	if (error != GL_NO_ERROR)
	{
		printf("Renderer:Draw GL Error\n");
		system("pause");
	}
}

void Renderer::DrawGeometryNode(GeometryNode* node, glm::mat4 model_matrix, glm::mat4 normal_matrix)
{
	glBindVertexArray(node->m_vao);
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(model_matrix));
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_normal_matrix"], 1, GL_FALSE, glm::value_ptr(normal_matrix));
	for (int j = 0; j < node->parts.size(); j++)
	{
		glm::vec3 diffuseColor = node->parts[j].diffuseColor;
		glm::vec3 specularColor = node->parts[j].specularColor;
		float shininess = node->parts[j].shininess;
		glUniform3f(m_geometry_rendering_program["uniform_diffuse"], diffuseColor.r, diffuseColor.g, diffuseColor.b);
		glUniform3f(m_geometry_rendering_program["uniform_specular"], specularColor.r, specularColor.g, specularColor.b);
		glUniform1f(m_geometry_rendering_program["uniform_shininess"], shininess);
		glUniform1f(m_geometry_rendering_program["uniform_has_texture"], (node->parts[j].textureID > 0) ? 1.0f : 0.0f);
		glBindTexture(GL_TEXTURE_2D, node->parts[j].textureID);

		glDrawArrays(GL_TRIANGLES, node->parts[j].start_offset, node->parts[j].count);
	}
}

void Renderer::DrawGeometryNodeToShadowMap(GeometryNode* node, glm::mat4 model_matrix, glm::mat4 normal_matrix)
{
	glBindVertexArray(node->m_vao);
	glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(model_matrix));
	for (int j = 0; j < node->parts.size(); j++)
	{
		glDrawArrays(GL_TRIANGLES, node->parts[j].start_offset, node->parts[j].count);
	}
}


void Renderer::RenderShadowMaps()
{
	// if the light source casts shadows
	if (m_spotlight_node.GetCastShadowsStatus())
	{
		int m_depth_texture_resolution = m_spotlight_node.GetShadowMapResolution();

		// bind the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, m_spotlight_node.GetShadowMapFBO());
		glViewport(0, 0, m_depth_texture_resolution, m_depth_texture_resolution);
		GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawbuffers);

		// clear depth buffer
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// Bind the shadow mapping program
		m_spot_light_shadow_map_program.Bind();

		// pass the projection and view matrix to the uniforms
		glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node.GetProjectionMatrix()));
		glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node.GetViewMatrix()));

		// draw terrain
		DrawGeometryNodeToShadowMap(this->terrain, this->terrain_transformation_matrix, this->terrain_transformation_normal_matrix);

		//draw road
		for (int i = 0; i < NUMBER_OF_ROAD_TILES; i++) {
			DrawGeometryNodeToShadowMap(this->roadTiles[i], this->roadTiles_transformation_matrix[i], this->roadTiles_transformation_normal_matrix[i]);
		} 

		//draw chest
		DrawGeometryNodeToShadowMap(this->renderChest, this->chest_transformation_matrix, this->chest_transformation_normal_matrix);


		//draw towers
		for (Tower * t : this->level->getBuiltTowers())
		{
			DrawGeometryNodeToShadowMap(t->getRenderTower(),t->getTransformationMatrix() , t->getTransformationNormalMatrix());
		}

		//draw cube 
		for (int i = 0; i < NUMBER_OF_ROAD_TILES; i++)
		{
			DrawGeometryNodeToShadowMap(this->cube[i], this->cube_transformation_matrix[i], this->cube_transformation_normal_matrix[i]);
		}
		
		//draw pirate
		for (Pirate * p : this->level->getPirates())
		{
			if (p->getDead()) continue;
			DrawGeometryNodeToShadowMap(p->getRenderPirateBody(), p->getPirateBodyTransformationMatrix(), p->getPirateBodyTransformationNormalMatrix());
			DrawGeometryNodeToShadowMap(p->getRenderPirateArm(), p->getPirateArmTransformationMatrix(), p->getPirateArmTransformationNormalMatrix());
			DrawGeometryNodeToShadowMap(p->getRenderPirateLeftFoot(), p->getPirateLeftFootTransformationMatrix(), p->getPirateLeftFootTransformationNormalMatrix());
			DrawGeometryNodeToShadowMap(p->getRenderPirateRightFoot(), p->getPirateRightFootTransformationMatrix(), p->getPirateRightFootTransformationNormalMatrix());
		}

		//draw cannonballs
		for (CannonBall * c : this->level->getCannonBalls())
		{
			DrawGeometryNodeToShadowMap(c->getRenderBall(), c->getCannonBallTransformationMatrix(), c->getCannonBallTransformationNormalMatrix());
		}


		// draw the third object
		//DrawGeometryNodeToShadowMap(m_geometric_object3, m_geometric_object3_transformation_matrix, m_geometric_object3_transformation_normal_matrix);

		glBindVertexArray(0);

		// Unbind shadow mapping program
		m_spot_light_shadow_map_program.Unbind();

		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Renderer::RenderGeometry()
{
	// Bind the Intermediate framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, m_screen_width, m_screen_height);
	GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawbuffers);

	// clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	switch (m_rendering_mode)
	{
	case RENDERING_MODE::TRIANGLES:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case RENDERING_MODE::LINES:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case RENDERING_MODE::POINTS:
		glPointSize(2);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	};

	// Bind the geometry rendering program
	m_geometry_rendering_program.Bind();

	// pass camera parameters to uniforms
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(m_projection_matrix));
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(m_view_matrix));
	glUniform3f(m_geometry_rendering_program["uniform_camera_position"], m_camera_position.x, m_camera_position.y, m_camera_position.z);

	// pass the light source parameters to uniforms
	glm::vec3 light_position = m_spotlight_node.GetPosition();
	glm::vec3 light_direction = m_spotlight_node.GetDirection();
	glm::vec3 light_color = m_spotlight_node.GetColor();
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_light_projection_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node.GetProjectionMatrix()));
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_light_view_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node.GetViewMatrix()));
	glUniform3f(m_geometry_rendering_program["uniform_light_position"], light_position.x, light_position.y, light_position.z);
	glUniform3f(m_geometry_rendering_program["uniform_light_direction"], light_direction.x, light_direction.y, light_direction.z);
	glUniform3f(m_geometry_rendering_program["uniform_light_color"], light_color.x, light_color.y, light_color.z);
	glUniform1f(m_geometry_rendering_program["uniform_light_umbra"], m_spotlight_node.GetUmbra());
	glUniform1f(m_geometry_rendering_program["uniform_light_penumbra"], m_spotlight_node.GetPenumbra());
	glUniform1i(m_geometry_rendering_program["uniform_cast_shadows"], (m_spotlight_node.GetCastShadowsStatus()) ? 1 : 0);
	// Set the sampler2D uniform to use texture unit 1
	glUniform1i(m_geometry_rendering_program["shadowmap_texture"], 1);
	// Bind the shadow map texture to texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, (m_spotlight_node.GetCastShadowsStatus()) ? m_spotlight_node.GetShadowMapDepthTexture() : 0);

	// Enable Texture Unit 0
	glUniform1i(m_geometry_rendering_program["uniform_diffuse_texture"], 0);
	glActiveTexture(GL_TEXTURE0);

	// draw terrain
	DrawGeometryNode(this->terrain, this->terrain_transformation_matrix, this->terrain_transformation_normal_matrix);

	//draw road
	for (int i = 0; i < NUMBER_OF_ROAD_TILES; i++) {
		DrawGeometryNode(this->roadTiles[i], this->roadTiles_transformation_matrix[i], this->roadTiles_transformation_normal_matrix[i]);
	} 

	//draw chest 
	DrawGeometryNode(this->renderChest, this->chest_transformation_matrix, this->chest_transformation_normal_matrix);

	//draw towers
	for (Tower * t : this->level->getBuiltTowers())
	{
		DrawGeometryNode(t->getRenderTower(), t->getTransformationMatrix(), t->getTransformationNormalMatrix());
	}

	
	//draw pirate
	for (Pirate * p : this->level->getPirates())
	{
		if (p->getDead()) continue;
		DrawGeometryNode(p->getRenderPirateBody(), p->getPirateBodyTransformationMatrix(), p->getPirateBodyTransformationNormalMatrix());
		DrawGeometryNode(p->getRenderPirateArm(), p->getPirateArmTransformationMatrix(), p->getPirateArmTransformationNormalMatrix());
		DrawGeometryNode(p->getRenderPirateLeftFoot(), p->getPirateLeftFootTransformationMatrix(), p->getPirateLeftFootTransformationNormalMatrix());
		DrawGeometryNode(p->getRenderPirateRightFoot(), p->getPirateRightFootTransformationMatrix(), p->getPirateRightFootTransformationNormalMatrix());
	}
	


	//draw cube 
	for (int i = 0; i < NUMBER_OF_ROAD_TILES; i++)
	{
		DrawGeometryNode(this->cube[i], this->cube_transformation_matrix[i], this->cube_transformation_normal_matrix[i]);
	}

	//draw cannonballs
	for (CannonBall * c : this->level->getCannonBalls())
	{
		//if (c->getHit()) continue;
		DrawGeometryNode(c->getRenderBall(), c->getCannonBallTransformationMatrix(), c->getCannonBallTransformationNormalMatrix());
	}

	// draw the red and green tile with transparency

	glDepthMask(GL_FALSE);
	glEnable (GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (this->level->checkSelectionTileInRoad() || this->level->checkSelectionTileInTower())
	{
		//if true then red Tile
		
		//draw redTile

		DrawGeometryNode(this->redTile, this->redTile_transformation_matrix, this->redTile_transformation_normal_matrix);
	}
	else
	{
		//if false then green Tile

		//draw greenTile

		DrawGeometryNode(this->greenTile, this->greenTile_transformation_matrix, this->greenTile_transformation_normal_matrix);
	}

	

	glDepthMask(GL_TRUE);


	//PARTICLES!!! --> we have 4 types: 0 is chest particles, 1 is dead pirate, 2 is the hit of the ball, 3 is energy.

	//if a pirate has reached a chest, render gold particles.
	if (this->level->getReachedChest())
	{
		this->loseCoins = 1; // for post shader.
		//printf(" NOW RED SCREEN!! \n");
		//std::cout << "PARTICLES GOLD NOW! " << std::endl;
		this->m_particles_program.Bind();
		glUniformMatrix4fv(this->m_particles_program["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(this->m_projection_matrix));
		glUniformMatrix4fv(this->m_particles_program["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(this->m_view_matrix));

		//pass the gold color to the shader
		glm::vec3 color = glm::vec3(1.f, 0.8431372549f, 0.f); //gold -->	#FFD700  rgb(255,215,0)
		glUniform3f(this->m_particles_program["uniform_color"], color.r, color.g, color.b);
		glUniform1f(this->m_particles_program["particles_type"], 0); //type is 0 for gold chest particles
		this->particleEmitter.Render();

		this->m_particles_program.Unbind();

	}

	//when the pirate dies.
	for (Pirate* p : this->level->getPirates())
	{
		if (p->getDead()) 
		{  // if not dead for good then don't bother.

			this->m_particles_program.Bind();
			glUniformMatrix4fv(this->m_particles_program["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(this->m_projection_matrix));
			glUniformMatrix4fv(this->m_particles_program["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(this->m_view_matrix));

			//pass the color to the shader --> red!
			glm::vec3 color = glm::vec3(1.f, 0.2f, 0.2f); // red --> rgb(255,51,51)
			glUniform3f(this->m_particles_program["uniform_color"], color.r, color.g, color.b);
			glUniform1f(this->m_particles_program["particles_type"], 1);

			p->getParticles()->Render(); // for dead pirate.
			//this->particleEmitter.Render();
			this->m_particles_program.Unbind();
		}

	}

	//pirate hit
	this->m_particles_program.Bind();
	glUniformMatrix4fv(this->m_particles_program["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(this->m_projection_matrix));
	glUniformMatrix4fv(this->m_particles_program["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(this->m_view_matrix));
	glm::vec3 color = glm::vec3(0, 0, 0); // pass black color!
	if(this->isEnergy) color = glm::vec3(0, 0, 1); //blue
	glUniform3f(this->m_particles_program["uniform_color"], color.r, color.g, color.b);
	glUniform1f(this->m_particles_program["particles_type"], 2);

	for (size_t i = 0; i < this->explosionsParticles.size(); i++)
	{
		this->explosionsParticles[i].Render();
	}

	this->m_particles_program.Unbind();


	//energy balls
	for (CannonBall * c : this->level->getCannonBalls())
	{
		if (!c->isEnergizedBall()) continue;

		this->m_particles_program.Bind();
		glUniformMatrix4fv(this->m_particles_program["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(this->m_projection_matrix));
		glUniformMatrix4fv(this->m_particles_program["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(this->m_view_matrix));

		//pass color to the shader --> energy blue!
		glm::vec3 color = glm::vec3(0,0.50196078431f,1);
		glUniform3f(this->m_particles_program["uniform_color"], color.r, color.g, color.b);
		glUniform1f(this->m_particles_program["particles_type"], 3);

		c->getParticles()->Render();

		this->m_particles_program.Unbind();
	} 


	glBindVertexArray(0);
	m_geometry_rendering_program.Unbind();





	glDisable(GL_DEPTH_TEST);
	if (m_rendering_mode != RENDERING_MODE::TRIANGLES)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPointSize(1.0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void Renderer::RenderToOutFB()
{
	// Bind the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_screen_width, m_screen_height);

	// clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// disable depth testing and blending
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// bind the post processing program
	m_postprocess_program.Bind();

	glBindVertexArray(m_vao_fbo);
	// Bind the intermediate color image to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glUniform1i(m_postprocess_program["uniform_texture"], 0);
	// Bind the intermediate depth buffer to texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
	glUniform1i(m_postprocess_program["uniform_depth"], 1);

	//our effects...
	glUniform1i(m_postprocess_program["GAME_OVER"], this->level->getGO() ? 1 : 0);

	//next frame!
	if (this->loseCoins == 1)
	{
		if (this->loseCoinsCurrentFrame == this->loseCoinsDuration)
		{
			this->loseCoins = 0;
			//printf(" END OF RED FRAMES!!\n ");
			this->loseCoinsCurrentFrame = 0;
		}
		else
		{
			this->loseCoinsCurrentFrame++;
		}
	}

	glUniform1i(m_postprocess_program["LOSE_COINS"], this->loseCoins);
	glUniform1i(m_postprocess_program["LOSE_COINS_FRAME"], this->loseCoinsCurrentFrame); 
	glUniform1i(m_postprocess_program["LOSE_COINS_DURATION"], this->loseCoinsDuration);

	glUniform1f(m_postprocess_program["uniform_time"], m_continous_time);
	glm::mat4 projection_inverse_matrix = glm::inverse(m_projection_matrix);
	glUniformMatrix4fv(m_postprocess_program["uniform_projection_inverse_matrix"], 1, GL_FALSE, glm::value_ptr(projection_inverse_matrix));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);

	// Unbind the post processing program
	m_postprocess_program.Unbind();
}

void Renderer::RenderText(std::string message, SDL_Color color, int x, int y, int size)
{
	bool same_size = (size == this->fontSize);

	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!same_size) {
		// Close the previous font
		TTF_CloseFont(font);
		// Open with new size
		//font = TTF_OpenFont("../Data/Fonts/OpenSans-Regular.ttf", size);
		this->font = TTF_OpenFont("../Data/Fonts/xbones.ttf", size);

	}
	SDL_Surface * sFont = TTF_RenderText_Blended(font, message.c_str(), color);

	GLuint vao, vbo_vertices, vbo_texcoords;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float lenW = float(sFont->w) / m_screen_width;
	float lenH = float(sFont->h) / m_screen_height;

	float xPos = float(x) / m_screen_width;
	float yPos = float(y) / m_screen_height;

	xPos = 2 * xPos - 1;
	yPos = -2 * yPos + 1;

	GLfloat vertices[] = {
		xPos, yPos - lenH,
		xPos + lenW, yPos - lenH,
		xPos, yPos,
		xPos + lenW, yPos,
	};

	GLfloat texCoords[] = {
		0, 1,
		1, 1,
		0, 0,
		1, 0,
	};

	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sFont->w, sFont->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, sFont->pixels);

	glGenBuffers(1, &vbo_texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,				// attribute index
		2,              // number of elements per vertex, here (x,y)
		GL_FLOAT,		// the type of each element
		GL_FALSE,       // take our values as-is
		0,		         // no extra data between each position
		0				// pointer to the C array or an offset to our buffer
	);

	// bind the post processing program
	this->m_text_program.Bind();

	glBindVertexArray(vao);
	// Bind the intermediate color image to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(this->m_text_program["uniform_texture"], 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);

	// Unbind the post processing program
	this->m_text_program.Unbind();

	glDisable(GL_BLEND);
	//glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo_vertices);
	glDeleteBuffers(1, &vbo_texcoords);

	SDL_FreeSurface(sFont);

}


void Renderer::CameraMoveForward(bool enable)
{
	m_camera_movement.x = (enable) ? 1 : 0;
}
void Renderer::CameraMoveBackWard(bool enable)
{
	m_camera_movement.x = (enable) ? -1 : 0;
}

void Renderer::CameraMoveLeft(bool enable)
{
	m_camera_movement.y = (enable) ? -1 : 0;
}
void Renderer::CameraMoveRight(bool enable)
{
	m_camera_movement.y = (enable) ? 1 : 0;
}

void Renderer::CameraLook(glm::vec2 lookDir)
{
	m_camera_look_angle_destination = glm::vec2(1, -1) * lookDir;
}


void Renderer::SetLevel(Level* lvl) 
{
	this->level = lvl;
}

Level* Renderer::GetLevel() 
{ 
	return this->level; 
}

void Renderer::setFontSize(unsigned int size) { this->fontSize = size; }

