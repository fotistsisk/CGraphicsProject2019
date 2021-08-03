#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "GLEW\glew.h"
#include "glm\glm.hpp"
#include <vector>
#include "ShaderProgram.h"

class ParticleSwirl
{
protected:
	std::vector<glm::vec3>							m_particles_position;
	std::vector<float>								m_particles_life;

	GLuint											m_vbo;
	GLuint											m_vao;

	float m_continous_time;

	bool treasureChest;
	glm::vec3										center;

public:
	ParticleSwirl();
	~ParticleSwirl();
	bool										Init();
	void										Update(float dt);
	void										Render();

	bool										isTreasureChest();
	void										setTreasureChest(bool flag);
	glm::vec3									getCenter();
	void										setCenter(glm::vec3 center);

};

class ParticleEmitter
{
protected:
	glm::vec3										center;
	std::vector<glm::vec3>							m_particles_position;
	std::vector<glm::vec3>							m_particles_velocity;
	std::vector<float>								m_particles_life;

	

	// Store Particle Data on the GPU
	GLuint											m_vbo;
	GLuint											m_vao;

	float m_continous_time;

	bool treasureChest;
	bool energyBall;
	bool explosion;


public:
	ParticleEmitter();
	ParticleEmitter(glm::vec3); // for explosion
	~ParticleEmitter();
	bool										Init();
	void										Update(float dt);
	void										Render();
	
	glm::vec3									getCenter();
	void										setCenter(glm::vec3 center);

	bool										isTreasureChest();
	void										setTreasureChest(bool flag);

	bool										isEnergyBall();
	void										setEnergyBall(bool flag);

	float										getContTime();

};

#endif
