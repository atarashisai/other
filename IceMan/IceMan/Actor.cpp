#include "Actor.h"
#include "StudentWorld.h"
#include <assert.h>
#include <utility>
#include <set>
#include <stack>
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
inline int distance(Actor* o, Actor* t) {
	return std::pow(o->x() - t->x(), 2) + std::pow(o->y() - t->y(), 2);
}
void Gold::doSomething() {
	if (!_alive) return;
	if (isPermanent) {
		StudentWorld* world = static_cast<StudentWorld*>(this->_gw);
		Iceman* player = static_cast<Iceman*>(world->getPlayer());
		if (distance(this, player) <= 16) {
			setVisible(true);
		}
		if (distance(this, player) <= 9) {
			player->addGold();
			world->increaseScore(10);
			kill();
		}
	}
	else {
		this->countdown -= 1;
		if (this->countdown <= 0) {
			setVisible(false);
			this->_alive = false;
		}
	}
}

void Barrel::doSomething() {
	if (!_alive) return;
	StudentWorld* world = static_cast<StudentWorld*>(this->_gw);
	Iceman* player = static_cast<Iceman*>(world->getPlayer());
	if (distance(this, player) <= 16) {
		setVisible(true);
		return;
	}
	if (distance(this, player) <= 9) {
		world->playSound(SOUND_FOUND_OIL);
		player->addBarrel();
		world->increaseScore(1000);
		kill();
	}
}

void Boulder::doSomething() {
	if (this->countdown > 0) {
		this->countdown -= 1;
		return;
	}
	if (!this->rolling) {
		bool no_support = true;
		StudentWorld* world = static_cast<StudentWorld*>(this->_gw);
		for (int dx = 0; dx < 4; dx++) {
			Ice* ice = dynamic_cast<Ice*>(world->at(this->x() + dx, this->y() - 1));
			no_support &= (ice == nullptr);
			if (!no_support)
				return;
		}
		if (no_support) {
			this->rolling = true;
			this->countdown = 30;
		}
	}
	else {
		StudentWorld* world = static_cast<StudentWorld*>(this->_gw);
		if (distance(world->getPlayer(), this) <= 6) {
			world->getPlayer()->kill();
			this->kill();
			return;
		}
		for (int dx = 0; dx < 4; dx++) {
			Ice* ice = dynamic_cast<Ice*>(world->at(this->x() + dx, this->y() - 1));
			if (ice != nullptr) {
				this->kill();
				return;
			}
		}
		this->moveTo(this->x(), this->y() - 1);
	}
}

void Sonar::doSomething() {
	if (!_alive) return;
	this->countdown -= 1;
	if (this->countdown <= 0) {
		setVisible(false);
		this->_alive = false;
	}
	StudentWorld* world = static_cast<StudentWorld*>(this->_gw);
	Iceman* player = static_cast<Iceman*>(world->getPlayer());
	if (distance(this, player) <= 9) {
		world->playSound(SOUND_GOT_GOODIE);
		player->addSonar();
		world->increaseScore(75);
		kill();
	}
}

void Water::doSomething() {
	if (!_alive) return;
	this->countdown -= 1;
	if (this->countdown <= 0) {
		setVisible(false);
		this->_alive = false;
	}
	StudentWorld* world = static_cast<StudentWorld*>(this->_gw);
	Iceman* player = static_cast<Iceman*>(world->getPlayer());
	if (distance(this, player) <= 9) {
		player->addSquirt();
		world->increaseScore(100);
		kill();
	}
}

void Iceman::doSomething() {
	int ch;
	if (getWorld()->getKey(ch) == true)
	{
		// user hit a key this tick!
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			/*... move player to the left ...;*/
			setDirection(left);
			if (this->passable(this->_x-1, this->_y)) {
				this->_x -= 1;
				removeTile();
				moveTo(this->_x, this->_y);
			}
			break;
		case KEY_PRESS_RIGHT:
			/*... move player to the right ...; */
			setDirection(right);
			if (this->passable(this->_x + 1, this->_y)) {
				this->_x += 1;
				removeTile();
				moveTo(this->_x, this->_y);
			}
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);
			if (this->passable(this->_x, this->_y - 1)) {
				this->_y -= 1;
				removeTile();
				moveTo(this->_x, this->_y);
			}
			break;
		case KEY_PRESS_UP:
			setDirection(up);
			if (this->passable(this->_x, this->_y + 1)) {
				this->_y += 1;
				removeTile();
				moveTo(this->_x, this->_y);
			}
			break;
		case 'z':
		case 'Z':
			if (this->sonar() > 0) {
				this->number_sonar -= 1;
			}
		case KEY_PRESS_ESCAPE:
			this->hit_point = 0;
			break;
		case KEY_PRESS_SPACE:
			/*... add a Squirt in front of the player...; */
			if (this->squirt() > 0) {
				StudentWorld* world = static_cast<StudentWorld*>(this->_gw);
				world->dropSquirt();
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
	StudentWorld* world = static_cast<StudentWorld*>(this->_gw);
	for (int dx = 0; dx < SPRITE_WIDTH; dx++) {
		for (int dy = 0; dy < SPRITE_HEIGHT; dy++) {
			if (dynamic_cast<Boulder*>(world->at(x + dx, y + dy)) != nullptr) {
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
void Iceman::addGold() {
	this->number_gold += 1;
}
void Iceman::addSonar() {
	this->number_sonar += 1;
}
void Iceman::addSquirt() {
	this->number_squirt += 5;
}
void Iceman::addBarrel() {
	this->number_barrel += 1;
}
void Iceman::removeTile() const {

	StudentWorld* world = static_cast<StudentWorld*>(this->_gw);
	for (int deltaX = 0; deltaX < SPRITE_WIDTH; deltaX++) {
		for (int deltaY = 0; deltaY < SPRITE_HEIGHT; deltaY++) {
			Ice* tile = dynamic_cast<Ice*>(world->at(this->_x + deltaX, this->_y + deltaY));
			if (tile != nullptr) {
				world->removeIce(tile);
			}
		}
	}
}
void Protester::doSomething() {
	if (!this->_alive)
		return;
	if (this->ticksToWaitBetweenMoves == 0) {
		if (this->step > 0)
			this->step -= 1;
		this->ticksToWaitBetweenMoves = std::max(unsigned int(0), 3 - this->_gw->getLevel() / 4);

		StudentWorld* world = static_cast<StudentWorld*>(this->_gw);
		std::set<Direction> possible_move;
		if (this->passable(this->_x + 1, this->_y))
			possible_move.emplace(right);
		if (this->passable(this->_x - 1, this->_y))
			possible_move.emplace(left);
		if (this->passable(this->_x, this->_y + 1))
			possible_move.emplace(up);
		if (this->passable(this->_x, this->_y - 1))
			possible_move.emplace(down);


		Direction dir = getDirection();
		bool can_move_forward = std::find(possible_move.begin(), possible_move.end(), dir) != possible_move.end();
		if (!can_move_forward) {
			std::set<Direction>::iterator it = possible_move.begin();
			for (int i = world->rand(0, possible_move.size() - 1); i > 0; i--) {
				it++;
			}
			dir = *(it);
		}
		else if (possible_move.size() > 2) {
			if (world->rand(0, 100) > 30 && this->step == 0) {
				possible_move.erase(dir);
				std::set<Direction>::iterator it = possible_move.begin();
				for (int i = world->rand(0, possible_move.size() - 1);i > 0;i--) {
					it++;
				}
				dir = *(it);
				this->step = 5 + world->rand(0, 20);
			}
		}

		switch (dir)
		{
		case right:
			setDirection(right);
			this->_x += 1;
			moveTo(this->_x, this->_y);
			break;
		case left:
			setDirection(left);
			this->_x -= 1;
			moveTo(this->_x, this->_y);
			break;
		case up:
			setDirection(up);
			this->_y += 1;
			moveTo(this->_x, this->_y);
			break;
		case down:
			setDirection(down);
			this->_y -= 1;
			moveTo(this->_x, this->_y);
			break;
		default:
			break;
		}
	}
	else {
		this->ticksToWaitBetweenMoves -= 1;
	}
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
void HardcoreProtester::doSomething() {
	Protester::doSomething();
}
void Squirt::doSomething() {
	if (!_alive)
		return;
	StudentWorld* world = static_cast<StudentWorld*>(this->_gw);
	Actor* tile = world->at(this->x(), this->y());
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
template <int ROW, int COL>
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
	std::vector<AStarNode> makePath(std::pair<int, int> dest) {
		int grid[ROW][COL] =
		{
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		};
		try {
			cout << "Found a path" << endl;
			int x = dest.first;
			int y = dest.second;
			std::stack<AStarNode> path;
			std::vector<AStarNode> usablePath;

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
				//cout << top.x << " " << top.y << endl;
				grid[top.x][top.y] = 1;
				usablePath.emplace_back(top);
			}
			for (int x = 0; x < ROW; x++) {
				for (int y = 0; y < COL; y++) {
					std::cout << grid[x][y] << " ";
				}
				std::cout << std::endl;
			}
			return usablePath;
		}
		catch (const exception & e) {
			std::cout << e.what() << std::endl;
		}
	}
	std::vector<AStarNode> search(std::pair<int, int> alpha, std::pair<int, int> omega, int grid[ROW][COL]) {
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
			cout << "p" << (*itNode).x << " " << (*itNode).y << " f " << (*itNode).f << endl;
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
						if (grid[x0 + dx][y0 + dy] == 1) {
							g = node.g + 1.0;
							h = calculateH(x0 + dx, y0 + dy, tx, ty);
							f = g + h;
						}
						if (map[x0 + dx][y0 + dy].f == FLT_MAX ||
							map[x0 + dx][y0 + dy].f > f) {
							cout << "__" << x0 + dx << " " << y0 + dy << " f" << f << endl;
							map[x0 + dx][y0 + dy].f = f;
							map[x0 + dx][y0 + dy].g = g;
							map[x0 + dx][y0 + dy].h = h;
							map[x0 + dx][y0 + dy].parentX = x0;
							map[x0 + dx][y0 + dy].parentY = y0;
							openList.emplace(map[x0 + dx][y0 + dy]);
							for (set<AStarNode>::iterator it = openList.begin();
								it != openList.end(); it = next(it)) {
								AStarNode n = *it;
								cout << ":" << (*it).x << " " << (*it).y << " f " << (*it).f << endl;
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

template <int ROW, int COL>
void aStarSearch(int grid[ROW][COL], std::pair<int, int> src, std::pair<int, int> dest) {
	auto a = AStar<ROW, COL>();
	a.search(src, dest, grid);
}