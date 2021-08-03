#pragma once
#include "SphereIntersection.h"
class Chest
{
private:
	unsigned int coins; //number of coins
	float x, y, z; //coordinates


	//  collision
	SphereIntersection* sphere;
public:

	Chest();
	~Chest();

	unsigned int getCoins();
	void reduceCoins(int x);
	float getX();
	float getY();
	float getZ();
	SphereIntersection* getSphere();
	void setX(float x);
	void setY(float y);
	void setZ(float z);
	void setCoins(unsigned int coins);
	void setSphere(SphereIntersection * s);
};
