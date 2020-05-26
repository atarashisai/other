#include "Actor.h"
#include "StudentWorld.h"
#include <assert.h>
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
void Gold::doSomething() {
	if (!_alive) return;
	if (isPermanent) {
		StudentWorld* world = static_cast<StudentWorld*>(this->_gw);
		Iceman* player = static_cast<Iceman*>(world->getPlayer());
		if (std::abs((this->x() + 1 - player->x())) < 4 && std::abs((this->y() + 1 - player->y())) < 4) {
			setVisible(true);
		}
		if (std::abs((this->x() + 1 - player->x())) < 3 && std::abs((this->y() + 1 - player->y())) < 3) {
			player->getGold();
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
	if (std::abs((this->x() + 1 - player->x())) < 4 && std::abs((this->y() + 1 - player->y())) < 4) {
		setVisible(true);
	}
	if (std::abs((this->x() + 1 - player->x())) < 3 && std::abs((this->y() + 1 - player->y())) < 3) {
		player->getBarrel();
		world->increaseScore(1000);
		kill();
	}
}
void Boulder::doSomething() {

}
void Boulder::removeIce() {
	StudentWorld* world = static_cast<StudentWorld*>(this->_gw);
	for (int deltaX = 0; deltaX < SPRITE_WIDTH; deltaX++) {
		for (int deltaY = 0; deltaY < SPRITE_HEIGHT; deltaY++) {
			Actor* tile = world
				->getTile(this->_x + deltaX, this->_y + deltaY);
			if (tile != nullptr) {
				world->removeIce(dynamic_cast<Ice*>(tile));
			}
		}
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
	if (std::abs((this->x() + 1 - player->x())) < 3 && std::abs((this->y() + 1 - player->y())) < 3) {
		player->getSonar();
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
	if (std::abs((this->x() + 1 - player->x())) < 3 && std::abs((this->y() + 1 - player->y())) < 3) {
		player->getSquirt();
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
			if (this->_x > 0) {
				this->_x -= 1;
				removeTile();
			}
			moveTo(this->_x, this->_y);
			break;
		case KEY_PRESS_RIGHT:
			/*... move player to the right ...; */
			setDirection(right);
			if (this->_x < VIEW_WIDTH - SPRITE_WIDTH) {
				this->_x += 1;
				removeTile();
			}
			moveTo(this->_x, this->_y);
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);
			if (this->_y > 0) {
				this->_y -= 1;
				removeTile();
			}
			moveTo(this->_x, this->_y);
			break;
		case KEY_PRESS_UP:
			setDirection(up);
			if (this->_y < VIEW_HEIGHT - SPRITE_HEIGHT) {
				this->_y += 1;
				removeTile();
			}
			moveTo(this->_x, this->_y);
			break;
		case KEY_PRESS_SPACE:
			/*... add a Squirt in front of the player...; */
			break;
			// etc…
		}
	}
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
void Iceman::getGold() {
	this->number_gold += 1;
}
void Iceman::getSonar() {
	this->number_sonar += 1;
}
void Iceman::getSquirt() {
	this->number_squirt += 5;
}
void Iceman::getBarrel() {
	this->number_barrel += 1;
}
void Iceman::removeTile() const {
	StudentWorld* world = static_cast<StudentWorld*>(this->_gw);
	for (int deltaX = 0; deltaX < SPRITE_WIDTH; deltaX++) {
		for (int deltaY = 0; deltaY < SPRITE_HEIGHT; deltaY++) {
			Actor* tile = world
				->getTile(this->_x + deltaX, this->_y + deltaY);
			if (tile != nullptr) {
				world->removeIce(dynamic_cast<Ice*>(tile));
			}
		}
	}
}