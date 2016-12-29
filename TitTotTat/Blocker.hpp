#pragma once

#include <SFML/Graphics.hpp>
#include "main.hpp"
#include "basicObject.hpp"

using namespace sf;

class Blocker :public BasicObject {
public:
	static const int maxHealth = 1200;
	static const int width = 64, height = 64;

	void preLaunchInit() {
		sizeX = width;
		sizeY = height;
		rect.setSize(Vector2f(sizeX - 15, sizeY - 15));
		rect.setFillColor(Color(192, 192, 192));
		rect.setOutlineThickness(15);
		rect.setOutlineColor(Color(128, 128, 128));
		rect.setOrigin(sizeX / 2.0, sizeY / 2.0);
		healthBase.setSize(Vector2f(50.0f, 3.0f));
		healthBase.setFillColor(Color(64, 64, 64));
		healthBase.setOrigin(25.0f, 1.5f);
		healthTop.setSize(Vector2f(50.0f, 3.0f));
		healthTop.setFillColor(Color(255, 0, 0));
		healthTop.setOrigin(25.0f, 1.5f);
	}

	void launch(Vector2d pos) {
		offX = pos.x;
		offY = pos.y;
		alive = true;
	}

	void update() {
		Vector2f pos = Vector2f(Mouse::getPosition(win).x + offX - win.getView().getSize().x / 2, Mouse::getPosition(win).y + offY - win.getView().getSize().y / 2);
		RectangleShape rect;
		CircleShape csh;
		rect.setFillColor(Color(128, 128, 128));
		rect.setPosition(offX, offY);
		rect.setSize(Vector2f(getDis(offX, offY, pos.x, pos.y), 1));
		csh.setRadius(getDis(offX, offY, pos.x, pos.y));
		csh.setOrigin(getDis(offX, offY, pos.x, pos.y), getDis(offX, offY, pos.x, pos.y));
		csh.setPosition(offX, offY);
		csh.setOutlineThickness(1.0f);
		csh.setOutlineColor(Color(128, 128, 128));
		csh.setFillColor(Color::Transparent);
		csh.setPointCount(100);
		double deg = atan((pos.y - offY) / (pos.x - offX)) * 180.0 / 3.14159;
		if (pos.x < offX)
			deg += 180.0;
		deg += 360.0;
		if (deg > 360.0)
			deg -= 360.0f;
		rect.setRotation(deg);
		win.draw(rect);
		win.draw(csh);
	}

	void updateLogic(RenderWindow& win) {
		
	}

	void onRender(RenderWindow& win) {
		healthTop.setSize(Vector2f(50.0*(double)health / (double)maxHealth, 3.0f));
		win.draw(rect);
		win.draw(healthBase);
		win.draw(healthTop);
	}

	void harm(int amount) {
		if (health > amount)
			health -= amount;
		else {
			health = 0;
			kill();
		}
	}

	void kill() { alive = false; }
	const bool isAlive() { return alive; }

	Vector2f getSize() { return Vector2f(sizeX, sizeY); }
	Vector2f getPosition() { return Vector2f(offX, offY); }
	Vector2d getPositionDouble() { return Vector2d(offX, offY); }

private:
	bool alive;
	double offX, offY;
	double sizeX, sizeY;
	int rot;
	int health;
	RectangleShape rect, healthBase, healthTop;
};
