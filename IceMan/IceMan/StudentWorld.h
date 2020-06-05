#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <memory>
#include <list>
#include <functional>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Ice;
class Boulder;
class Sonar;
class Water;
class Gold;
class Barrel;
class Iceman;
class Protester;
class HardcoreProtester;
class Squirt;

class StudentWorld : public GameWorld
{
private:
	std::list<Actor*> allActors;
	std::list<Actor*> treasure;
	std::list<Protester*> allProtesters;
	Iceman* _player;
	int G_chance = 0;
	int maxNumberOfProtester = 0;
	unsigned int ticksSinceLastProtesterAdded = 0;
	int number_barrel = 0;
	std::mt19937 mt{ std::random_device{}() };
	//Actor* map[VIEW_WIDTH][VIEW_HEIGHT];

	/* Help functions */
	int getPlayerGoldCount();
	int getSquirtsLeftInSquirtGun();
	int getPlayerSonarChargeCount();
	int getCurrentHealth();
	int getNumberOfBarrelsRemainingToBePickedUp();
	std::string format();
	void updateDisplayText();
	bool outsideMap(int x, int y);
public:
	/* Override virtual methods */
	StudentWorld(std::string assetDir);
	int init();
	int move();
	void cleanUp();
	/* Interface for Actor classes */
	Actor* at(int x, int y);
	Iceman* player();
	std::list<Protester*> protesters();
	bool dropSquirt(Actor* actor, int x, int y);
	bool dropGold(Actor* actor);
	bool dropEnemy();
	bool dropNewItem();
	bool dropBoulder(Boulder* boulder);
	void allTreasure(std::function<void(Actor*)> callback);
	int rand(int min, int max) {
		return std::uniform_int_distribution<int>{min, max}(mt);
	}
	/* Template functions */

	/* removeTile
	Remove an tile from the map.
	* Return true if an tile is removed.
	* Return false and do nothing if map[x][y] is not an T object.
	*/
	template<typename T> bool removeTile(int x, int y) {
		if (!outsideMap(x, y)) {
			Actor* tile = map[x][y];
			if (dynamic_cast<T*>(tile) != nullptr) {
				this->map[x][y] = nullptr;
				tile->kill();
				return true;
			}
		}
		return false;
	}
	/* Test functions */
	template<typename T> void killAll() {
		for (auto actor : allActors) {
			if (dynamic_cast<T*>(actor) != nullptr) {
				dynamic_cast<T*>(actor)->defeat();
			}
		}
	}
	Actor* map[VIEW_WIDTH][VIEW_HEIGHT];
};

#endif // STUDENTWORLD_H_