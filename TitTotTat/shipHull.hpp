#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "main.hpp"
#include "basicObject.hpp"
#include "ship.hpp"

using namespace std;
using namespace sf;

class ShipHull :public BasicObject
{
public:
	ShipHull() :BasicObject(), sizeX(36.0f), sizeY(36.0f) { }
	ShipHull(float posX, float posY, int rotation, string name, bool isDocked, bool hasFire, bool isDead) :
		BasicObject(), sizeX(36.0f), sizeY(36.0f), offX(posX), offY(posY), rot(rotation), isDocked(isDocked), hasFire(hasFire), isDead(isDead) { }

	void updateLogic(RenderWindow& win) {

	}

	void onRender(RenderWindow& win) {
		if (!isDead) {
			//Ship::drawShip(win, offX, offY, rot, name, isDocked, false);
			if (isDocked) //Draw ship
			{
				stowerbase.setPosition(offX, offY);
				win.draw(stowerbase);
				stowertop.setPosition(offX, offY);
				stowertop.setRotation(rot + 270);
				win.draw(stowertop);
			}
			else
			{
				sship.setPosition(offX, offY);
				sship.setRotation(rot + 270);
				win.draw(sship);
			}
			NFont::Text text(offX, offY, name, Color::White, true, 1, 1, 1); //Draw name
			FloatRect rect = fon.getBoundingRect(text);
			text.x = offX - rect.width / 2;
			text.y = offY - 55;
			fon.displayText(text, win);
		}
	}

	virtual const FloatRect getHitbox() { return FloatRect(offX - sizeX / 2.0f, offY - sizeY / 2.0f, sizeX, sizeY); }
	virtual Vector2f getSize() { return Vector2f(sizeX, sizeY); }
	virtual Vector2f getPosition() { return Vector2f(offX, offY); }

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
