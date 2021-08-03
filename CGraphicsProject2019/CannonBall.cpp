#include "CannonBall.h"

CannonBall::CannonBall()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->speed = 10.0f;
	this->hit = false;
	this->isEnergyBall = false;

	this->renderBall = nullptr;
	this->sphere = new SphereIntersection();
	sphere->setCenterX(0);
	sphere->setCenterY(0);
	sphere->setCenterZ(0);
	sphere->setRadius(1 * 0.1f);

	this->particles = new ParticleEmitter();
	this->particles->Init();
	this->particles->setTreasureChest(false);

}
CannonBall::~CannonBall()
{
	delete this->renderBall;
}

float CannonBall::getX(){return x;}
float CannonBall::getY(){return y;}
float CannonBall::getZ(){return z;}
void CannonBall::setX(float x){this->x = x;}
void CannonBall::setY(float y){this->y = y;}
void CannonBall::setZ(float z){this->z = z;}

float CannonBall::getSpeed(){return speed;}
void CannonBall::setSpeed(float speed){this->speed = speed;}

int CannonBall::getDamage(){return damage;}
void CannonBall::setDamage(int x) { this->damage = x; }


float CannonBall::getNextX(){return next_x;}
float CannonBall::getNextY(){return next_y;}
float CannonBall::getNextZ(){return next_z;}
void CannonBall::setNextX(float x) { this->next_x = x; }
void CannonBall::setNextZ(float z) { this->next_y = y; }
void CannonBall::setNextY(float y) { this->next_z = z; }

void CannonBall::setEnergyBall(bool flag)
{
	this->isEnergyBall = flag;
}

bool CannonBall::isEnergizedBall()
{
	return this->isEnergyBall;
}


Pirate * CannonBall::getPirate(){return target;}
void CannonBall::setPirate(Pirate* p){this->target = p;}

Tower* CannonBall::getTower() { return this->owner; }
void CannonBall::setTower(Tower* t) { this->owner = t; }

bool CannonBall::getHit(){return hit;}
void CannonBall::setHit(bool hit){this->hit = hit;}

GeometryNode* CannonBall::getRenderBall() { return this->renderBall; }
glm::mat4 CannonBall::getCannonBallTransformationMatrix() { return this->cannonball_transformation_matrix; }
glm::mat4 CannonBall::getCannonBallTransformationNormalMatrix() { return this->cannonball_transformation_normal_matrix; }

void CannonBall::setRenderBall(class GeometryNode * node){this->renderBall = node;}
void CannonBall::setCannonBallTransformationMatrix(glm::mat4 matrix) { this->cannonball_transformation_matrix = matrix; }
void CannonBall::setCannonBallTransformationNormalMatrix(glm::mat4 matrix) { this->cannonball_transformation_normal_matrix = matrix; }

//sphere and particles
SphereIntersection * CannonBall::getSphere() {return sphere;}
void CannonBall::setSphere(SphereIntersection * s){this->sphere = s;}


ParticleEmitter * CannonBall::getParticles() { return this->particles; }
void CannonBall::setParticles(ParticleEmitter* p) { this->particles = p; }