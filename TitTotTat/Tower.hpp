#pragma once

#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "basicObject.hpp"
#include "bullet.hpp"
#include "uuid.hpp"
#include "ship.hpp"

using namespace std;
using namespace sf;

Texture towerStandaloneBase, towerStandaloneTop;
bool isTowerTextureLoaded;

class Tower :public BasicObject {
public:
	static const int stateStatic = 0, stateTurnClockwise = 1, stateTurnCounterClockwise = 2, stateAttack = 3;
	static const int statePeriod = 500;

	Tower() { sizeX = sizeY = 26.0; }
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
	}
	void updateLogic(RenderWindow& win) {
		if (periodChangeTimer.getElapsedTime().asMilliseconds() >= statePeriod) {
			state = max(rand() % 6 - 3, 0);
			//if(getDis())
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
	void harm(int) {}
	void kill() { alive = false; }
	const bool isAlive() { return alive; }
	void setSize(int width, int height) { sizeX = width; sizeY = height; }
	Vector2f getSize() { return Vector2f(sizeX, sizeY); }
	Vector2f getPosition() { return Vector2f(offX, offY); }
	int getRotation() { return rot; }

private:
	Clock periodChangeTimer;
	int state;

	double sizeX, sizeY;
	double offX, offY;
	int rot;
	bool alive;
};

class TowerList {
public:
	list<Tower> tower;
	void addTower(Vector2d position) {
		Tower tmp;
		cout << "Trying to add a TOWER." << endl;
		tmp.preLaunchInit();
		tmp.launch(position.x, position.y);
		tower.push_back(tmp);
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
