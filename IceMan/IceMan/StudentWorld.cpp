#include "StudentWorld.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}
StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir)
{
	t += 1;
}

// Get a tile from a specific location.
Actor* StudentWorld::at(int x, int y) {
	if (x < 0 || y < 0 || x >= VIEW_WIDTH || y >= VIEW_WIDTH)
		return nullptr;
	else
		return map[x][y];
}
void StudentWorld::removeIce(Ice* ice) {
	map[ice->x()][ice->y()] = nullptr;
	ice->kill();
}
Actor* StudentWorld::getPlayer() {
	return player;
}
int StudentWorld::init()
{
	Actor* tile;
	/*clean up map array*/memset(map, 0, sizeof(map));

	int current_level_number = getLevel();

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
		allItems.push_back(tile);
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

	/* Protesters */

	allActors.push_back(new Protester(this, 60, 60));
	allActors.push_back(new HardcoreProtester(this, 60, 60));
	return GWSTATUS_CONTINUE_GAME;
}
void StudentWorld::addNewItem() {
	if (rand(0, this->G_chance) < 1) {
		if (rand(0, 5) < 1) {
			/* Add a new Sonar */
			allActors.push_back(new Sonar(this, 0, VIEW_HEIGHT - SPRITE_HEIGHT));
		}
		else {
			/* Add a water pool */
			allActors.push_back(new Water(this, VIEW_WIDTH - SPRITE_WIDTH, VIEW_HEIGHT - SPRITE_HEIGHT));
		}
	}
}
int StudentWorld::move()
{
	// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	//decLives();
	if (getNumberOfBarrelsRemainingToBePickedUp() == 0) {
		return GWSTATUS_FINISHED_LEVEL;
	}
	if (player->health() == 0) {
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	for (auto actor : allActors) {
 		actor->doSomething();
	}
	updateDisplayText();
	addNewItem();

	auto it = allActors.begin();
	while (it != allActors.end()) {
		if (!(*it)->isAlive()) {
			delete (*it);
			it = allActors.erase(it);
		}
		else {
			it++;
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}
void StudentWorld::cleanUp()
{
	for (auto each : allActors) {
		delete each;
	}
	allActors.erase(allActors.begin(), allActors.end());
}
int StudentWorld::getPlayerGoldCount() {
	return player->gold();
}
int StudentWorld::getSquirtsLeftInSquirtGun() {
	return player->squirt();
}
int StudentWorld::getPlayerSonarChargeCount() {
	return player->sonar();
}
int StudentWorld::getCurrentHealth() {
	return player->health() * 10;
}
int StudentWorld::getNumberOfBarrelsRemainingToBePickedUp() {
	return this->number_barrel - player->barrel();
}
std::string StudentWorld::format() {
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
void StudentWorld::updateDisplayText() {
	setGameStatText(format());
}
void StudentWorld::dropSquirt() {
	Actor* squirt = new Squirt(this, this->player->x(), this->player->y(), this->player->getDirection());
	allActors.push_back(squirt);
}
// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

