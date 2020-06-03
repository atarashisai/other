#include "Actor.h"
#include "StudentWorld.h"
#include <assert.h>
#include <utility>
#include <set>
#include <stack>
using namespace std;
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

inline double distance_square(Actor* o, Actor* t) {
	return std::pow(o->x() - t->x(), 2) + std::pow(o->y() - t->y(), 2);
}

void Gold::doSomething() {
	if (!isAlive()) return;
	if (isPermanent) {
		Iceman* player = world()->player();
		if (distance_square(this, player) <= 9) {
			world()->playSound(SOUND_GOT_GOODIE);
			player->addGold();
			world()->increaseScore(10);
			kill();
			return;
		}
		if (distance_square(this, player) <= 16) {
			setVisible(true);
			return;
		}
	}
	else {
		this->countdown -= 1;
		if (this->countdown <= 0) {
			kill();
			return;
		}
	}
}

void Gold::setLost() {
	this->countdown = maxCount();
	this->isPermanent = false;
	setVisible(true);
}

void Barrel::doSomething() {
	if (!isAlive()) return;
	Iceman* player = world()->player();
	if (distance_square(this, player) <= 9) {
		world()->playSound(SOUND_FOUND_OIL);
		player->addBarrel();
		world()->increaseScore(1000);
		kill();
		return;
	}
	if (distance_square(this, player) <= 16) {
		setVisible(true);
		return;
	}
}

void Boulder::doSomething() {
	if (!isAlive()) return;
	/* State: may be able to fall, check all supporting ices. */
	if (!this->rolling) {
		bool no_support = true;
		for (int dx = 0; dx < SPRITE_WIDTH; dx++) {
			Ice* ice = dynamic_cast<Ice*>(world()->at(this->x() + dx, this->y() - 1));
			if (!(no_support &= (ice == nullptr)))
				return;
		}
		if (no_support) {
			this->rolling = true;
			this->countdown = 30;
		}
		return;
	}
	/* State: No supporting ice, about to fall */
	if (this->countdown > 1) {
		//Count down to 1.
		//During countdown, this boulder is still static.
		this->countdown -= 1;
		return;
	}
	/* State: No supporting ice, about to fall */
	if (this->countdown == 1) {
		this->countdown -= 1;
		/*Now,
		//This boulder is not a static object on the map.
		//So we will remove it from the map.
		//.
		//Of cause we will also kill it,
		//So we need also replace it with a moving boulder;*/
		world()->dropBoulder(this);
		for (int dx = 0; dx < SPRITE_WIDTH; dx++) {
			for (int dy = 0; dy < SPRITE_HEIGHT; dy++) {
				world()->removeTile<Boulder>(getX() + dx, getY() + dy);
			}
		}
		world()->playSound(SOUND_FALLING_ROCK);
		return;
	}
	/* State: No supporting ice, falling */
	else {
		//Requirment.Boulder.onTick.4.A.(a)
		if (getY() == 0) { 
			this->kill();
			return;
		}
		//Requirment.Boulder.onTick.4.A.(b/c)
		for (int dx = 0; dx < SPRITE_WIDTH; dx++) {
			Actor* tile = world()->at(this->x() + dx, this->y() - 1);
			if (dynamic_cast<Ice*>(tile) != nullptr || dynamic_cast<Boulder*>(tile) != nullptr) {
				this->kill();
				return;
			}
		}
		//Requirment.Boulder.onTick.4.B
		if (distance_square(world()->player(), this) <= 9) {
			world()->player()->kill();
			this->kill();
			return;
		}
		this->moveTo(this->x(), this->y() - 1);
	}
}
int Supply::maxCount() {
	return std::max(100, 300 - int(10 * world()->getLevel()));
}
void Supply::doSomething() {
	if (!isAlive()) return;
	this->countdown -= 1;
	if (this->countdown <= 0) {
		setVisible(false);
		this->_alive = false;
	}
	Iceman* player = world()->player();
	if (distance_square(this, player) <= 9) {
		world()->playSound(SOUND_GOT_GOODIE);
		this->receiveSupply();
		kill();
	}
}

void Water::receiveSupply() {
	world()->player()->addSquirt();
	world()->increaseScore(100);
}

void Sonar::receiveSupply() {
	world()->player()->addSonar();
	world()->increaseScore(75);
}

void Iceman::doSomething() {
	if (!isAlive()) return;
	int ch;
	int x, y; /* temp var for actor's current position */
	if (world()->getKey(ch) == true)
	{
		// user hit a key this tick!
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			/*... move player to the left ...;*/
			x = getX() - 1;
			y = getY();
			setDirection(left);
			if (passable(x, y)) {
				removeTile(x, y);
				moveTo(x, y);
			}
			break;
		case KEY_PRESS_RIGHT:
			/*... move player to the right ...; */
			x = getX() + 1;
			y = getY();
			setDirection(right);
			if (passable(x, y)) {
				removeTile(x, y);
				moveTo(x, y);
			}
			break;
		case KEY_PRESS_DOWN:
			/*... move player down ...; */
			x = getX();
			y = getY() - 1;
			setDirection(down);
			if (passable(x, y)) {
				removeTile(x, y);
				moveTo(x, y);
			}
			break;
		case KEY_PRESS_UP:
			/*... move player up ...; */
			x = getX();
			y = getY() + 1;
			setDirection(up);
			if (passable(x, y)) {
				removeTile(x, y);
				moveTo(x, y);
			}
			break;
		case 'z':
		case 'Z':
			if (this->sonar() > 0) {
				//this->number_sonar -= 1;
				world()->allTreasure([this](Actor* item) {
					if (distance_square(this, item) <= 144) {
						item->setVisible(false);
					}
				});
			}
			break;
		case 'k':
		case 'K':
			world()->killAll<Protester>();
			break;
		case KEY_PRESS_TAB:
			if (this->gold() > 0) {
				//this->number_gold -= 1;
				world()->dropGold(this);
			}
			break;
		case KEY_PRESS_ESCAPE:
			kill();
			break;
		case KEY_PRESS_SPACE:
			/*... add a Squirt in front of the player...; */
			if (this->squirt() > 0) {
				world()->dropSquirt(this);
				world()->playSound(SOUND_PLAYER_SQUIRT);
				//this->number_squirt--;
			}
			break;
			// etc…
		}
	}
}
bool Iceman::passable(int x, int y) {
	if (x < 0 || y < 0 || x > VIEW_WIDTH - SPRITE_WIDTH || y > VIEW_HEIGHT - SPRITE_HEIGHT)
		return false;
	for (int dx = 0; dx < SPRITE_WIDTH; dx++) {
		for (int dy = 0; dy < SPRITE_HEIGHT; dy++) {
			if (dynamic_cast<Boulder*>(world()->at(x + dx, y + dy)) != nullptr) {
				return false;
			}
		}
	}
	return true;
}
int Iceman::gold() {
	return this->number_gold;
}
int Iceman::sonar() {
	return this->number_sonar;
}
int Iceman::squirt() {
	return this->number_squirt;
}
int Iceman::barrel() {
	return this->number_barrel;
}
int Iceman::health() {
	return this->hit_point;
}
inline void Iceman::addGold() {
	this->number_gold += 1;
}
inline void Iceman::addSonar() {
	this->number_sonar += 1;
}
inline void Iceman::addSquirt() {
	this->number_squirt += 5;
}
inline void Iceman::addBarrel() {
	this->number_barrel += 1;
}
void Iceman::removeTile(int x, int y) {
	for (int deltaX = 0; deltaX < SPRITE_WIDTH; deltaX++) {
		for (int deltaY = 0; deltaY < SPRITE_HEIGHT; deltaY++) {
			world()->removeTile<Ice>(x + deltaX, y + deltaY);
		}
	}
}

void Iceman::kill() {
	this->hit_point = 0;
	world()->playSound(SOUND_PLAYER_GIVE_UP);
}
int Protester::maxWait() {
	return std::max(unsigned int(0), 3 - this->_gw->getLevel() / 4);
}

void Protester::doSomething() {
	if (!isAlive()) return;
	if (this->ticksToWaitBetweenMoves > 0) {
		this->ticksToWaitBetweenMoves--;
		return;
	}
	this->ticksToWaitBetweenMoves = std::max(unsigned int(0), 3 - this->_gw->getLevel() / 4);
	if (this->leaving) {
		if (path.size() > 0) {
			std::pair<int, int> p = path.front();
			path.pop();
			/* Adjust facing */
			Direction dir;
			if (p.first > getX())
				dir = right;
			else if (p.first < getX())
				dir = left;
			else if (p.second > getY())
				dir = up;
			else
				dir = down;
			if (getDirection() != dir)
				setDirection(dir);
			/* Advance */
			moveTo(p.first, p.second);
		}
		else {
			kill();
		}
		return;
	}


	/* Random motion */
	if (this->numSquaresToMoveInCurrentDirection > 0)
		this->numSquaresToMoveInCurrentDirection -= 1;

	std::set<Direction> possible_move;
	if (passable(getX() + 1, getY()))
		possible_move.emplace(right);
	if (passable(getX() - 1, getY()))
		possible_move.emplace(left);
	if (passable(getX(), getY() + 1))
		possible_move.emplace(up);
	if (passable(getX(), getY() - 1))
		possible_move.emplace(down);

	Direction dir = getDirection();
	bool can_move_forward = std::find(possible_move.begin(), possible_move.end(), dir) != possible_move.end();
	if (!can_move_forward) {
		std::set<Direction>::iterator it = possible_move.begin();
		for (int i = world()->rand(0, possible_move.size() - 1); i > 0; i--) {
			it++;
		}
		dir = *(it);
	}
	else if (possible_move.size() > 2) {
		if (world()->rand(0, 100) > 30 && this->numSquaresToMoveInCurrentDirection == 0) {
			possible_move.erase(dir);
			std::set<Direction>::iterator it = possible_move.begin();
			for (int i = world()->rand(0, possible_move.size() - 1);i > 0;i--) {
				it++;
			}
			dir = *(it);
			this->numSquaresToMoveInCurrentDirection = 8 + world()->rand(0, 52);
		}
	}
	int x, y; /* temp var for actor's current position */
	switch (dir)
	{
	case right:
		x = getX() + 1;
		y = getY();
		setDirection(right);
		moveTo(x, y);
		break;
	case left:
		x = getX() - 1;
		y = getY();
		setDirection(left);
		moveTo(x, y);
		break;
	case up:
		x = getX();
		y = getY() + 1;
		setDirection(up);
		moveTo(x, y);
		break;
	case down:
		x = getX();
		y = getY() - 1;
		setDirection(down);
		moveTo(x, y);
		break;
	default:
		break;
	}
	return;
}
bool Protester::passable(int x, int y) {
	if (x < 0 || y < 0 || x > VIEW_WIDTH - SPRITE_WIDTH || y > VIEW_HEIGHT - SPRITE_HEIGHT)
		return false;
	StudentWorld* world = static_cast<StudentWorld*>(this->_gw);
	for (int dx = 0; dx < SPRITE_WIDTH; dx++) {
		for (int dy = 0; dy < SPRITE_HEIGHT; dy++) {
			if (world->at(x + dx, y + dy) != nullptr) {
				return false;
			}
		}
	}
	return true;
}
void Protester::defeat() {
	this->path = aStarSearch<VIEW_WIDTH, VIEW_HEIGHT>(
		world()->map, 
		make_pair(getX(), getY()), 
		make_pair(60, 60),
		[this](int x, int y) -> bool { return this->passable(x, y); }
	);
	this->leaving = true;
}
void HardcoreProtester::doSomething() {
	Protester::doSomething();
}
void Squirt::doSomething() {
	if (!isAlive()) return;
	Actor* tile = world()->at(this->x(), this->y());
	if (dynamic_cast<Ice*>(tile) != nullptr) {
		this->kill();
		return;
	}
	if (dynamic_cast<Boulder*>(tile) != nullptr) {
		this->kill();
		return;
	}

	if (this->can_travel > 0) {
		this->can_travel--;
		switch (this->getDirection())
		{
		case right:
			if (this->x() < VIEW_WIDTH - SPRITE_WIDTH)
				this->moveTo(this->x() + 1, this->y());
			else
				this->kill();
			break;
		case left:
			if (this->x() > 0)
				this->moveTo(this->x() - 1, this->y());
			else
				this->kill();
			break;
		case up:
			if (this->y() < VIEW_HEIGHT - SPRITE_HEIGHT)
				this->moveTo(this->x(), this->y() + 1);
			else
				this->kill();
			break;
		case down:
			if (this->y() > 0)
				this->moveTo(this->x(), this->y() - 1);
			else
				this->kill();
			break;
		default:
			break;
		}
	}
	if (this->can_travel == 0) {
		this->kill();
	}
}

