#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <algorithm>
#include <utility>
#include <iostream>
#include <stack>
#include <set>
#include <vector>
#include <queue>
#include <functional>
#include <future>
#include <chrono>
class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
private:
	/* */
protected:
	StudentWorld* _gw;
	bool _alive;
public:
	Actor(
		StudentWorld* gw,
		int imageID, 
		bool visible, 
		int startX, 
		int startY, 
		Direction dir = right, 
		double size = 1.0, 
		unsigned int depth = 2
	): GraphObject(imageID, startX, startY, dir, size, depth) {
		this->_gw = gw;
		this->_alive = true;
		setVisible(visible);
	}
	virtual ~Actor() { }
	virtual void doSomething() {}
	StudentWorld* world() { return this->_gw; };
	void kill() {
		this->_alive = false;
		this->setVisible(false);
	}
	bool isAlive() { return this->_alive; }
	int x() { return this->getX(); };
	int y() { return this->getY(); };
	bool passable(int x, int y);
};
class Ice : public Actor {
public:
	Ice(StudentWorld* gw, int x, int y):
		Actor(gw, IID_ICE, true, x, y, right, 0.25, 3) {};
	void doSomething() {};
	~Ice() { kill(); }
};
class Boulder : public Actor {
private:
	int countdown;
	bool rolling;
public:
	Boulder(StudentWorld* gw, int x, int y):
		Actor(gw, IID_BOULDER, true, x, y, down, 1, 1), 
		rolling(false),
		countdown(-1) {
	};
	Boulder(const Boulder& rhs):
		Actor(rhs._gw, IID_BOULDER, true, rhs.getX(), rhs.getY(), down, 1, 1),
		rolling(rhs.rolling),
		countdown(rhs.countdown) {
	}
	virtual void doSomething();
	~Boulder() { kill(); }
};
class Goody : public Actor {
protected:
	int countdown;
public:
	using Actor::Actor;
	void doSomething() {}
	virtual ~Goody() { kill(); }
};
class Supply : public Actor {
protected:
	int countdown;
public:
	using Actor::Actor;
	void doSomething();
	virtual ~Supply() { kill(); }
	virtual void receiveSupply() {};
	int maxCount();
};

class Sonar : public Supply {
public:
	Sonar(StudentWorld* gw, int x, int y) :
		Supply(gw, IID_SONAR, true, x, y)
	{
		this->countdown = maxCount();
	}
	void receiveSupply();
	~Sonar() { kill(); }
};

class Water : public Supply {
public:
	Water(StudentWorld* gw, int x, int y) :
		Supply(gw, IID_WATER_POOL, true, x, y)
	{
		this->countdown = maxCount();
	}
	void receiveSupply();
	~Water() { kill(); }
};

class Gold : public Goody {
	bool isPermanent = true;
public:
	Gold(StudentWorld* gw, int x, int y) :
		Goody(gw, IID_GOLD, false, x, y) {}
	void doSomething();
	~Gold() { kill(); }
	void setLost();
	int maxCount() { return 100; }
};

class Barrel : public Goody {
public:
	Barrel(StudentWorld* gw, int x, int y):
		Goody(gw, IID_BARREL, false, x, y) {}
	void doSomething();
	~Barrel() { kill(); }
};
class Iceman : public Actor {
private:
	int number_barrel;
	int number_squirt;
	int number_sonar;
	int number_gold;
	int hit_point;
public:
	Iceman(StudentWorld* gw, int x = 30, int y = 60) :
		Actor(gw, IID_PLAYER, true, x, y, right, 1, 0),
		number_barrel(0),
		number_squirt(5),
		number_sonar(1), 
		number_gold(0),
		hit_point(10) {}
	virtual void doSomething();
	~Iceman() { kill(); }
	int gold();
	int sonar();
	int squirt();
	int health();
	int barrel();
	void addGold();
	void addSonar();
	void addSquirt();
	void addBarrel();
	void removeTile(int x, int y);
	bool passable(int x, int y);
	void kill();
	void annoyed(int hit);
};

class Protester : public Actor {
private:
	int ticksToWaitBetweenMoves;
	int numSquaresToMoveInCurrentDirection;
	int ticksSinceLastShout;
	int ticksSinceLastTurn;
	int hit_point;
	bool leaving;
	bool thinking;
	std::queue<std::pair<int, int>> path;
	std::future<std::queue<std::pair<int, int>>> fut;
	/* */
public:
	/* */
	Protester(StudentWorld* gw, int x, int y, const int IID= IID_PROTESTER, int hit_point = 5) :
		Actor(gw, IID, true, x, y, left, 1.0, 0) {
		this->ticksToWaitBetweenMoves = maxWait();
		this->numSquaresToMoveInCurrentDirection = 10;
		this->leaving = false;
		this->thinking = false;
		this->hit_point = hit_point;
		this->ticksSinceLastShout = 15;
		this->ticksSinceLastTurn = 200;
	}
	~Protester() { kill(); }
	void doSomething();
	int maxWait();
	int health();
	void defeat();
	void shout();
	void beannoyed(int hit);
	bool isLeaving();
	virtual void bebribed();
	virtual void bekilled();
	void bestunned();
	void wait(int tick);
	void move(Direction dir, int x, int y);
};
class HardcoreProtester : public Protester {
private:
	/* */
public:
	/* */
	HardcoreProtester(StudentWorld* gw, int x, int y) :
		Protester(gw, x, y, IID_HARD_CORE_PROTESTER, 20) {}
	~HardcoreProtester() { kill(); }
	void doSomething();
	void bebribed();
	void bekilled();
};
class Squirt : public Actor {
private:
	int can_travel;
public:
	Squirt(StudentWorld* gw, int x, int y, Direction dir) :
		Actor(gw, IID_WATER_SPURT, false, x, y, dir, 1.0, 1),
		can_travel(5) {

		if (passable(this->x(), this->y()))
			setVisible(true);
		else
			kill();
	}
	~Squirt() {}
	void doSomething();
};

/*A**/
struct AStarNode {
	int y;
	int x;
	int parentX;
	int parentY;
	double g;
	double h;
	double f;
};
inline bool operator < (const AStarNode& lhs, const AStarNode& rhs)
{//We need to overload "<" to put our struct into a set
	if (lhs.y < rhs.y)
		return true;
	else if (lhs.y > rhs.y)
		return false;
	else
		return lhs.x < rhs.x;
}
template <int ROW, int COL, typename T>
class AStar {
private:
	bool closedList[ROW][COL];
	AStarNode map[ROW][COL];
public:
	AStar() {
		for (int x = 0; x < ROW; x++) {
			for (int y = 0; y < COL; y++) {
				closedList[x][y] = false;
				map[x][y].f = FLT_MAX;
				map[x][y].g = FLT_MAX;
				map[x][y].h = FLT_MAX;
				map[x][y].parentX = -1;
				map[x][y].parentY = -1;
				map[x][y].x = x;
				map[x][y].y = y;
			}
		}
	}
	double calculateH(int x, int y, int tx, int ty) {
		double H = (sqrt((x - tx) * (x - tx)
			+ (y - ty) * (y - ty)));
		return H;
	}
	std::queue<std::pair<int, int>> makePath(std::pair<int, int> dest) {
		int grid[ROW][COL];
		memset(grid, 0, sizeof(grid));
		try {
			int x = dest.first;
			int y = dest.second;
			std::stack<AStarNode> path;
			std::queue<std::pair<int, int>> usablePath;

			while (!(map[x][y].parentX == x && map[x][y].parentY == y)
				&& map[x][y].x != -1 && map[x][y].y != -1)
			{
				path.push(map[x][y]);
				int tempX = map[x][y].parentX;
				int tempY = map[x][y].parentY;
				x = tempX;
				y = tempY;

			}
			path.push(map[x][y]);

			while (!path.empty()) {
				AStarNode top = path.top();
				path.pop();
				grid[top.x][top.y] = 1;
				usablePath.push(make_pair(top.x, top.y));
			}
			/*
			for (int x = 0; x < ROW; x++) {
				for (int y = 0; y < COL; y++) {
					std::cout << grid[x][y] << " ";
				}
				std::cout << std::endl;
			}
			*/
			return usablePath;
		}
		catch (const exception & e) {
			std::cout << e.what() << std::endl;
		}
	}
	std::queue<std::pair<int, int>> search(
		std::pair<int, int> alpha, 
		std::pair<int, int> omega, 
		T grid[ROW][COL],
		std::function<bool(int, int)> Callback) {
		int x0 = alpha.first;  //original x value
		int y0 = alpha.second; //original y value
		int tx = omega.first;
		int ty = omega.second;
		map[x0][y0].f = 0.0;
		map[x0][y0].g = 0.0;
		map[x0][y0].h = 0.0;
		map[x0][y0].parentX = x0;
		map[x0][y0].parentY = y0;
		set<AStarNode> openList;
		openList.emplace(map[x0][y0]);
		while (!openList.empty()) {
			AStarNode node;
			double temp = FLT_MAX;
			std::set<AStarNode>::iterator itNode = openList.end();
			for (std::set<AStarNode>::iterator it = openList.begin();
				it != openList.end(); it = next(it)) {
				AStarNode n = *it;
				if (n.f < temp) {
					temp = n.f;
					itNode = it;
				}
			}
			node = *itNode;
			openList.erase(node);

			x0 = node.x;
			y0 = node.y;
			closedList[x0][y0] = true;
			for (int dx = -1; dx <= 1; dx++) {
				for (int dy = -1; dy <= 1; dy++) {
					if ((dx == 1 && dy == 1) || (dx == 1 && dy == -1) || (dx == -1 && dy == 1) || (dx == -1 && dy == -1)) {
						continue;
					}
					if (x0 + dx < 0 || y0 + dy < 0 || x0 + dx >= COL || y0 + dy >= ROW) {
						continue;
					}
					if (x0 + dx == tx && y0 + dy == ty) {
						map[x0 + dx][y0 + dy].parentX = x0;
						map[x0 + dx][y0 + dy].parentY = y0;
						return makePath(omega);
					}
					if (closedList[x0 + dx][y0 + dy] == false) {
						double g = FLT_MAX;
						double h = FLT_MAX;
						double f = FLT_MAX;
						if (Callback(x0 + dx, y0 + dy)) { //passable()
							g = node.g + 1.0;
							h = calculateH(x0 + dx, y0 + dy, tx, ty);
							f = g + h;
						}
						if (map[x0 + dx][y0 + dy].f == FLT_MAX ||
							map[x0 + dx][y0 + dy].f > f) {
							map[x0 + dx][y0 + dy].f = f;
							map[x0 + dx][y0 + dy].g = g;
							map[x0 + dx][y0 + dy].h = h;
							map[x0 + dx][y0 + dy].parentX = x0;
							map[x0 + dx][y0 + dy].parentY = y0;
							openList.emplace(map[x0 + dx][y0 + dy]);
							for (set<AStarNode>::iterator it = openList.begin();
								it != openList.end(); it = next(it)) {
								AStarNode n = *it;
								if (n.f < temp) {
									temp = n.f;
									itNode = it;
								}
							}
						}
					}
				}
			}
		}
		return makePath(omega);
	}
};

template <int ROW, int COL, typename T>
std::queue<std::pair<int, int>> aStarSearch(
	T grid[ROW][COL], 
	std::pair<int, int> src, 
	std::pair<int, int> dest,
	std::function<bool(int, int)> callback) {
	auto a = AStar<ROW, COL, T>();
	return a.search(src, dest, grid, callback);
}
inline double distance_square(Actor* o, Actor* t) {
	return std::pow(o->x() - t->x(), 2) + std::pow(o->y() - t->y(), 2);
}
#endif // ACTOR_H_