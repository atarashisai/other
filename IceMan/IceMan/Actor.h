#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameWorld.h"
#include <algorithm>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
private:
	/* */
protected:
	GameWorld* _gw;
	int _x;
	int _y;
	bool _alive;
public:
	Actor(
		GameWorld* gw, 
		int imageID, 
		bool visible, 
		int startX, 
		int startY, 
		Direction dir = right, 
		double size = 1.0, 
		unsigned int depth = 2
	): GraphObject(imageID, startX, startY, dir, size, depth) {
		this->_x = startX;
		this->_y = startY;
		this->_gw = gw;
		this->_alive = true;
		setVisible(visible);
	}
	virtual ~Actor() { }
	virtual void doSomething() {}
	virtual GameWorld* getWorld() { return this->_gw; };
	virtual void kill() {
		this->_alive = false;
		this->setVisible(false);
	}
	bool isAlive() { return this->_alive; }
	int x() { return this->_x; };
	int y() { return this->_y; };
};
class Ice : public Actor {
public:
	Ice(GameWorld* gw, int x, int y): 
		Actor(gw, IID_ICE, true, x, y, right, 0.25, 3) {};
	void doSomething() {};
	~Ice() { }
};
class Boulder : public Actor {
public:
	Boulder(GameWorld* gw, int x, int y): 
		Actor(gw, IID_BOULDER, true, x, y, down, 1, 1) {
	};
	virtual void doSomething();
	void removeIce();
	~Boulder() { }
};
class Goody : public Actor {
public:
	using Actor::Actor;
	void doSomething() {}
	virtual ~Goody() {}
};
class Sonar : public Goody {
private:
	int countdown;
public:
	Sonar(GameWorld* gw, int x, int y) :
		Goody(gw, IID_SONAR, true, x, y), 
		countdown(std::max(100, 300 - 10 * (int)gw->getLevel())) {}
	void doSomething();
	~Sonar() {}
};

class Water : public Goody {
private:
	int countdown;
public:
	Water(GameWorld* gw, int x, int y) :
		Goody(gw, IID_WATER_POOL, true, x, y) {}
	void doSomething();
	~Water() {}
};

class Gold : public Goody {
private:
	int countdown;
	bool isPermanent = true;
public:
	Gold(GameWorld* gw, int x, int y) :
		Goody(gw, IID_GOLD, true, x, y) {}
	void doSomething();
	~Gold() {}
};

class Barrel : public Goody {
private:
	int countdown;
public:
	Barrel(GameWorld* gw, int x, int y):
		Goody(gw, IID_BARREL, true, x, y) {}
	void doSomething();
	~Barrel() {}
};
class Iceman : public Actor {
private:
	int number_barrel;
	int number_squirt;
	int number_sonar;
	int number_gold;
	int hit_point;
public:
	Iceman(GameWorld* gw, int x = 30, int y = 60) :
		Actor(gw, IID_PLAYER, true, x, y, right, 1, 0),
		number_barrel(0),
		number_squirt(5),
		number_sonar(1), 
		number_gold(0),
		hit_point(10) {}
	virtual void doSomething();
	~Iceman() {}
	int gold();
	int sonar();
	int squirt();
	int health();
	int barrel();
	void addGold();
	void addSonar();
	void addSquirt();
	void addBarrel();
	void removeTile() const;
	bool passable(int x, int y);
};

class Protester : public Actor {
private:
	int ticksToWaitBetweenMoves;
	int step;
	/* */
public:
	/* */
	Protester(GameWorld* gw, int x, int y, const int IID= IID_PROTESTER) :
		Actor(gw, IID, true, x, y) {
		this->ticksToWaitBetweenMoves = std::max(unsigned int(0), 3 - this->_gw->getLevel() / 4);
		this->step = 10;
	}
	~Protester() {}
	void doSomething();
	bool passable(int x, int y);
};
class HardcoreProtester : public Protester {
private:
	/* */
public:
	/* */
	HardcoreProtester(GameWorld* gw, int x, int y) :
		Protester(gw, x, y, IID_HARD_CORE_PROTESTER) {}
	~HardcoreProtester() {}
	void doSomething();
};
#endif // ACTOR_H_