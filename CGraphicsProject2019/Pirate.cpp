#include "Pirate.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include <iostream>
Pirate::Pirate() {
	this->x = 0;
	this->y = 0;
	this->z = -0.5;
	this->health = 40;
	this->speed = 1.f;
	this->size = 1.f;
	this->roadID = 0;
	this->next_x = 0;
	this->next_z = 0;
	this->continuous_x = 0;
	this->continuous_z = 0;
	this->renderPirateArm = nullptr;
	this->renderPirateBody = nullptr;
	this->renderPirateLeftFoot = nullptr;
	this->renderPirateRightFoot = nullptr;
	this->isDead = false;
	this->playsDead = false;
	this->reachChangeTileTime = 0.f; //important

	this->sphere = new SphereIntersection();
	this->sphere->setCenterX(-0.5957*0.09f);
	this->sphere->setCenterY(11.683*0.09f);
	this->sphere->setCenterZ(-4.274*0.09f);
	this->sphere->setRadius(12.87075f * 0.09f);

	this->particles = new ParticleEmitter();
	this->particles->Init();
	this->particles->setTreasureChest(false);

}

Pirate::~Pirate()
{
	delete this->renderPirateArm;
	delete this->renderPirateBody;
	delete this->renderPirateRightFoot;
	delete this->renderPirateLeftFoot;
	delete this->sphere;
	delete this->particles;
}

float Pirate::getX(){return x;}
float Pirate::getY(){return y;}
float Pirate::getZ(){return z;}

float Pirate::getRythm() { return this->rythm; }
float Pirate::getChangeTileTime() { return this->changeTileTime; }

float Pirate::getReachChangeTileTime() { return this->reachChangeTileTime; }
void Pirate::setReachChangeTileTime(float c) { this->reachChangeTileTime = c; }

int Pirate::getRoadID() { return roadID; }
int Pirate::getDir(){return direction;}
float Pirate::getSpeed(){return speed;}
bool Pirate::getDead(){return isDead;}
int Pirate::getHealth(){return health;}
float Pirate::getEnterTime() { return enter_time; }

SphereIntersection * Pirate::getSphere() {return sphere;}
void Pirate::setX(float x){this->x = x;}
void Pirate::setY(float y){this->y = y;}
void Pirate::setZ(float z){this->z = z;}
void Pirate::setHealth(int h){this->health = h;}
void Pirate::setRoadID(int id){this->roadID = id;}
void Pirate::setDead(bool isDead){this->isDead = isDead;}
void Pirate::setDir(int dir){this->direction = dir;}
void Pirate::setSpeed(float s){this->speed = s;}
void Pirate::setEnterTime(float e_t) { this->enter_time = e_t; }
void Pirate::setSphere(SphereIntersection * s){this->sphere = s;}
void Pirate::setRythm(float r) { this->rythm = r; }


ParticleEmitter * Pirate::getParticles() { return this->particles; }
void Pirate::setParticles(ParticleEmitter* p) { this->particles = p; }

float Pirate::getInterpolant() { return this->interpolant; }
void Pirate::setInterpolant(float i) { this->interpolant = i; }

float Pirate::getSize() { return this->size; }
void Pirate::setSize(float s) { this->size = s; }

int Pirate::getType() { return this->type; }
void Pirate::setType(int t) { this->type = t; }

void Pirate::setChangeTileTime(float t) { this->changeTileTime = t; }

float Pirate::getNextX()
{
	return this->next_x;
}

float Pirate::getNextZ()
{
	return this->next_z;
}

void Pirate::setNextX(float x)
{
	this->next_x = x;

}

void Pirate::setNextZ(float z)
{
	this->next_z = z;
}

bool Pirate::playingDead()
{
	return this->playsDead;
}

void Pirate::setPlayingDead(bool flag)
{
	this->playsDead = flag;
}

float Pirate::getContX() { return this->continuous_x; }
float Pirate::getContZ() { return this->continuous_z; }
void Pirate::setContX(float x) { this->continuous_x = x; }
void Pirate::setContZ(float z) { this->continuous_z = z; }



class GeometryNode* Pirate::getRenderPirateBody()
{
	return this->renderPirateBody;
}
glm::mat4 Pirate::getPirateBodyTransformationMatrix(){
	return this->pirateBody_transformation_matrix;
}
glm::mat4 Pirate::getPirateBodyTransformationNormalMatrix(){
	return this->pirateBody_transformation_normal_matrix;
}

class GeometryNode* Pirate::getRenderPirateArm()
{
	return this->renderPirateArm;
}
glm::mat4 Pirate::getPirateArmTransformationMatrix()
{
	return this->pirateArm_transformation_matrix;
}
glm::mat4 Pirate::getPirateArmTransformationNormalMatrix() {
	return this->pirateArm_transformation_normal_matrix;
}

class GeometryNode* Pirate::getRenderPirateRightFoot()
{
	return this->renderPirateRightFoot;
}
glm::mat4 Pirate::getPirateRightFootTransformationMatrix() {
	return this->pirateRightFoot_transformation_matrix;
}
glm::mat4 Pirate::getPirateRightFootTransformationNormalMatrix() {
	return this->pirateRightFoot_transformation_normal_matrix;
}

class GeometryNode* Pirate::getRenderPirateLeftFoot()
{
	return this->renderPirateLeftFoot;
}
glm::mat4 Pirate::getPirateLeftFootTransformationMatrix()
{
	return this->pirateLeftFoot_transformation_matrix;
}
glm::mat4 Pirate::getPirateLeftFootTransformationNormalMatrix() {
	return this->pirateLeftFoot_transformation_normal_matrix;
}

void Pirate::setRenderPirateBody(class GeometryNode * node)
{
	this->renderPirateBody = node;
}
void Pirate::setPirateBodyTransformationMatrix(glm::mat4 matrix) {
	this->pirateBody_transformation_matrix = matrix;
}
void Pirate::setPirateBodyTransformationNormalMatrix(glm::mat4 matrix) {
	this->pirateBody_transformation_normal_matrix = matrix;
}

void Pirate::setRenderPirateArm(class GeometryNode * node)
{
	this->renderPirateArm= node;
}
void Pirate::setPirateArmTransformationMatrix(glm::mat4 matrix) {
	this->pirateArm_transformation_matrix = matrix;
}
void Pirate::setPirateArmTransformationNormalMatrix(glm::mat4 matrix){
	this->pirateArm_transformation_normal_matrix = matrix;
}

void Pirate::setRenderPirateLeftFoot(class GeometryNode * node)
{
	this->renderPirateLeftFoot = node;
}
void Pirate::setPirateLeftFootTransformationMatrix(glm::mat4 matrix){
	this->pirateLeftFoot_transformation_matrix = matrix;
}
void Pirate::setPirateLeftFootTransformationNormalMatrix(glm::mat4 matrix){
	this->pirateLeftFoot_transformation_normal_matrix = matrix;
}

void Pirate::setRenderPirateRightFoot(class GeometryNode * node)
{
	this->renderPirateRightFoot = node;
}
void Pirate::setPirateRightFootTransformationMatrix(glm::mat4 matrix){
	this->pirateRightFoot_transformation_matrix = matrix;
}
void Pirate::setPirateRightFootTransformationNormalMatrix(glm::mat4 matrix){
	this->pirateRightFoot_transformation_normal_matrix = matrix;
}


void Pirate::playDead()
{
	this->playsDead = true;
	glm::vec3 piratePos = glm::vec3(this->continuous_x * 4, 0, this->continuous_z * 4);
	//std::cout << this->x << " " << this->z << std::endl;
	this->pirateBody_transformation_matrix = glm::translate(glm::mat4(1.0), glm::vec3(0, 1.45f, 0) + piratePos)*
											 glm::rotate(glm::mat4(1.0f), 1.57f, glm::vec3(1, 0, 0)) *
											 glm::scale(glm::mat4(1.0), glm::vec3(this->size*0.09f));

	this->pirateBody_transformation_normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(this->pirateBody_transformation_matrix))));


	this->pirateArm_transformation_matrix = glm::translate(glm::mat4(1.0), glm::vec3(1.0f, 1.45f, 0) + piratePos)*
											glm::rotate(glm::mat4(1.0f), 1.57f, glm::vec3(0, 0, 1)) *
											glm::scale(glm::mat4(1.0), glm::vec3(this->size*0.09f));

	this->pirateArm_transformation_normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(this->pirateArm_transformation_matrix))));

	this->pirateRightFoot_transformation_matrix = glm::translate(glm::mat4(1.0), glm::vec3(1.0f, 1.45f, 0) + piratePos)*
												  glm::scale(glm::mat4(1.0), glm::vec3(this->size*0.09f));

	this->pirateRightFoot_transformation_normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(this->pirateRightFoot_transformation_matrix))));

	this->pirateLeftFoot_transformation_matrix = glm::translate(glm::mat4(1.0), glm::vec3(1.0f, 1.45f, 0) + piratePos)*
												 glm::scale(glm::mat4(1.0), glm::vec3(this->size*0.09f));

	this->pirateLeftFoot_transformation_normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(this->pirateRightFoot_transformation_matrix))));

}