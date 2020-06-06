#include "Actor.h"
#include "StudentWorld.h"
#include <assert.h>
#include <utility>
#include <set>
#include <stack>
using namespace std;
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

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
		for (auto p : world()->protesters()) {
			if (distance_square(this, p) <= 9) {
				world()->playSound(SOUND_PROTESTER_FOUND_GOLD);
				kill();
				p->bebribed();
				return;
			}
		}
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
		}
		for (auto p : world()->protesters()) {
			if (distance_square(p, this) <= 9) {
				p->beannoyed(100);
				world()->increaseScore(500);
			}
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
			if (getDirection() != left) {
				setDirection(left);
				return;
			}
			x = getX() - 1;
			y = getY();
			if (passable(x, y)) {
				removeTile(x, y);
				moveTo(x, y);
			}
			break;
		case KEY_PRESS_RIGHT:
			/*... move player to the right ...; */
			if (getDirection() != right) {
				setDirection(right);
				return;
			}
			x = getX() + 1;
			y = getY();
			if (passable(x, y)) {
				removeTile(x, y);
				moveTo(x, y);
			}
			break;
		case KEY_PRESS_DOWN:
			/*... move player down ...; */
			if (getDirection() != down) {
				setDirection(down);
				return;
			}
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
			if (getDirection() != up) {
				setDirection(up);
				return;
			}
			x = getX();
			y = getY() + 1;
			if (passable(x, y)) {
				removeTile(x, y);
				moveTo(x, y);
			}
			break;
		case 'z':
		case 'Z':
			if (this->sonar() > 0) {
				this->number_sonar -= 1;
				world()->allTreasure([this](Actor* item) {
					if (distance_square(this, item) <= 144) {
						item->setVisible(true);
					}
				});
			}
			break;
		case KEY_PRESS_TAB:
		case 't':
		case 'T':
			if (this->gold() > 0) {
				this->number_gold -= 1;
				world()->dropGold(this);
			}
			break;
		case KEY_PRESS_ESCAPE:
			kill();
			break;
		case KEY_PRESS_SPACE:
			/*... add a Squirt in front of the player...; */
			if (this->squirt() > 0) {
				this->number_squirt--;
				switch (getDirection())
				{
				case left:
					world()->dropSquirt(this, getX() - SPRITE_WIDTH, getY());
					break;
				case right:
					world()->dropSquirt(this, getX() + SPRITE_WIDTH, getY());
					break;
				case up:
					world()->dropSquirt(this, getX(), getY() + SPRITE_HEIGHT);
					break;
				case down:
					world()->dropSquirt(this, getX(), getY() - SPRITE_HEIGHT);
					break;
				default:
					break;
				}
				world()->playSound(SOUND_PLAYER_SQUIRT);
			}
			break;
			// etc…
		}
	}
}
bool Iceman::passable(int x, int y) {
	if (x < 0 || y < 0 || x > VIEW_WIDTH - SPRITE_WIDTH || y > VIEW_HEIGHT - SPRITE_HEIGHT)
		return false;
	if (dynamic_cast<Boulder*>(world()->at(x + 0, y + 2)) != nullptr) {
		return false;
	}
	if (dynamic_cast<Boulder*>(world()->at(x + 2, y + 0)) != nullptr) {
		return false;
	}
	if (dynamic_cast<Boulder*>(world()->at(x + 2, y + 4)) != nullptr) {
		return false;
	}
	if (dynamic_cast<Boulder*>(world()->at(x + 4, y + 2)) != nullptr) {
		return false;
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
			if (world()->removeTile<Ice>(x + deltaX, y + deltaY))
				world()->playSound(SOUND_DIG);
		}
	}
}

void Iceman::kill() {
	this->hit_point = 0;
	world()->playSound(SOUND_PLAYER_GIVE_UP);
}
void Iceman::annoyed(int hit) {
	this->hit_point = std::max(this->hit_point - hit, 0);
}
int Protester::maxWait() {
	return std::max(unsigned int(0), 3 - this->_gw->getLevel() / 4);
}

void Protester::doSomething() {
	int x, y; /* temp var for actor's current position */
	if (!isAlive()) return;
	if (this->ticksToWaitBetweenMoves > 0) {
		this->ticksToWaitBetweenMoves--;
		return;
	}

	this->ticksToWaitBetweenMoves = std::max(unsigned int(0), 3 - this->_gw->getLevel() / 4);
	
	if (this->leaving && getX() == 60 && getY() == 60) {
		kill();
		return;
	}
	if (this->thinking) {
		std::future_status status = this->fut.wait_for(std::chrono::seconds(1));
		if (status == std::future_status::deferred) {
			return;
		}
		else if (status == std::future_status::timeout) {
			return;
		}
		else if (status == std::future_status::ready) {
			this->thinking = false;
			this->path = this->fut.get();
			if (this->path.size() > 0) {
				this->path.pop(); /* remove the original location */
			}
		}
	}
	/* leaving */
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
	if (this->ticksSinceLastShout > 0) {
		this->ticksSinceLastShout--;
	}
	if (distance_square(this, world()->player()) <= 16) {
		if (getDirection() == left && world()->player()->getX() < this->getX()) {
			shout();
			return;
		}

		if (getDirection() == right && world()->player()->getX() > this->getX()) {
			shout();
			return;
		}

		if (getDirection() == down && world()->player()->getY() < this->getY()) {
			shout();
			return;
		}

		if (getDirection() == up && world()->player()->getY() > this->getY()) {
			shout();
			return;
		}
	}

	Iceman* player = world()->player();
	if (this->getX() == player->getX() || this->getY() == player->getY()) {
		if (distance_square(this, world()->player()) > 16) {
			Direction playersDirection;
			if (player->getX() < this->getX())
				playersDirection = left;
			else if (player->getX() > this->getX())
				playersDirection = right;
			else if (player->getY() < this->getY())
				playersDirection = down;
			else
				playersDirection = up;
			if (getDirection() != playersDirection) {
				bool accessible = true;
				std::function<int(int)> fx;
				std::function<int(int)> fy;
				x = this->getX();
				y = this->getY();
				switch (playersDirection) {
				case left:
					fx = [](int x) -> int { return x - 1; };
					fy = [](int y) -> int { return y; };
					break;
				case right:
					fx = [](int x) -> int { return x + 1; };
					fy = [](int y) -> int { return y; };
					break;
				case up:
					fx = [](int x) -> int { return x; };
					fy = [](int y) -> int { return y + 1; };
					break;
				case down:
					fx = [](int x) -> int { return x; };
					fy = [](int y) -> int { return y - 1; };
					break;
				}
				while (x != player->getX() || y != player->getY()) {
					x = fx(x);
					y = fy(y);
					accessible &= passable(x, y);
					if (!accessible)
						break;
				}
				if (accessible) {
					setDirection(playersDirection);
					move(playersDirection, this->getX(), this->getY());
					std::queue<std::pair<int, int>>().swap(this->path);
					return;
				}
			}
		}
	}

	if (this->haveAnotherPlanOnMove()) { /* create a callback handler for childern. */
		return;
	}

	/* Random motion */
	if (this->numSquaresToMoveInCurrentDirection > 0)
		this->numSquaresToMoveInCurrentDirection--;
	if (this->ticksSinceLastTurn > 0)
		this->ticksSinceLastTurn--;

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
		setDirection(dir);
		/* Reset numSquaresToMoveInCurrentDirection due to changing direction */
		this->numSquaresToMoveInCurrentDirection = 8 + world()->rand(0, 52);
		return;
	}
	else if (possible_move.size() > 2) {
		/* make a perpendicular turn */
		if (this->ticksSinceLastTurn == 0) {
			this->ticksSinceLastTurn = 200;
			this->numSquaresToMoveInCurrentDirection = 0;
			switch (dir) {
			case left:
			case right:
				possible_move.erase(right);
				possible_move.erase(left);
				break;
			case up:
			case down:
				possible_move.erase(up);
				possible_move.erase(down);
				break;
			}
		}
		if (this->numSquaresToMoveInCurrentDirection == 0) {
			possible_move.erase(dir);
			std::set<Direction>::iterator it = possible_move.begin();
			for (int i = world()->rand(0, possible_move.size() - 1); i > 0; i--) {
				it++;
			}
			if ((dir == up || dir == down) && (*(it) != up && *(it) != down)) {
				this->ticksSinceLastTurn = 200;
			}
			if ((dir == right || dir == left) && (*(it) != right && *(it) != left)) {
				this->ticksSinceLastTurn = 200;
			}
			dir = *(it);
			this->numSquaresToMoveInCurrentDirection = 8 + world()->rand(0, 52);
		}
	}
	move(dir, getX(), getY());
	return;
}
void Protester::move(Direction dir, int x, int y) {
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
}
bool Actor::passable(int x, int y) {
	if (x < 0 || y < 0 || x > VIEW_WIDTH - SPRITE_WIDTH || y > VIEW_HEIGHT - SPRITE_HEIGHT)
		return false;
	for (int dx = 0; dx < SPRITE_WIDTH; dx++) {
		for (int dy = 0; dy < SPRITE_HEIGHT; dy++) {
			if (world()->at(x + dx, y + dy) != nullptr) {
				return false;
			}
		}
	}
	if (dynamic_cast<Boulder*>(world()->at(x + 0, y + 2)) != nullptr) {
		return false;
	}
	if (dynamic_cast<Boulder*>(world()->at(x + 2, y + 0)) != nullptr) {
		return false;
	}
	if (dynamic_cast<Boulder*>(world()->at(x + 2, y + 4)) != nullptr) {
		return false;
	}
	if (dynamic_cast<Boulder*>(world()->at(x + 4, y + 2)) != nullptr) {
		return false;
	}
	return true;
}
void Protester::defeat() {
	findpath(60, 60);
	this->leaving = true;
}
void Protester::findpath(int destination_x, int destination_y) {
	fut = std::async(aStarSearch<VIEW_WIDTH, VIEW_HEIGHT, Actor*>, world()->map,
		make_pair(getX(), getY()),
		make_pair(destination_x, destination_y),
		[this](int x, int y) -> bool { return this->passable(x, y); });
	this->thinking = true;
}
void Protester::shout() {
	if (this->ticksSinceLastShout > 0) {
		return;
	}
	world()->player()->annoyed(2);
	world()->playSound(SOUND_PROTESTER_YELL);
	this->ticksSinceLastShout = 15;
}
void Protester::bebribed() {
	this->hit_point = 0;
	world()->increaseScore(25);
}
void Protester::bekilled() {
	world()->increaseScore(100);
}
void Protester::bestunned() {
	int N = std::max(50, int(100 - world()->getLevel() * 10));
	wait(N);
}
bool Protester::isLeaving() {
	return this->leaving;
}
int Protester::health() {
	return this->hit_point;
}
void Protester::wait(int tick) {
	this->ticksToWaitBetweenMoves = tick;
}
void Protester::beannoyed(int hit) {
	this->hit_point = std::max(this->hit_point-hit, 0);
	if (this->hit_point > 0) {
		world()->playSound(SOUND_PROTESTER_ANNOYED);
	}
	else {
		world()->playSound(SOUND_PROTESTER_GIVE_UP);
		this->numSquaresToMoveInCurrentDirection = 0;
		defeat();
		wait(0);
	}
}
bool HardcoreProtester::haveAnotherPlanOnMove() {
	if (this->makingPlan) {
		int size = path.size();
		if (size > 0) {
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
			return true;
		}
		else {
			this->makingPlan = false;
			this->giveupPlan = size - this->alertDistance;
			return false;
		}
	}
	if (this->giveupPlan > 0) {
		this->giveupPlan--;
		return false;
	}
	if (distance_square(this, world()->player()) <= 16) {
		return false;
	}
	if (distance_square(this, world()->player()) <= std::pow(this->alertDistance, 2)) {
		findpath(world()->player()->getX(), world()->player()->getY());
		this->makingPlan = true;
		return true;
	}
	return false;
}
void HardcoreProtester::bebribed() {
	world()->increaseScore(50);
	int ticks_to_stare = std::max(50, int(100 - world()->getLevel() * 10));
	wait(ticks_to_stare);
}
void HardcoreProtester::bekilled() {
	world()->increaseScore(250);
}
int HardcoreProtester::calcAlertDistance() {
	return 16 + world()->getLevel() * 2;
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
	for (auto p : world()->protesters()) {
		if (distance_square(this, p) <= 9) {
			p->beannoyed(2);
			if (p->health() == 0) {
				p->bekilled();
			}
			else {
				p->bestunned();
			}
			this->kill();
			return;
		}
	}

	if (this->can_travel > 0) {
		this->can_travel--;
		switch (this->getDirection())
		{
		case right:
			if (passable(this->x() + 1, this->y()))
				this->moveTo(this->x() + 1, this->y());
			else {
				this->kill();
				return;
			}
			break;
		case left:
			if (passable(this->x() - 1, this->y()))
				this->moveTo(this->x() - 1, this->y());
			else {
				this->kill();
				return;
			}
			break;
		case up:
			if (passable(this->x(), this->y() + 1))
				this->moveTo(this->x(), this->y() + 1);
			else {
				this->kill();
				return;
			}
			break;
		case down:
			if (passable(this->x(), this->y() - 1))
				this->moveTo(this->x(), this->y() - 1);
			else {
				this->kill();
				return;
			}
			break;
		default:
			break;
		}
	}
	if (this->can_travel == 0) {
		this->kill();
		return;
	}
}
