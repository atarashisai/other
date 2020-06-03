#include "StudentWorld.h"
#include "Actor.h"
#include <string>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}
StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir)
{ }

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

/* withinMap
Check if the position is outside of the map.
*/
inline bool StudentWorld::outsideMap(int x, int y) {
	return (x < 0 || y < 0 || x >= VIEW_WIDTH || y >= VIEW_WIDTH);
}

/* at
Get a tile from the map.
* A tile can be an 'Ice' object or a 'boulder' object.
* This method will return nullptr if
* 1. position (x, y) is outside of the map.
* 2. map[x][y] contains nullptr.
*/
Actor* StudentWorld::at(int x, int y) {
	if (outsideMap(x, y))
		return nullptr;
	else
		return map[x][y];
}

/* player
Return the only 'Iceman' object as a player.
*/
Iceman* StudentWorld::player() {
	return _player;
}

/* onInit */
int StudentWorld::init()
{
	Actor* tile;

	int current_level_number = getLevel();

	/*clean up map array*/memset(this->map, 0, sizeof(this->map));

	/* Calculate the number of boulders, gold nuggets and barrels of oil */
	int B = std::min(current_level_number / 2 + 2, 9);
	int G = std::max(5 - current_level_number / 2, 2);
	int L = std::min(2 + current_level_number, 21);
	int T = max(25, 200 - current_level_number);
	int P = min(15, 2 + (int)(current_level_number * 1.5));
	number_barrel = L;

	/* There is a 1 in G chance that a new Water Pool or Sonar Kit Goodie will be added */
	this->G_chance = current_level_number * 25 + 300;
	/* Boulder */
	for (int n = 0; n < B; n++) {
		int x = rand(0, VIEW_HEIGHT - SPRITE_HEIGHT);
		int y = rand(20, 56);
		tile = new Boulder(this, x, y);
		allActors.push_back(tile);

		/* A boulder should occupy the 4x4 space */
		for (int dx = 0; dx < SPRITE_WIDTH; dx++) {
			for (int dy = 0; dy < SPRITE_HEIGHT; dy++) {
				map[x+dx][y+dy] = tile;
			}
		}
	}
	/* Ice */
	for (int x = 0; x < 64; x++) {
		for (int y = 0; y < 60; y++) {
			if (map[x][y] != nullptr) {
				continue;
			}
			if ((x >= 30 && x <= 33 && y >= 4)) {
				continue;
			}
			else {
				map[x][y] = new Ice(this, x, y);
				allActors.push_back(map[x][y]);
			}
		}
	}
	/* Gold */
	for (int n = 0; n < G; n++) {
		tile = new Gold(this, rand(0, VIEW_HEIGHT - SPRITE_HEIGHT), rand(20, 56));
		treasure.push_back(tile);
		allActors.push_back(tile);
	}
	/* Barrel */
	for (int n = 0; n < L; n++) {
		tile = new Barrel(this, rand(0, VIEW_HEIGHT - SPRITE_HEIGHT), rand(20, 56));
		treasure.push_back(tile);
		allActors.push_back(tile);
	}
	/* Water */
	tile = new Water(this, rand(0, 60), rand(20, 56));
	allActors.push_back(tile);

	/* Iceman */
	this->_player = new Iceman(this);
	this->allActors.push_back(this->_player);

	/* Protesters */

	allActors.push_back(new Protester(this, 60, 60));
	allActors.push_back(new HardcoreProtester(this, 60, 60));
	return GWSTATUS_CONTINUE_GAME;
}

/* onMove */
int StudentWorld::move()
{
	if (getNumberOfBarrelsRemainingToBePickedUp() == 0) {
		return GWSTATUS_FINISHED_LEVEL;
	}
	if (player()->health() == 0) {
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	for (auto actor : allActors) {
		if (actor->isAlive())
			actor->doSomething();
		else {
			allActors.remove(actor);
			delete actor;
		}

	}
	updateDisplayText();
	dropNewItem();

	auto it = allActors.begin();
	while (it != allActors.end()) {
		if (!static_cast<Actor*>(*it)->isAlive()) {
			delete (*it);
			it = allActors.erase(it);
		}
		else {
			it++;
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}

/* onCleanUp */
void StudentWorld::cleanUp()
{
	for (auto actor : allActors) {
		delete actor;
	}
	this->allActors.clear();
	this->treasure.clear();
	this->_player = nullptr;
	this->G_chance = 0;
	this->number_barrel = 0;

	/*clean up map array*/memset(this->map, 0, sizeof(this->map));
}

/* Below are helper functions to support updateDisplayText()
* getPlayerGoldCount()
* getSquirtsLeftInSquirtGun()
* getPlayerSonarChargeCount()
* getCurrentHealth()
* getNumberOfBarrelsRemainingToBePickedUp()
* format()
*/
inline int StudentWorld::getPlayerGoldCount() {
	return player()->gold();
}
inline int StudentWorld::getSquirtsLeftInSquirtGun() {
	return player()->squirt();
}
inline int StudentWorld::getPlayerSonarChargeCount() {
	return player()->sonar();
}
inline int StudentWorld::getCurrentHealth() {
	return player()->health() * 10;
}
inline int StudentWorld::getNumberOfBarrelsRemainingToBePickedUp() {
	return this->number_barrel - player()->barrel();
}
inline std::string StudentWorld::format() {
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
/* updateDisplayText
* Draw status text on top of the screen.
* This method is only called by move().
*/
inline void StudentWorld::updateDisplayText() {
	setGameStatText(format());
}

/* dropSquirt
Drop a squirt on the map based on player's position and facing.
*/
bool StudentWorld::dropSquirt(Actor* actor) {
	Actor* squirt = new Squirt(this, actor->getX(), actor->getY(), actor->getDirection());
	allActors.push_back(squirt);
	return true; /* always return true to match the pattern of other dropXXX functions */
}
/* dropGold
Drop a gold on the map based on player's position.
*/
bool StudentWorld::dropGold(Actor* actor) {
	Gold* gold = new Gold(this, actor->getX(), actor->getY());
	gold->setLost();
	allActors.push_back(gold);
	return true; /* always return true to match the pattern of other dropXXX functions */
}

/* dropNewItem
Drop a new item, a sonar or water pool, based on probability.
*/
bool StudentWorld::dropNewItem() {
	if (rand(0, this->G_chance) < 1) {
		if (rand(0, 5) < 1) {
			/* Add a new Sonar */
			allActors.push_back(new Sonar(this, 0, VIEW_HEIGHT - SPRITE_HEIGHT));
		}
		else {
			/* Add a water pool */
			allActors.push_back(new Water(this, VIEW_WIDTH - SPRITE_WIDTH, VIEW_HEIGHT - SPRITE_HEIGHT));
		}
		return true;
	}
	return false;
}

/* dropEnemy
Drop a protester,normal or hardcore, based on probability.
*/
bool StudentWorld::dropEnemy() {
	return false;
}

/* dropBoulder
Drop a falling boulder to replace the static boulder.
*/
bool StudentWorld::dropBoulder(Boulder* boulder) {
	Boulder* stone = new Boulder(*boulder);
	allActors.push_back(stone);
	return false;
}

/* allTreasure
Iterate all hidden items with a callback lambda function.
*/
void StudentWorld::allTreasure(std::function<void(Actor*)> callback) {
	for (auto item : treasure) {
		callback(item);
	}
}