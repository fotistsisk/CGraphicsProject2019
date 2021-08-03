#include "Tower.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"

Tower::Tower()
{
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
	this->range = 2;
	this->damage = 10;
	this->level = 1;
	this->fireNow = 0;
	this->fireRate = 2000; //2 secs for level 1.
	this->kills = 0;
	this->renderTower = nullptr;
	this->toBeChanged = false;
	this->toBeUpgraded = false;
}

Tower::~Tower() {
	delete renderTower;
}

float Tower::getX(){return x;}
float Tower::getY(){return y;}
float Tower::getZ(){return z;}
int Tower::getRange(){return range;}
int Tower::getKills() { return this->kills; }
int Tower::getDamage(){return damage;}
int Tower::getLevel() { return this->level; }
int Tower::getFireNow() { return this->fireNow; }
void Tower::setX(float x){this->x=x;}
void Tower::setY(float y){this->y=y;}
void Tower::setZ(float z){this->z=z;}
void Tower::setRange(int r){this->range = r;}
void Tower::setDagame(int d) { this->damage = d; }
void Tower::setLevel(int l) { this->level = l; }
void Tower::setFireNow(int f) { this->fireNow = f; }
void Tower::setKills(int k) { this->kills = k; }
int Tower::getFireRate() { return this->fireRate; }
void Tower::setFireRate(int f) { this->fireRate = f; }
bool Tower::getToBeChanged() { return this->toBeChanged; }
void Tower::setToBeChanged(bool flag) { this->toBeChanged = flag; }
class GeometryNode* Tower::getRenderTower(){return renderTower;}
glm::mat4 Tower::getTransformationMatrix(){return tower_transformation_matrix;}
glm::mat4 Tower::getTransformationNormalMatrix(){return tower_transformation_normal_matrix;}


void Tower::setRenderTower(class GeometryNode* node)
{
	this->renderTower = node;
}

void Tower::setTransformationMatrix(glm::mat4 matrix)
{
	this->tower_transformation_matrix = matrix;
}

void Tower::setTransformationNormalMatrix(glm::mat4 matrix)
{
	this->tower_transformation_normal_matrix = matrix;
}

void Tower::goUnderGround(float dt)
{
	
	this->tower_transformation_matrix = glm::translate(glm::mat4(1.0), glm::vec3(this->x*4,this->y-=dt*4.f,this->z*4)) *
										glm::scale(glm::mat4(1.0), glm::vec3(0.4f));

	this->tower_transformation_normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(this->tower_transformation_matrix))));
}

bool Tower::getToBeUpgraded() { return this->toBeUpgraded; }

void Tower::setToBeUpgraded(bool flag) { this->toBeUpgraded = flag; }