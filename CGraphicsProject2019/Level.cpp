#include "Level.h"
#include <iostream>
#include <algorithm>
#include "OBJLoader.h"

Level::Level()
{
	//the coordinates of the road tiles
	this->roadTiles[0] = std::tuple<int, int>(0, 0);
	this->roadTiles[1] = std::tuple<int, int>(0, 1);
	this->roadTiles[2] = std::tuple<int, int>(0, 2);
	this->roadTiles[3] = std::tuple<int, int>(0, 3);
	this->roadTiles[4] = std::tuple<int, int>(1, 3);
	this->roadTiles[5] = std::tuple<int, int>(1, 4);
	this->roadTiles[6] = std::tuple<int, int>(1, 5);
	this->roadTiles[7] = std::tuple<int, int>(1, 6);
	this->roadTiles[8] = std::tuple<int, int>(1, 7);
	this->roadTiles[9] = std::tuple<int, int>(2, 7);
	this->roadTiles[10] = std::tuple<int, int>(2, 8);
	this->roadTiles[11] = std::tuple<int, int>(3, 8);
	this->roadTiles[12] = std::tuple<int, int>(4, 8);
	this->roadTiles[13] = std::tuple<int, int>(5, 8);
	this->roadTiles[14] = std::tuple<int, int>(6, 8);
	this->roadTiles[15] = std::tuple<int, int>(6, 7);
	this->roadTiles[16] = std::tuple<int, int>(6, 6);
	this->roadTiles[17] = std::tuple<int, int>(7, 6);
	this->roadTiles[18] = std::tuple<int, int>(7, 5);
	this->roadTiles[19] = std::tuple<int, int>(7, 4);
	this->roadTiles[20] = std::tuple<int, int>(7, 3);
	this->roadTiles[21] = std::tuple<int, int>(8, 3);
	this->roadTiles[22] = std::tuple<int, int>(9, 3);
	this->roadTiles[23] = std::tuple<int, int>(9, 2);
	this->roadTiles[24] = std::tuple<int, int>(9, 1);
	this->roadTiles[25] = std::tuple<int, int>(8, 1);
	this->roadTiles[26] = std::tuple<int, int>(7, 1);
	this->roadTiles[27] = std::tuple<int, int>(6, 1);
	this->roadTiles[28] = std::tuple<int, int>(6, 0);

	//initialise chest
	this->chest = new Chest();
	this->gold = 300; 
	this->chest->setCoins(this->gold);
	//this->chest->setCoins(300);

	//initialise coordinates for selection tile
	this->selectionTileX = 3;
	this->selectionTileZ = 2;

	this->builtTowers = std::vector<Tower*>();
	this->availableTowers = std::vector<Tower*>();

	// beggining the game with 3 towers!
	for (int i = 0; i < 3; i++)
	{
		this->availableTowers.push_back(new Tower());
	}
	
	this->pirates = std::vector<Pirate*>();
	//Pirate* p = new Pirate();
	//p->setEnterTime(0.0f);
	//p->setRythm(0.0f);
	//p->setType(0);
	//this->addPirate(p,p->getType());
	//Pirate * p = new Pirate();
	//p->setX(std::get<0>(this->roadTiles[0]));
	//p->setZ(std::get<1>(this->roadTiles[0]));
	//
	//this->pirates.push_back(new Pirate());

	this->cannonballs = std::vector<CannonBall*>();

	this->pirateReachedChest = false;
	this->gameOver = false;
	this->paused = false;
	this->wasPaused = false;

	this->score = 0;
	this->canUpgradeTower = false;
	this->wave = 1;
	this->pirateBoss = false;
	this->stopWaves = false;

	this->pirateRate = 2000; //msecs.


}



Level::~Level() { 

	delete[] this->roadTiles; delete this->chest; 
	for (Tower * t : this->availableTowers) delete t;
	for (Tower * t : this->builtTowers) delete t;
	for (Pirate * p : this->pirates) delete p;
	for (CannonBall* c : this->cannonballs) delete c;
}


std::tuple<int, int>* Level::getRoadTiles() 
{
	return this->roadTiles;
}

int Level::getRoadTileX(int number_of_road_tile) {
	return  std::get<0>(this->roadTiles[number_of_road_tile]);
}

int Level::getRoadTileZ(int number_of_road_tile) {
	return  std::get<1>(this->roadTiles[number_of_road_tile]);
}

unsigned int Level::getGold()
{
	return this->gold;
}

void Level::setGold(unsigned int gold)
{
	this->gold = gold;
}

Chest* Level::getChest() {
	return this->chest;
}

void Level::setChest(Chest * chest)
{
	this->chest = chest;
}

bool Level::getBoss() { return this->pirateBoss; }
void Level::setBoss(bool flag) { this->pirateBoss = flag; }

bool Level::getStopWaves() { return this->stopWaves; }

void Level::setStopWaves(bool f)
{
	this->stopWaves = f;
}

int Level::getPirateRate() { return this->pirateRate; }
void Level::setPirateRate(int r) { this->pirateRate = r; }


int Level::getSelectionTileX()
{
	return this->selectionTileX;
}

int Level::getSelectionTileZ()
{
	return this->selectionTileZ;
}

void Level::setSelectionTileX(int tileX){
	this->selectionTileX = tileX;
}
void Level::setSelectionTileZ(int tileZ){
	this->selectionTileZ = tileZ;
}

bool Level::checkSelectionTileInRoad() {
	//bool isTrue = false;
	for (auto tuple : this->roadTiles)
	{
		if(std::get<0>(tuple) == this->selectionTileX && std::get<1>(tuple) ==this->selectionTileZ){
			//printf("Hello");
			//isTrue=true;
			return true;
		}
	}

	
	//printf("Hello False");
	return false;
}

bool Level::checkSelectionTileInTower()
{
	//bool isTrue = false;
	for (Tower * t : this->builtTowers) {
		if (t->getX() == this->selectionTileX && t->getZ() == this->selectionTileZ)
			return true;
	}
	return false;
}

void Level::setUpgradeTower(bool flag)
{
	this->canUpgradeTower = flag;
}
bool Level::getUpgradeTower() { return this->canUpgradeTower; }



std::vector<Tower*> Level::getAvailableTowers(){
	return this->availableTowers;
}

std::vector<Tower*> Level::getBuiltTowers(){
	return this->builtTowers;
}

bool Level::addTower(){
	if (this->availableTowers.empty()) {
		return false;
	}
	else{
		float x = (float)this->selectionTileX;
		float z = (float)this->selectionTileZ;
		for (Tower * t : this->builtTowers)
		{
			if (x == t->getX() && z == t->getZ()) return false;
		}
		for (int i=0;i<NUMBER_OF_ROAD_TILES;i++){
			if (getRoadTileX(i) == x && this->getRoadTileZ(i) == z) return false;
		}
		//creating coordinates for the tower --> maybe Z not Y

		this->availableTowers.back()->setX(x);
		this->availableTowers.back()->setZ(z);
		this->builtTowers.push_back(this->availableTowers.back());
		this->availableTowers.pop_back();
		return true;
	}
	
}



bool Level::changeTileOfTower(){

	if (this->builtTowers.empty()) return false;

	float x = (float)this->selectionTileX;
	float z = (float)this->selectionTileZ;
	//int i = 0;
	for (Tower * t : this->builtTowers)
	{
		if (x == t->getX() && z == t->getZ())
		{
			t->setToBeChanged(true);
			//Tower * deleted = this->builtTowers[i];
			//deleted->goUnderGround();
			//if (deleted->getY() < -3)
			//{
				//this->builtTowers.erase(this->builtTowers.begin() + i); // finding the index of the t Tower we want to remove.
				//this->availableTowers.push_back(deleted);
				//this->availableTowers.insert(this->availableTowers.begin(), deleted);
				//this->availableTowers.push_back(t); //not working
			//}
			
			return true;
		}
		//i++;

	}
	return false;
}

void Level::deleteUndergroundTowers()
{
	int i = 0;
	for (Tower* t : this->builtTowers)
	{
		if (t->getToBeChanged() && t->getY() < -5.f)
		{
			this->availableTowers.insert(this->availableTowers.begin(), this->builtTowers[i]);
			this->builtTowers.erase(this->builtTowers.begin() + i);
		}
		i++;
	}
}


void Level::addTowerToAvailableTowers(Tower * tower){
	OBJLoader loader;	
	auto mesh = loader.load("../Data/MedievalTower/tower.obj");
	tower->setRenderTower(new GeometryNode());
	tower->getRenderTower()->Init(mesh);

	this->availableTowers.push_back(tower);
}

int Level::upgradeTower()
{
	OBJLoader loader;
	
	if (this->builtTowers.size() != 0)
	{
		float x = (float)this->selectionTileX;
		float z = (float)this->selectionTileZ;
		int i = 0;
		for (Tower* t : this->builtTowers)
		{
			if (t->getX() == x && t->getZ() == z)
			{
				Tower* selected = this->builtTowers[i];
				int lvl = selected->getLevel();
				if (lvl < 2)
				{
					if (lvl == 1  && !selected->getToBeUpgraded())
					{
						printf("You can not upgrade yet!\n ");
						return 0;
					}
					auto mesh = loader.load("../Data/MedievalTowerLevel2/tower.obj");
					lvl += 1;
					selected->setLevel(lvl);
					selected->setFireRate(1500);
					selected->setDagame(15);
					selected->setRange(2);
					selected->setRenderTower(new GeometryNode());
					selected->getRenderTower()->Init(mesh);
					//std::cout << "UPGRADED!!!" << std::endl;
					selected->setToBeUpgraded(false);
					return 2;
				}
			}
			i++;
		}
	}
	return -1;
}

std::vector<Pirate*> Level::getPirates(){
	return this->pirates;
}

std::vector<CannonBall*> Level::getCannonBalls(){
	return this->cannonballs;
}

void Level::setWave(int w){
	this->wave = w;
}

int Level::getWave(){
	return wave;
}

unsigned int Level::getScore()
{
	return this->score;
}

void Level::setScore(unsigned int score)
{
	this->score = score;
}


bool Level::getGO() { return this->gameOver; }

void Level::setGO(bool gameOver) { this->gameOver = gameOver; }

bool Level::getReachedChest() { return this->pirateReachedChest; }

void Level::setReachedChest(bool reached) { this->pirateReachedChest = reached; }

void Level::setPaused(bool flag) { this->paused = flag; }
bool Level::isPaused() { return this->paused; }

void Level::setWasPaused(bool flag) { this->wasPaused = flag; }
bool Level::getWasPaused() { return this->wasPaused; }

void Level::updateNextTileOfPirate(Pirate* p)
{
	

	if (p->getDead() || p->playingDead()) return;

		// Check if pirate has reached the prev target
		//bool reached = false;
		/*if (abs(p->getContX() - p->getNextX()) < 0.001 && abs(p->getContZ() - p->getNextZ()) < 0.001 && p->getRoadID() != 29) {
			int ID = p->getRoadID();
			p->setRoadID(ID + 1);

		} */

	if (p->getRoadID() != 29 )
	{
			int ID = p->getRoadID();
			p->setRoadID(ID + 1);
	} 
		

		/*if (a>=0.9f)
		{
			//std::cout << " A IS: " << a << std::endl;
			int ID = p->getRoadID();
			p->setRoadID(ID + 1);
		} */

		// Get in which index is in the road
	int idx = p->getRoadID();

		// Set the target to the next x,z coordinates using the road tiles
	if (idx + 1 < 29) {
			float x_new = std::get<0>(this->roadTiles[idx + 1]);
			float z_new = std::get<1>(this->roadTiles[idx + 1]);
			//printf("new z inside function is:%f\n", z_new);
			p->setX(std::get<0>(this->roadTiles[idx]));
			p->setZ(std::get<1>(this->roadTiles[idx]));
			//p->setContX(p->getX());
			//p->setContZ(p->getZ());
			p->setNextX(x_new);
			p->setNextZ(z_new);

			//std::cout << " z from function is now: " << p->getZ() << " and next z from function is: " << p->getNextZ() << std::endl;
	}
	else {
			// Set index to 29 as an indicator that it finished 
			p->setRoadID(29);
			p->setX(std::get<0>(this->roadTiles[idx]));
			p->setZ(std::get<1>(this->roadTiles[idx]));
			//p->setContX(p->getX());
			//p->setContZ(p->getZ());
			float x_new = 6;
			float z_new = -1;
			//std::cout << "reached chest!!! " << std::endl;
			p->setNextX(x_new);
			p->setNextZ(z_new);
	}

	
}




void Level::addPirate(float enterTime,int type) {
	OBJLoader loader;
	Pirate* p = new Pirate();
	p->setType(type);
	p->setChangeTileTime(1.0f / 1.0f);

	auto mesh = loader.load("../Data/Pirate/pirate_body.obj");
	if (type == 1)
	{
		mesh = loader.load("../Data/PirateBoss/pirate_body.obj");
		//std::cout << "LOADED BLACK HEADBAND!!! " << std::endl;
	}
	p->setRenderPirateBody(new GeometryNode());
	p->getRenderPirateBody()->Init(mesh);

	mesh = loader.load("../Data/Pirate/pirate_arm.obj");
	if (type == 1)
	{
		mesh = loader.load("../Data/PirateBoss/pirate_arm.obj");
	}
	p->setRenderPirateArm(new GeometryNode());
	p->getRenderPirateArm()->Init(mesh);

	mesh = loader.load("../Data/Pirate/pirate_left_foot.obj");
	p->setRenderPirateLeftFoot(new GeometryNode());
	p->getRenderPirateLeftFoot()->Init(mesh);

	mesh = loader.load("../Data/Pirate/pirate_right_foot.obj");
	p->setRenderPirateRightFoot(new GeometryNode());
	p->getRenderPirateRightFoot()->Init(mesh);

	if (type == 1)
	{
		p->setSpeed(0.6f);
		p->setSize(1.5f);
		p->setHealth(90);
		p->setChangeTileTime(1.0f / 0.6f);
	}

	p->setX(std::get<0>(this->roadTiles[0]));
	p->setZ(std::get<1>(this->roadTiles[0]));

	p->setContX(std::get<0>(this->roadTiles[0]));
	p->setContZ(std::get<1>(this->roadTiles[0]));

	p->setNextX(std::get<0>(this->roadTiles[1]));
	p->setNextZ(std::get<1>(this->roadTiles[1]));

	p->setEnterTime(enterTime);
	p->setRythm(enterTime);

	this->pirates.push_back(p);
}

void Level::addCannonBall(Tower* tower, Pirate * pirate)
{
	OBJLoader loader;
	CannonBall * cb = new CannonBall();
	cb->setX(tower->getX()*4 - 0.0101);
	cb->setY(tower->getY()*4 + 3.0);
	cb->setZ(tower->getZ()*4 - 0.0758);
	
	//std::cout<<"x " << cb->getX() << " y " << cb->getY() << " z " << cb->getZ() << std::endl;

	//cb->setNextX((pirate->getX() - cb->getX())/10000000.0f);
	//cb->setNextY((pirate->getY() - cb->getY())/10000000.0f);
	//cb->setNextZ((pirate->getZ() - cb->getZ())/10000000.0f);

	cb->setPirate(pirate);
	cb->setDamage(tower->getDamage());
	cb->setTower(tower);

	auto mesh = loader.load("../Data/Various/cannonball.obj");
	cb->setRenderBall(new GeometryNode());
	cb->getRenderBall()->Init(mesh);

	if (tower->getLevel() == 2)
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); // 0 to 1
		if (r <= 0.3f) //30% probability for energy ball
		{
			cb->setSpeed(0.5f);
			cb->setDamage(tower->getDamage()*2);
			cb->setEnergyBall(true);
		}
	}

	this->cannonballs.push_back(cb);
}

void Level::towersShoot(){
	for(Tower * tower : this->builtTowers)
	{
		int fire = tower->getFireNow();
		if (fire == tower->getFireRate() / 100)
		{
			for (Pirate * pirate : this->pirates)
			{
				if (pirate->getDead() || pirate->playingDead()) continue;
				float pirate_x = pirate->getX();
				float pirate_z = pirate->getZ();
				float tower_x = tower->getX();
				float tower_z = tower->getZ();
				if (abs(tower_x - pirate_x) <= tower->getRange() && abs(tower_z - pirate_z) <= tower->getRange())
				{
					addCannonBall(tower, pirate);
					break;
				}

			}

			tower->setFireNow(0);
		}
		else
		{
			tower->setFireNow((fire + 1) % (tower->getFireRate() / 100 + 1));
		}
		
	}
}

void Level::deleteCannonball(CannonBall * c){
	this->cannonballs.erase(std::remove(this->cannonballs.begin(), this->cannonballs.end(), c), this->cannonballs.end());
	delete c;
}

void Level::deletePirate(Pirate * p){
	this->pirates.erase(std::remove(this->pirates.begin(), this->pirates.end(), p), this->pirates.end());
	delete p;
}

void Level::checkChestCollision()
{


	for (Pirate* p : this->pirates)
	{
		if (p->getDead() || p->playingDead()) return;
		if (this->chest->getSphere()->isIntersection(p->getSphere()))
		{
			//std::cout << "REACHED CHEST " << std::endl;
			this->pirateReachedChest = true; //particles 
			if (p->getType() == 1)
			{
				this->gold -= 60;
				this->setStopWaves(false);
			}
			else
			{
				this->gold -= 20;
			}

			this->deletePirate(p);
			//std::cout << " Gold is: " << this->gold << std::endl;
			if (gold <= 0) {
				this->gameOver = true;
				std::cout << "GAME OVER" << std::endl;
			}
		}
	}
	
}

/*
 * after some time passed delete the dead pirates to stop the particles.
*/
void Level::deleteDeadPirates()
{
	for (Pirate* p : this->pirates)
	{
		if (p->getDead()) this->deletePirate(p);
	}
}

void Level::killPiratesForGood()
{
	for (Pirate* p : this->pirates)
	{
		if (p->playingDead()) p->setDead(true);
	}
}



