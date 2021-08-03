#pragma once

#include "Pirate.h"
#include "GeometricMesh.h"
#include "GeometryNode.h"
#include "Chest.h"
#include "Tower.h"
#include "GLEW\glew.h"
#include "glm\glm.hpp"
#include <vector>
#include "CannonBall.h"




#define NUMBER_OF_ROAD_TILES 29


class Level
{
private:
	std::tuple<int, int> roadTiles[NUMBER_OF_ROAD_TILES];
	
	Chest* chest;
	
	int gold;
	unsigned int score;

	int wave; // wave for pirates.

	//coordinates of selected road tile
	int selectionTileX, selectionTileZ;

	//towers that can be built
	std::vector<Tower*> availableTowers;
	//towers that have been built
	std::vector<Tower*> builtTowers;

	//pirates
	std::vector<Pirate*> pirates;

	//cannonballs
	std::vector<CannonBall*> cannonballs;

	bool gameOver;
	bool pirateReachedChest;
	bool paused;
	bool wasPaused;

	bool canUpgradeTower;

	bool pirateBoss;
	bool stopWaves; // stop waves when the boss is on.
	
	int pirateRate; // rate in wave.



public:
	Level();
	~Level();
	std::tuple<int, int>* getRoadTiles();

	//returns x coordinate of the road tile
	int getRoadTileX(int number_of_road_tile);
	//returns y coordinate of the road tile
	int getRoadTileZ(int number_of_road_tile);

	unsigned int getGold();
	void setGold(unsigned int gold);

	Chest* getChest();

	void setChest(Chest* chest);

	int getSelectionTileX();
	int getSelectionTileZ();


	void setSelectionTileX(int tileX);
	void setSelectionTileZ(int tileZ);

	//checks if the color of the current selected road tile should be green or red (green=false red=true)
	bool checkSelectionTileInRoad();
	bool checkSelectionTileInTower();

	std::vector<Tower*> getAvailableTowers();

	std::vector<Tower*> getBuiltTowers();

	std::vector<Pirate*> getPirates();

	std::vector<CannonBall*> getCannonBalls();

	void setWave(int w);
	int getWave();

	bool getBoss();
	void setBoss(bool);

	bool getStopWaves();
	void setStopWaves(bool);

	int getPirateRate();
	void setPirateRate(int);

	unsigned int getScore();
	void setScore(unsigned int score);

	bool getGO();
	void setGO(bool gameOver);

	bool getReachedChest();
	void setReachedChest(bool reached);

	bool getUpgradeTower();
	void setUpgradeTower(bool);

	void setPaused(bool flag);
	bool isPaused();

	void setWasPaused(bool flag);
	bool getWasPaused();

	bool addTower();

	bool changeTileOfTower();

	void addTowerToAvailableTowers(Tower * tower);

	void updateNextTileOfPirate(Pirate*);

	void addPirate(float,int);

	void addCannonBall(Tower* tower, Pirate * pirate);
	
	void towersShoot();

	void deleteCannonball(CannonBall * c);

	void deletePirate(Pirate * p);

	void checkChestCollision();

	void deleteDeadPirates();

	void killPiratesForGood();

	int upgradeTower(); // returns the level

	void deleteUndergroundTowers();
};