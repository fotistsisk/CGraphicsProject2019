#include "Chest.h"


Chest::Chest()
{
	this->x = 6.0f;
	this->y = 0.0f;
	this->z = -0.5f;

	this->sphere = new SphereIntersection();
	sphere->setCenterX(-0.1760 * 0.9f);
	sphere->setCenterY(10.8174 * 0.9f);
	sphere->setCenterZ(-8.0619 * 0.9f);
	sphere->setRadius(12.0284 * 0.9f);
	sphere->setX(x*4.0f);
	sphere->setY(1.4f);
	sphere->setZ(z*6.5f);

}

Chest::~Chest(){}

unsigned int Chest::getCoins(){ return coins;}

void Chest::reduceCoins(int x){
	this->coins-=x;
}

float Chest::getX(){return x;}

float Chest::getY(){return y;}

float Chest::getZ(){return z;}

void Chest::setX(float x){this->x = x;}

void Chest::setY(float y){this->y = y;}

void Chest::setZ(float z){this->z = z;}

void Chest::setCoins(unsigned int coins){this->coins = coins;}

SphereIntersection* Chest::getSphere() { return sphere; }
void Chest::setSphere(SphereIntersection * s){this->sphere = s;}