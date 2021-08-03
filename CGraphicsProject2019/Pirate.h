#pragma once

#include "GeometryNode.h"
#include "SphereIntersection.h"
#include "ParticleSystem.h"

class Pirate{

private:

	//auto pirate_start;
	float x, y, z;
	float next_x, next_z;
	float continuous_x, continuous_z;

	float enter_time;
	int roadID;
	int health;
	bool isDead;
	int direction; // 0=up , 1=right , 2=left , 3=down //0=forward_up 1=forward_left 2=forward_down 3=forward_right 4=turn_left 5=turn_right 
	float speed;

	bool playsDead;

	float interpolant; // a for mix

	int type; // 0 for normal , 1 for boss
	float size; //based on type.

	float rythm; // time passing by according the speed. --> needed for glm::mix

	float changeTileTime; // interval for switching tile for movement.
	float reachChangeTileTime;  // used for reaching the above 



	
	
	//pirate body
	class GeometryNode* renderPirateBody;
	glm::mat4 pirateBody_transformation_matrix;
	glm::mat4 pirateBody_transformation_normal_matrix;
	//pirate arm
	class GeometryNode* renderPirateArm;
	glm::mat4 pirateArm_transformation_matrix;
	glm::mat4 pirateArm_transformation_normal_matrix;
	//pirate left foot
	class GeometryNode* renderPirateLeftFoot;
	glm::mat4 pirateLeftFoot_transformation_matrix;
	glm::mat4 pirateLeftFoot_transformation_normal_matrix;
	//pirate right foot
	class GeometryNode* renderPirateRightFoot;
	glm::mat4 pirateRightFoot_transformation_matrix;
	glm::mat4 pirateRightFoot_transformation_normal_matrix;


	// we want collision detection and particles !!!
	SphereIntersection * sphere;
	ParticleEmitter* particles;
	



public:

	Pirate();
	~Pirate();
	int getType();
	float getRythm();
	float getSize();
	float getX();
	float getY();
	float getZ();
	float getNextX();
	float getNextZ();
	float getContX();
	float getContZ();
	int getRoadID();
	int getDir();
	float getSpeed();
	bool getDead();
	int getHealth();
	float getEnterTime();
	float getInterpolant();
	float getChangeTileTime();
	float getReachChangeTileTime();
	SphereIntersection* getSphere();
	ParticleEmitter* getParticles();
	void setX(float x);
	void setY(float y);
	void setZ(float z);
	void setContX(float);
	void setContZ(float);
	void setHealth(int h);
	void setRoadID(int id);
	void setDead(bool isDead);
	void setDir(int dir);
	void setSpeed(float s);
	void setNextX(float x);
	void setNextZ(float z);
	void setEnterTime(float e_t);
	void setSphere(SphereIntersection * s);
	void setParticles(ParticleEmitter* p);
	void setInterpolant(float);
	void setType(int);
	void setSize(float);
	void setRythm(float);
	void setChangeTileTime(float);
	void setReachChangeTileTime(float);

	bool playingDead();
	void setPlayingDead(bool);
	



	class GeometryNode* getRenderPirateBody();
	glm::mat4 getPirateBodyTransformationMatrix();
	glm::mat4 getPirateBodyTransformationNormalMatrix();

	class GeometryNode* getRenderPirateArm();
	glm::mat4 getPirateArmTransformationMatrix();
	glm::mat4 getPirateArmTransformationNormalMatrix();

	class GeometryNode* getRenderPirateRightFoot();
	glm::mat4 getPirateRightFootTransformationMatrix();
	glm::mat4 getPirateRightFootTransformationNormalMatrix();

	class GeometryNode* getRenderPirateLeftFoot();
	glm::mat4 getPirateLeftFootTransformationMatrix();
	glm::mat4 getPirateLeftFootTransformationNormalMatrix();

	void setRenderPirateBody(class GeometryNode * node);
	void setPirateBodyTransformationMatrix(glm::mat4 matrix);
	void setPirateBodyTransformationNormalMatrix(glm::mat4 matrix);

	void setRenderPirateArm(class GeometryNode * node);
	void setPirateArmTransformationMatrix(glm::mat4 matrix);
	void setPirateArmTransformationNormalMatrix(glm::mat4 matrix);

	void setRenderPirateLeftFoot(class GeometryNode * node);
	void setPirateLeftFootTransformationMatrix(glm::mat4 matrix);
	void setPirateLeftFootTransformationNormalMatrix(glm::mat4 matrix);

	void setRenderPirateRightFoot(class GeometryNode * node);
	void setPirateRightFootTransformationMatrix(glm::mat4 matrix);
	void setPirateRightFootTransformationNormalMatrix(glm::mat4 matrix);



	void playDead();



};