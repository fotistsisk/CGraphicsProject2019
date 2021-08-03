#pragma once

#include "GeometryNode.h"
#include "Pirate.h"
#include "Tower.h"


class CannonBall
{
private:
	float x;
	float y;
	float z;

	float next_x;
	float next_y;
	float next_z;
	float speed;
	int damage;

	Pirate * target;
	bool hit;
	Tower* owner;

	bool isEnergyBall;

	SphereIntersection * sphere;

	ParticleEmitter * particles;

	class GeometryNode* renderBall;
	glm::mat4 cannonball_transformation_matrix;
	glm::mat4 cannonball_transformation_normal_matrix;

public:
	CannonBall();
	~CannonBall();

	float getX();
	float getY();
	float getZ();
	void setX(float x);
	void setY(float y);
	void setZ(float z);

	float getSpeed();
	void setSpeed(float speed);
	int getDamage();
	void setDamage(int x);

	float getNextX();
	float getNextY();
	float getNextZ();
	void setNextX(float x);
	void setNextZ(float z);
	void setNextY(float y);

	bool isEnergizedBall();
	void setEnergyBall(bool flag);

	Pirate * getPirate();
	void setPirate(Pirate* p);

	Tower* getTower();
	void setTower(Tower*);

	bool getHit();
	void setHit(bool hit);

	GeometryNode* getRenderBall();
	glm::mat4 getCannonBallTransformationMatrix();
	glm::mat4 getCannonBallTransformationNormalMatrix();

	void setRenderBall(class GeometryNode * node);
	void setCannonBallTransformationMatrix(glm::mat4 matrix);
	void setCannonBallTransformationNormalMatrix(glm::mat4 matrix);

	// intersection, particles.
	SphereIntersection * getSphere();
	void setSphere(SphereIntersection * s);

	ParticleEmitter* getParticles();
	void setParticles(ParticleEmitter* p);

	ParticleSwirl* getParticlesS();
	void setParticlesS(ParticleSwirl* p);
	
};