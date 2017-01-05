#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "main.hpp"
#include "basicObject.hpp"
#include "bullet.hpp"

using namespace std;
using namespace sf;

class BulletHull :public BasicObject {
public:
	BulletHull() :BasicObject(), sizeX(6.0f), sizeY(6.0f) { alive = false; }
	BulletHull(float posX, float posY, int rot, int size) :BasicObject(),
		sizeX(6.0f), sizeY(6.0f), offX(posX), offY(posY), rot(rot),
		size(size) {
		alive = true;
	}

	void updateLogic(RenderWindow& win) {

	}

	void onRender(RenderWindow& win) {
		Bullet::drawBullet(win, offX, offY, size, rot);
	}

	virtual const FloatRect getHitbox() { return FloatRect(offX - sizeX / 2.0f, offY - sizeY / 2.0f, sizeX, sizeY); }
	int getBulletSize() { return size; }

private:
	bool alive;
	float sizeX, sizeY;
	float offX, offY;
	int rot;
	int size; //Refer to "Bullet.hpp" Bullet::Small, Bullet::Large;
};

BulletHull bulletHull[2000];
const int bulletHullCount = 100;
int bulletHullAliveCount = 0;
