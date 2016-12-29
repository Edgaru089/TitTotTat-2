#pragma once

#include "main.hpp"
#include "basicObject.hpp"
#include "bullet.hpp"
#include "bulletHull.hpp"
#include "ship.hpp"
#include "shipHull.hpp"

class ObjectCollisonManager : public BasicObject {
public:
	ObjectCollisonManager() :BasicObject() {}
	void preLaunchInit() {

	}
	void launch() {

	}
	void updateLogic(RenderWindow& win) {
		logicDataLock.lock();
		for (int i = 0; i < bulletHullAliveCount; i++) {
			if (doesObjectIntersects(bulletHull[i], ship))
				ship.harm(bulletHull[i].getBulletSize() == Bullet::Large ? bulletLargeHarm : bulletSmallHarm);
		}
		for (int i = 0; i < bulletCount; i++) {
			if (bullet[i].isAlive()) {
				bullet[i].updateKillTimer();
				for (int j = 0; j < shipHullAliveCount; j++)
					if (doesObjectIntersects(bullet[i], shipHull[j]) && shipHull[j].isAlive())
						bullet[i].killIn(0);
			}
		}
		logicDataLock.unlock();
	}
	void onRender(RenderWindow&) {

	}
};

ObjectCollisonManager objectCollisonManager;
