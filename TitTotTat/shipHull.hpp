#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "main.hpp"
#include "basicObject.hpp"

using namespace std;
using namespace sf;

class ShipHull :public BasicObject
{
public:
	ShipHull() :BasicObject(), sizeX(50.0f), sizeY(50.0f) { }
	ShipHull(float posX, float posY, int rotation, string name, bool isDocked, bool hasFire, bool isDead) :
		BasicObject(), sizeX(50.0f), sizeY(50.0f), offX(posX), offY(posY), rot(rotation), isDocked(isDocked), hasFire(hasFire), isDead(isDead) { }

	void updateLogic(RenderWindow& win) {

	}

	void onRender(RenderWindow& win) {
		if(!isDead)
			Ship::drawShip(win, offX, offY, rot, name, isDocked, false);
	}

	virtual const FloatRect getHitbox() { return FloatRect(offX - sizeX / 2.0f, offY - sizeY / 2.0f, sizeX, sizeY); }

	const bool isAlive() {
		return !isDead;
	}

private:
	float sizeX, sizeY;
	float offX, offY;
	int rot;
	string name;
	bool isDocked, hasFire, isDead;
};


ShipHull shipHull[100];
const int shipHullCount = 100;
int shipHullAliveCount = 0;
