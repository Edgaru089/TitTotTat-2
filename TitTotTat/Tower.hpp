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

class Tower :public BasicObject {
public:
	static const int stateStatic = 0, stateTurnClockwise = 1, stateTurnCounterClockwise = 2, stateAttack = 3;
	static const int statePeriod = 500;

	static Texture towerBase, towerTop;
	static bool isLoaded;

	Tower(){}
	void launch(RenderWindow& win) {
		alive = true;
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

	}

private:
	Clock periodChangeTimer;
	int state;

	double sizeX, sizeY;
	double offX, offY;
	int rot;
	bool alive;
};
