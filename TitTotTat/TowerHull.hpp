#pragma once

#include <SFML/Graphics.hpp>
#include "main.hpp"
#include "Tower.hpp"

using namespace sf;

class TowerHull :public BasicObject {
public:
	TowerHull() { sizeX = sizeY = 50.0; }
	TowerHull(Vector2d pos, int rot) { sizeX = sizeY = 50.0; offX = pos.x; offY = pos.y; this->rot = rot; }

	void updateLogic(RenderWindow&) {}
	void onRender(RenderWindow& win) {
		if (!isTowerTextureLoaded) {
			towerStandaloneBase.loadFromImage(shipPng, IntRect(111, 1, 26, 26));
			towerStandaloneTop.loadFromImage(shipPng, IntRect(141, 0, 17, 30));
			isTowerTextureLoaded = true;
		}
		Sprite sp(towerStandaloneBase);
		sp.setOrigin(towerStandaloneBase.getSize().x / 2.0f, towerStandaloneBase.getSize().y / 2.0f);
		sp.setPosition(offX, offY);
		sp.setScale(2.0f, 2.0f);/*
		CircleShape cir(Tower::attackRange, 60);
		cir.setFillColor(Color(128, 128, 128, 64));
		cir.setOutlineColor(Color(192, 192, 192, 128));
		cir.setOutlineThickness(1.5f);
		cir.setOrigin(Tower::attackRange, Tower::attackRange);
		cir.setPosition(offX, offY);
		win.draw(cir);*/
		win.draw(sp);
		sp.setTexture(towerStandaloneTop);
		sp.setOrigin(8.5f, 7.5f);
		sp.setRotation(rot + 270);
		win.draw(sp);
		renderObjectHitbox(win, *this);
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
	double sizeX, sizeY;
	double offX, offY;
	int rot;
	bool alive;
};

list<TowerHull> towerHulls;
