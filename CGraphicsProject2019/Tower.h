#pragma once

#include "glm\glm.hpp"
#include <vector>
#include "GeometryNode.h"

class Tower
{
private:
	//tower coordinates
	float x;
	float y;
	float z;

	//range
	int range;
	//dagame infilction to the pirates
	int damage;

	//level of the tower. For 5 kills, you gain the ability to upgrade the tower.
	int level;

	int fireRate; // interval for shooting (msecs)
	int fireNow; // should be 20 for level 1 and 15 for level 2 in order for the tower to shoot. It will be updated every 100 msecs so it will need 2 secs for the level 1 tower to shoot etc...

	int kills; //for upgrade!

	bool toBeChanged;
	bool toBeUpgraded;

	// we put those here because every tower needs its own transformation matrix and GeometryNode.
	// ALTERNATIVE: array of towers nodes in renderer.h
	class GeometryNode* renderTower;
	glm::mat4 tower_transformation_matrix;
	glm::mat4 tower_transformation_normal_matrix;

	//collision





public:
	Tower();
	~Tower();
	float getX();
	float getY();
	float getZ();
	int getRange();
	int getDamage();
	int getLevel();
	int getFireNow();
	int getFireRate();
	int getKills();
	bool getToBeChanged();
	bool getToBeUpgraded();
	void setToBeUpgraded(bool);
	void setToBeChanged(bool);
	void setX(float x);
	void setY(float y);
	void setZ(float z);
	void setRange(int r);
	void setDagame(int d);
	void setLevel(int l);
	void setFireNow(int f);
	void setFireRate(int f);
	void setKills(int);
	class GeometryNode* getRenderTower();
	glm::mat4 getTransformationMatrix();
	glm::mat4 getTransformationNormalMatrix();
	void setRenderTower(class GeometryNode * node);
	void setTransformationMatrix(glm::mat4 matrix);
	void setTransformationNormalMatrix(glm::mat4 matrix);

	void goUnderGround(float);

	
	
	

};