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
	int rand(int min, int max) {
		return std::uniform_int_distribution<int>{min, max}(mt);
	}
	Actor* map[VIEW_WIDTH][VIEW_HEIGHT];
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
		t += 1;
	}
	Actor* getTile(int x, int y) {
		return map[x][y];
	}
	Actor* getPlayer() {
		return player;
	}
	virtual int init()
	{
		Actor* tile;
		int current_level_number = getLevel();

		/* Calculate the number of boulders, gold nuggets and barrels of oil */
		int B = std::min(current_level_number / 2 + 2, 9);
		int G = std::max(5 - current_level_number / 2, 2);
		int L = std::min(2 + current_level_number, 21);
		number_barrel = L;

		/* There is a 1 in G chance that a new Water Pool or Sonar Kit Goodie will be added */
		this->G_chance = current_level_number * 25 + 300;
		/* Ice */
		for (int x = 0; x < 64; x++) {
			for (int y = 0; y < 60; y++) {
				if (x >= 30 && x <= 33) {
					map[x][y] = nullptr;
				}
				else {
					map[x][y] = new Ice(this, x, y);
					allActors.push_back(map[x][y]);
				}
			}
			for (int y = 60; y < 64; y++) {
				map[x][y] = nullptr;
			}
		}
		/* Boulder */
		for (int n = 0; n < B; n++) {
			tile = new Boulder(this, rand(0, VIEW_HEIGHT - SPRITE_HEIGHT), rand(20, 56));
			allItems.push_back(tile);
			allActors.push_back(tile);
		}
		/* Gold */
		for (int n = 0; n < G; n++) {
			tile = new Gold(this, rand(0, VIEW_HEIGHT - SPRITE_HEIGHT), rand(20, 56));
			allItems.push_back(tile);
			allActors.push_back(tile);
		}
		/* Barrel */
		for (int n = 0; n < L; n++) {
			tile = new Barrel(this, rand(0, VIEW_HEIGHT - SPRITE_HEIGHT), rand(20, 56));
			allItems.push_back(tile);
			allActors.push_back(tile);
		}
		/* Water */
		tile = new Water(this, rand(0, 60), rand(20, 56));
		allActors.push_back(tile);
		/* Iceman */
		player = new Iceman(this);
		allActors.push_back(player);
		return GWSTATUS_CONTINUE_GAME;
	}
	void addNewItem() {
		if (rand(0, this->G_chance) < 1) {
			if (rand(0, 5) < 1) {
				/* Add a new Sonar */
				allActors.push_back(new Sonar(this, 0, VIEW_HEIGHT - SPRITE_HEIGHT));
			}
			else {
				/* Add a water pool */
				allActors.push_back(new Water(this, VIEW_WIDTH-SPRITE_WIDTH, VIEW_HEIGHT - SPRITE_HEIGHT));
			}
		}
	}
	virtual int move()
	{	
		// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
		//decLives();
		if (getNumberOfBarrelsRemainingToBePickedUp() == 0) {
			return GWSTATUS_FINISHED_LEVEL;
		}
		if (player->health() == 0) {
			decLives();
			return GWSTATUS_CONTINUE_GAME;
		}
		for (auto actor : allActors) {
			actor->doSomething();
		}
		updateDisplayText();
		addNewItem();
		return GWSTATUS_CONTINUE_GAME;
	}

	virtual void cleanUp()
	{
		for (auto actor : allActors) {
			delete actor;
		}
	}
	int getPlayerGoldCount() {
		return player->gold();
	}
	int getSquirtsLeftInSquirtGun() {
		return player->squirt();
	}
	int getPlayerSonarChargeCount() {
		return player->sonar();
	}
	int getCurrentHealth() {
		return player->health()*10;
	}
	int getNumberOfBarrelsRemainingToBePickedUp() {
		return this->number_barrel - player->barrel();
	}
	std::string format() {
		std::ostringstream ostr;
		ostr << "Lvl: "
			<< std::setw(2) << getLevel()
			<< " Lives: "
			<< std::setw(1) << getLives()
			<< " Hlth: "
			<< std::setw(2) << getCurrentHealth() << "%"
			<< " Wtr: "
			<< std::setw(2) << getSquirtsLeftInSquirtGun()
			<< " Gld: "
			<< std::setw(2) << getPlayerGoldCount()
			<< " Oil Left: "
			<< std::setw(2) << getNumberOfBarrelsRemainingToBePickedUp()
			<< " Sonar: "
			<< std::setw(2) << getPlayerSonarChargeCount()
			<< " Scr: " 
			<< std::setw(6) << getScore();
		return ostr.str();
	}
	void updateDisplayText() {
		setGameStatText(format());
	}
private:
};

#endif // STUDENTWORLD_H_
