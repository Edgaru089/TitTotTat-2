#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "main.hpp"
#include "basicObject.hpp"
#include "Particle.hpp"
#include "uuid.hpp"

using namespace sf;

static const double bulletStepAclc = 15.0, bulletSlowAclc = 11.0;
const int bulletLargeHarm = 800, bulletSmallHarm = 400;

class Bullet :public BasicObject
{
public:

	static const int Small = 0, Large = 1;
	double sizeX, sizeY;
	int size;

	Bullet() :BasicObject(), sizeX(6.0f), sizeY(6.0f) { alive = false; }

	void launch(double posX, double posY, int Rot, int Size, Uuid Owner)
	{
		offX = posX;
		offY = posY;
		rot = Rot;
		size = Size;
		killTimer = -1;
		if (size == Small)
			rect.setSize(Vector2f(1.5, 12));
		else
			rect.setSize(Vector2f(3, 8));
		rect.setFillColor(Color::White);
		rect.setOrigin(rect.getSize().x / 2.0, rect.getSize().y / 2.0);
		rect.setRotation(rot + 270);
		owner = Owner;
		aclcTimer.restart();
		alive = true;
	}

	void updateLogic(RenderWindow& win)
	{
		if (!alive)
			return;
		Time aclcTime = aclcTimer.restart();
		double aclcMultply = aclcTime.asMilliseconds() / 20.0;
		offX += (size == Small ? bulletStepAclc : bulletSlowAclc)*cos(PI*rot / 180.0f)*aclcMultply;
		offY += (size == Small ? bulletStepAclc : bulletSlowAclc)*sin(PI*rot / 180.0f)*aclcMultply;
		if (offX<0 || offX>arenaWidth || offY<0 || offY>arenaHeight)
			kill();
	}

	void updateKillTimer() {
		if (killTimer > 0)
			killTimer--;
		if (killTimer == 0) {
			kill();
			killTimer = -1;
		}
	}

	static void drawBullet(RenderWindow& win, float offX, float offY, int size, int rot)
	{
		RectangleShape rect;
		if (size == Small)
			rect.setSize(Vector2f(1.5, 12));
		else
			rect.setSize(Vector2f(3, 8));
		rect.setFillColor(Color::White);
		rect.setOrigin(rect.getSize().x / 2.0, rect.getSize().y / 2.0);
		rect.setRotation(rot + 270);
		rect.setPosition(offX, offY);
		win.draw(rect);
	}

	void onRender(RenderWindow& win)
	{
		if (!alive)
			return;
		rect.setPosition(offX, offY);
		win.draw(rect);
	}

	const bool isAlive()
	{
		return alive;
	}

	const bool isHit(BasicObject& target)
	{
		//if (target == owner)
			//return false;
		//else
			return doesObjectIntersects(*this, target);
	}

	void kill()
	{
		if(size==Bullet::Large)
			particleManager.emit(Vector2d(offX, offY), Color(255, 255, 255), 20, 1.5, 0, 360.0);
		else
			particleManager.emit(Vector2d(offX, offY), Color(255, 255, 255), 8, 0.8, 0, 360.0);
		alive = false;
	}

	void killIn(int rounds = 10) {
		killTimer = rounds;
	}

	Vector2f getPosition() { return Vector2f(offX, offY); }
	int getRotation() { return rot; }
	int getBulletSize() { return size; }
	virtual const FloatRect getHitbox() { return FloatRect(offX - sizeX / 2.0f, offY - sizeY / 2.0f, sizeX, sizeY); }

private:
	double offX, offY;
	int rot;
	int killTimer;
	bool alive;
	Clock aclcTimer;
	RectangleShape rect;
	Uuid owner;
};

Bullet bullet[2000];
const int bulletCount = 800;
int bulletAliveCount;

void updateAliveBulletCount()
{
	for (int i = 0; i < bulletCount; i++)
		if (bullet[i].isAlive())
			bulletAliveCount++;
}

void shootSmall(Vector2d posD,int rot)
{
	int tmp = 0;
	while (bullet[tmp].isAlive())
		tmp++;
	bullet[tmp].launch(posD.x + 20.0*cos(rot*PI / 180.0), posD.y + 20.0*sin(rot*PI / 180.0), rot, Bullet::Small, Uuid::getUuid());
	particleManager.emit(Vector2d(posD.x + 20.0*cos(rot*PI / 180.0), posD.y + 20.0*sin(rot*PI / 180.0)), Color(192, 192, 192), 2, 2.0, rot, 15.0);
}
void shootSmall(Vector2d pos, int rot, double speed) { shootSmall(pos, rot); }

void shootLarge(Vector2d posD,  int rot) {
	int tmp = 0;
	while (bullet[tmp].isAlive())
		tmp++;
	bullet[tmp].launch(posD.x + 20.0*cos(rot*PI / 180.0), posD.y + 20.0*sin(rot*PI / 180.0), rot, Bullet::Large, Uuid::getUuid());
	particleManager.emit(Vector2d(posD.x + 20.0*cos(rot*PI / 180.0), posD.y + 20.0*sin(rot*PI / 180.0)), Color(192, 192, 192),
		10, 2.0, rot, 30.0);
}
void shootLarge(Vector2d pos, int rot, double speed) { shootLarge(pos, rot); }
