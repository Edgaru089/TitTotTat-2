#pragma once

#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "basicObject.hpp"
#include "bullet.hpp"
#include "uuid.hpp"

using namespace std;
using namespace sf;


class Tower :public BasicObject {
public:
	static const int attackRange = 300;
	static const int stateStatic = 0, stateTurnClockwise = 1, stateTurnCounterClockwise = 2, stateAttack = 3;
	static const int statePeriod = 500;
	static const int maxHealth = 2000;

	static const double getRotationBetweenObjects(Vector2f center, Vector2f pos) {
		double deg = atan((pos.y - center.y) / (pos.x - center.x)) * 180.0 / 3.14159;
		if (pos.x < center.x)
			deg += 180.0;
		return deg;
	}

	Tower() { sizeX = sizeY = 50.0; }
	void preLaunchInit() {
		if (!isTowerTextureLoaded) {
			towerStandaloneBase.loadFromImage(shipPng, IntRect(111, 1, 26, 26));
			towerStandaloneTop.loadFromImage(shipPng, IntRect(141, 0, 17, 30));
			isTowerTextureLoaded = true;
		}
	}
	void launch(double posX, double posY) {
		alive = true;
		offX = posX;
		offY = posY;
		cout << "Tower initalaized on position (" << posX << ", " << posY << ")." << endl;
		periodChangeTimer.restart();
		fireTimer.restart();
		health = maxHealth;
	}
	void updateLogic(RenderWindow& win) {
		isFirst = false;
		if (periodChangeTimer.getElapsedTime().asMilliseconds() >= statePeriod) {
			state = max(rand() % 6 - 3, 0);
			double minDist = attackRange, temp;
			inRangeTarget = NULL;
			for (int i = 0; i < shipHullAliveCount; i++) {
				if (shipHull[i].isAlive() && (temp = getDis(offX, offY, shipHull[i].getPosition().x, shipHull[i].getPosition().y)) < minDist) {
					minDist = temp;
					inRangeTarget = &(shipHull[i]);
				}
			}
			if (inRangeTarget != NULL) {
				state = stateAttack;
			}
			periodChangeTimer.restart();
		}
		switch (state)
		{
		case stateTurnClockwise:
			rot += 2;
			break;
		case stateTurnCounterClockwise:
			rot -= 2;
			break;
		case stateAttack:
			rot = getRotationBetweenObjects(this->getPosition(), inRangeTarget->getPosition());
			if (fireTimer.getElapsedTime() > milliseconds(800)) {
				fireTimer.restart();
				shootSmall(Vector2d(offX + 20.0*cos(rot*PI / 180.0), offY + 20.0*sin(rot*PI / 180.0)), rot);
			}
			break;
		default:
			break;
		}
		rot = (rot + 360) % 360;
	}
	void onRender(RenderWindow& win) {
		Sprite sp(towerStandaloneBase);
		sp.setOrigin(towerStandaloneBase.getSize().x / 2.0f, towerStandaloneBase.getSize().y / 2.0f);
		sp.setPosition(offX, offY);
		sp.setScale(2.0f, 2.0f);
		win.draw(sp);
		sp.setTexture(towerStandaloneTop);
		sp.setOrigin(8.5f, 7.5f);
		sp.setRotation(rot + 270);
		win.draw(sp);
	}

	const FloatRect getHitbox() { return FloatRect(offX - sizeX / 2.0f, offY - sizeY / 2.0f, sizeX, sizeY); }
	void harm(int amount) {
		health -= amount;
		particleManager.emit(Vector2d(offX, offY), Color(0, 0, 255), 5, 2.5);
		particleManager.emit(Vector2d(offX, offY), Color(255, 32, 0), 10, 3.0);
		if (health <= 0) {
			health = 0;
			kill();
		}
	}
	void kill() {
		alive = false;
		particleManager.emit(Vector2d(offX, offY), Color(64, 64, 255), 100, 3.0);
		particleManager.emit(Vector2d(offX, offY), Color(255, 0, 0), 50, 6.0);
	}
	const bool isAlive() { return alive; }
	void setSize(int width, int height) { sizeX = width; sizeY = height; }
	Vector2f getSize() { return Vector2f(sizeX, sizeY); }
	Vector2f getPosition() { return Vector2f(offX, offY); }
	int getRotation() { return rot; }
	bool isRecentlyAdded() { return isFirst; }

private:
	Clock periodChangeTimer, fireTimer;
	int state;
	ShipHull* inRangeTarget;
	double health;

	double sizeX, sizeY;
	double offX, offY;
	int rot;
	bool alive;
	bool isFirst;
};

class TowerList {
public:
	list<Tower> tower;
	bool addTower(Vector2d position) {
		cout << "Trying to add a TOWER." << endl;
		for (Tower& i : tower) {
			if (FloatRect(position.x - 50 / 2.0f, position.y - 50 / 2.0f, 50, 50).intersects(i.getHitbox()))
				return false;
		}
		Tower tmp;
		tmp.preLaunchInit();
		tmp.launch(position.x, position.y);
		tower.push_back(tmp);
		return true;
	}
	void updateLogic(RenderWindow& win) {
		for (list<Tower>::iterator i = tower.begin(); i != tower.end();) {
			if (!i->isAlive()) {
				i = tower.erase(i);
				cout << "Erasing Dead Tower." << endl;
			}
			else {
				i->updateLogic(win);
				i++;
			}
		}
	}

	void onRender(RenderWindow& win) {
		for (list<Tower>::iterator i = tower.begin(); i != tower.end();) {
			if (!i->isAlive()) {
				i = tower.erase(i);
				cout << "Erasing Dead Tower." << endl;
			}
			else {
				i->onRender(win);
				i++;
			}
		}
	}

	void clear() {
		tower.clear();
	}
};

TowerList towerList;
