#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include "main.hpp"
#include "uuid.hpp"

using namespace std;
using namespace sf;

class BasicObject
{
public:
	BasicObject() :id(Uuid::getUuid()), alive(false) {}
	~BasicObject() {}
	virtual void preLaunchInit() {};
	virtual void launch() { alive = true; }
	virtual void launch(RenderWindow& win) { alive = true; }
	virtual void updateLogic(RenderWindow&) = 0;
	virtual void onRender(RenderWindow&) = 0;
	virtual const bool isAlive() { return alive; }
	virtual const bool operator== (BasicObject& obj) { return id == obj.id; }
	virtual const bool operator== (Uuid& obj) { return id == obj; }
	virtual Uuid getUuid() { return id; }
	virtual const FloatRect getHitbox() { return FloatRect(offX - sizeX / 2.0f, offY - sizeY / 2.0f, sizeX, sizeY); }
	virtual void harm(int) {}
	virtual void kill() { alive = false; }
	virtual void setSize(int width, int height) { sizeX = width; sizeY = height; }
	virtual Vector2f getSize() { return Vector2f(sizeX, sizeY); }
	virtual Vector2f getPosition() { return Vector2f(offX, offY); }
	virtual int getRotation() { return rot; }
private:
	double sizeX, sizeY;
	double offX, offY;
	int rot;
	Uuid id;
	bool alive;
};

bool doesObjectIntersects(BasicObject& x, BasicObject& y)
{
	return x.getHitbox().intersects(y.getHitbox());
}

void renderObjectHitbox(RenderWindow& win,BasicObject& obj) {
	FloatRect bound = obj.getHitbox();
	RectangleShape rect;
	rect.setPosition(bound.left, bound.top);
	rect.setSize(Vector2f(bound.width, bound.height));
	rect.setFillColor(Color::Transparent);
	rect.setOutlineThickness(1.5f);
	rect.setOutlineColor(Color::White);
	win.draw(rect);
}

