#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <memory>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
private:
	std::vector<Actor*> allActors;
	std::vector<Actor*> allItems;
	Iceman* player;
	int G_chance = 0;
	int number_barrel = 0;
	std::mt19937 mt{ std::random_device{}() };
	int t = 0;
	Actor* map[VIEW_WIDTH][VIEW_HEIGHT];
public:
	int rand(int min, int max) {
		return std::uniform_int_distribution<int>{min, max}(mt);
	}
	StudentWorld(std::string assetDir);
	Actor* at(int x, int y);
	void removeIce(Ice* ice);
	Actor* getPlayer();
	virtual int init();
	void addNewItem();
	virtual int move();

	virtual void cleanUp();
	int getPlayerGoldCount();
	int getSquirtsLeftInSquirtGun();
	int getPlayerSonarChargeCount();
	int getCurrentHealth();
	int getNumberOfBarrelsRemainingToBePickedUp();
	std::string format();
	void updateDisplayText();
};

#endif // STUDENTWORLD_H_