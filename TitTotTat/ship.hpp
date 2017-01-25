#pragma once

#include <cstdlib>
#include <cmath>
#include <string>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include "main.hpp"
#include "NFont.hpp"
#include "basicObject.hpp"
#include "Particle.hpp"
#include "bullet.hpp"
#include "shipHull.hpp"
//#include "Tower.hpp"
//#include "TowerHull.hpp"

using namespace std;
using namespace sf;

const double msp = 50.0, ac = 0.4, dc = 0.02;
int isFire;
bool isDead, isExplode;
bool isSpiltControled = false;
bool isShipResLoaded = false;
Clock deadTimer;


class Ship :public BasicObject
{
public:

	enum Fire
	{
		None = 0,
		Back = 1,
		Front = 2
	};

	static const int maxFuel = 1200, maxHealth = 1000, maxShield = 1500, maxEnergy = 1000;
	float sizeX, sizeY;

	Ship() :BasicObject(), sizeX(40.0f), sizeY(40.0f) { /*fani = exani = 0;*/ isReady = false; }

	bool loadRes()
	{
		if (isShipResLoaded)
			return true;

		if (!shipPng.loadFromFile("ship.png"))
			return false;
		shipPng.createMaskFromColor(Color::Black);

		tship.loadFromImage(shipPng, IntRect(0, 1, 25, 35));
		tTowerBase.loadFromImage(shipPng, IntRect(49, 1, 36, 36));
		tTowerTop.loadFromImage(shipPng, IntRect(88, 1, 17, 46));

		/*
		fire[0].loadFromImage(shipPng, IntRect(6, 49, 39, 40));
		fire[1].loadFromImage(shipPng, IntRect(52, 49, 39, 40));
		fire[2].loadFromImage(shipPng, IntRect(103, 49, 39, 40));
		fire[3].loadFromImage(shipPng, IntRect(153, 49, 39, 40));

		epl[0].loadFromImage(shipPng, IntRect(0, 100 - 5, 50, 50));
		epl[1].loadFromImage(shipPng, IntRect(50, 100 - 5, 50, 50));
		epl[2].loadFromImage(shipPng, IntRect(100, 100 - 5, 50, 50));
		epl[3].loadFromImage(shipPng, IntRect(150, 100 - 5, 50, 50));
		epl[4].loadFromImage(shipPng, IntRect(0, 150 - 5, 50, 50));
		epl[5].loadFromImage(shipPng, IntRect(50, 150 - 5, 50, 50));
		epl[6].loadFromImage(shipPng, IntRect(100, 150 - 5, 50, 50));
		epl[7].loadFromImage(shipPng, IntRect(150, 150 - 5, 50, 50));
		epl[8].loadFromImage(shipPng, IntRect(0, 200 - 5, 50, 50));
		epl[9].loadFromImage(shipPng, IntRect(50, 200 - 5, 50, 50));
		epl[10].loadFromImage(shipPng, IntRect(100, 200 - 5, 50, 50));
		epl[11].loadFromImage(shipPng, IntRect(150, 200 - 5, 50, 50));
		epl[12].loadFromImage(shipPng, IntRect(0, 250 - 5, 50, 50));
		epl[13].loadFromImage(shipPng, IntRect(50, 250 - 5, 50, 50));
		epl[14].loadFromImage(shipPng, IntRect(100, 250 - 5, 50, 50));
		epl[15].loadFromImage(shipPng, IntRect(150, 250 - 5, 50, 50));
		*/

		sship.setTexture(tship);
		sship.setOrigin(12.5, 19.5);
		sship.setScale(1.5, 1.5);
		stowertop.setTexture(tTowerTop);
		stowertop.setOrigin(8.5, 8.5);
		stowertop.setScale(1.5, 1.5);
		stowerbase.setTexture(tTowerBase);
		stowerbase.setOrigin(18, 18);
		stowerbase.setScale(1.5, 1.5);
		//sfire.setTexture(fire[0]);
		//sfire.setOrigin(7, 36);
		//sepl.setTexture(epl[0]);
		//sepl.setOrigin(25, 25);
		//sepl.setScale(1.5, 1.5);

		isShipResLoaded = true;
		return true;
	}

	void preLaunchInit() { loadRes(); }

	void launch(double OffX, double OffY, int Rot = 0, string Name = "NO_NAME")
	{
		offX = OffX;
		offY = OffY;
		rot = Rot;
		name = Name;
		spX = spY = 0.0;
		fuel = 8000;
		engineOff = false;
		health = 1000;
		shield = 1000;
		energy = 0;
		isDead = isExplode = dock = false;
		fireTimer = 0;
		vibrateTimer = 0;

		isReady = true;
		aclcTimer.restart();
	}

	void rotate(int angle)
	{
		rot += angle;
		while (rot < 0)
			rot += 360;
		rot %= 360;
	}

	void setPosition(double OffX, double OffY)
	{
		offX = OffX;
		offY = OffY;
	}

	void setName(string Name) { name = Name; }

	void setRotation(int rotation) { rot = rotation; }

	void drawFire(bool isBack, RenderWindow& win)
	{
		/*
		if (isBack)
			sfire.setOrigin(7, 36);
		else
			sfire.setOrigin(18, 30);
		sfire.setTexture(fire[fani]);
		sfire.setPosition(offX, offY);
		sfire.setRotation(rot + 270 + (isBack ? 0 : 180));
		win.draw(sfire);*/
		particleManager.emit(Vector2d(offX - 14.0*cos(rot*PI / 180.0), offY - 14.0*sin(rot*PI / 180.0)), Color(128, 128, 128), 1, 1.5, (rot + 180) % 360, 40.0);
	}

	void drawShip(RenderWindow& win)
	{

		if (dock)
		{
			stowerbase.setPosition(offX, offY);
			win.draw(stowerbase);
			stowertop.setPosition(offX, offY);
			stowertop.setRotation(rot + 270);
			win.draw(stowertop);
			return;
		}
		sship.setPosition(offX, offY);
		sship.setRotation(rot + 270);
		win.draw(sship);
		renderObjectHitbox(win, *this);
	}

	static void drawShip(RenderWindow& win, double offX, double offY, int rot, string name, bool isDocked, bool hasFire)
	{
		/*
		sfire.setOrigin(7, 36); //Draw fire
		sfire.setTexture(fire[fani]);
		sfire.setPosition(offX, offY);
		sfire.setRotation(rot + 270);
		if (hasFire)
			win.draw(sfire);*/
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

	void drawName(RenderWindow& win)
	{
		NFont::Text text(offX, offY, name, Color::White, true, 1, 1, 1);
		FloatRect rect = fon.getBoundingRect(text);
		text.x = offX - rect.width / 2;
		text.y = offY - 55;
		fon.displayText(text, win);
	}

	string getFormatString()
	{
		char s[1001];
		sprintf(s, "Offset: (%.2lf, %.2lf)\nSpeed: (%.5lf, %.5lf, %.5lf)\nRotation: %ddeg\nHealth/Shield/Fuel:\n%.3lf/%d, %.3lf/%d, %.3lf/%d\nZoom: X%.2f\nisSpiltControled: %s\n%s",
			offX, offY, spX, spY, sqrt(spX*spX + spY*spY), rot, health, maxHealth, shield, maxShield, fuel, maxFuel, 1 / viewZoomFactor,
			isSpiltControled ? "True" : "False", (isDead || isExplode) ? "Dead" : "");
		return string(s);
	}
	void reborderX(BasicObject& obj) {
		if (offX > obj.getPosition().x)
			offX = obj.getPosition().x + obj.getSize().x / 2.0 + sizeX / 2.0;
		else
			offX = obj.getPosition().x - obj.getSize().x / 2.0 - sizeX / 2.0;
		spX = 0.0;
	}
	void moveX(double amount) {
		offX += amount;
		for (int i = 0; i < shipHullAliveCount; i++)
			if (shipHull[i].isAlive() && doesObjectIntersects(shipHull[i], *this))
				reborderX(shipHull[i]);
		//for (Tower& i : towerList.tower)
		//	if (i.isAlive() && doesObjectIntersects(i, *this))
		//		reborderX(i);
		//for (TowerHull& i : towerHulls)
		//	if (doesObjectIntersects(i, *this))
		//		reborderX(i);
	}

	void reborderY(BasicObject& obj) {
		if (offY > obj.getPosition().y)
			offY = obj.getPosition().y + obj.getSize().y / 2.0 + sizeY / 2.0;
		else
			offY = obj.getPosition().y - obj.getSize().y / 2.0 - sizeY / 2.0;
		spY = 0.0;
	}
	void moveY(double amount) {
		offY += amount;
		for (int i = 0; i < shipHullAliveCount; i++)
			if (doesObjectIntersects(shipHull[i], *this))
				reborderY(shipHull[i]);
		//for (Tower& i : towerList.tower)
		//	if (i.isAlive() && doesObjectIntersects(i, *this))
		//		reborderY(i);
		//for (TowerHull& i : towerHulls)
		//	if (doesObjectIntersects(i, *this))
		//		reborderY(i);
	}

	void spUpdate(int aclc, double aclcMultply)
	{
		while (rot < 0)
			rot += 360;
		rot %= 360;
		if (aclc == 1)
		{
			fuel -= 12 * aclcMultply;
			spX += ac*cos(PI*rot / 180.0f)*aclcMultply;
			spY += ac*sin(PI*rot / 180.0f)*aclcMultply;
		}
		else if (aclc == -1)
		{
			fuel -= 12;
			spX += ac*cos(PI*rot / 180.0f) / -2.0*aclcMultply;
			spY += ac*sin(PI*rot / 180.0f) / -2.0*aclcMultply;
		}
		spX -= pow(dc, aclcMultply)*spX;// damping
		spY -= pow(dc, aclcMultply)*spY;

		if (spX<0.0 && spX>-0.05)
			spX = 0.0;
		else if (spX > 0.0&&spX < 0.05)
			spX = 0.0;
		if (spY<0.0 && spY>-0.05)
			spY = 0.0;
		else if (spY > 0.0&&spY < 0.05)
			spY = 0.0;

		if ((isSame(offX, 0) && spX < 0) || (isSame(offX, arenaWidth) && spX > 0)) //Collison on the four edges
			spX = 0;
		if ((isSame(offY, 0) && spY < 0) || (isSame(offY, arenaHeight) && spY > 0))
			spY = 0;

		moveX(spX*aclcMultply);
		moveY(spY*aclcMultply);
	}

	void updateLogic(RenderWindow& win)   //Called once per frame.
	{
		if (!win.isOpen())
			return;
		if (isDead)
		{
			if (deadTimer.getElapsedTime() >= seconds(3))
			{
				isDead = false;
				launch(rand() % arenaWidth, rand() % arenaHeight, 270, name);
			}
			return;
		}
		Time aclcTime = aclcTimer.restart();
		double aclcMultply = aclcTime.asMicroseconds() / 20000.0;
		if (Keyboard::isKeyPressed(isSpiltControled ? Keyboard::A : Keyboard::Left)) //Turn left;
			rotate(-2);
		if (Keyboard::isKeyPressed(isSpiltControled ? Keyboard::D : Keyboard::Right)) //Turn right;
			rotate(2);
		isFire = None;
		if (Keyboard::isKeyPressed(isSpiltControled ? Keyboard::W : Keyboard::Up) && !engineOff && !dock) //Go ahead;
		{
			spUpdate(1, aclcMultply);
			isFire = Back;
		}
		else
			spUpdate(0, aclcMultply);
		if (fireTimer > 0)
			fireTimer--;
		else if (!isDocked() && Keyboard::isKeyPressed(isSpiltControled ? Keyboard::C : Keyboard::Space) && useEnergy(80)) //SHOOT!!!
			shoot();
		else if (isDocked() && Keyboard::isKeyPressed(isSpiltControled ? Keyboard::C : Keyboard::Space) && useEnergy(200))
			shootLarge();
		if (dockKeyPressed && !Keyboard::isKeyPressed(isSpiltControled ? Keyboard::V : Keyboard::L))
			dockDown(!isDocked());
		dockKeyPressed = Keyboard::isKeyPressed(isSpiltControled ? Keyboard::V : Keyboard::L);
		if (offX < 0) { //Collison on the four edges; emit particles.
			offX = 0; spX = 0;
			particleManager.emit(Vector2d(offX, offY), Color(240, 240, 240), 20, 4.0, 0, 90);
		}
		if (offX > arenaWidth) {
			offX = arenaWidth; spX = 0;
			particleManager.emit(Vector2d(offX, offY), Color(240, 240, 240), 20, 4.0, 180, 90);
		}
		if (offY < 0) {
			offY = 0; spY = 0;
			particleManager.emit(Vector2d(offX, offY), Color(240, 240, 240), 20, 4.0, 90, 90);
		}
		if (offY > arenaHeight) {
			offY = arenaHeight; spY = 0;
			particleManager.emit(Vector2d(offX, offY), Color(240, 240, 240), 20, 4.0, 270, 90);
		}
		//Progcess shield / health.
		if (shield > maxShield)
			shield = maxShield;
		else if (shield != maxShield)
			shield += 4.0* aclcMultply;
		if (health > maxHealth)
			health = maxHealth;
		else if (health != maxHealth)
			health += 1.0* aclcMultply;
		//Progcess fuel.
		if (fuel <= 0)
		{
			fuel = 0;
			engineOff = true;
		}
		if (engineOff&&fuel >= maxFuel / 3.0)
			engineOff = false;
		fuel += 10.0* aclcMultply;
		if (fuel > maxFuel)
			fuel = maxFuel;
		//Progcess energy.

		if (energy > maxEnergy)
			energy = maxEnergy;
		else if (energy < maxEnergy)
			energy += 5 * aclcMultply;

		if (vibrateTimer > 0)
			vibrateTimer--;
	}

	void shoot()
	{
		fireTimer = 10;
		int tmp = 0;
		while (bullet[tmp].isAlive())
			tmp++;
		Vector2d posD = getPositionDouble();
		Vector2d speedD = getSpeedDouble();
		bullet[tmp].launch(posD.x + 20.0*cos(rot*PI / 180.0), posD.y + 20.0*sin(rot*PI / 180.0), rot, Bullet::Small, getUuid());
		particleManager.emit(Vector2d(posD.x + 20.0*cos(rot*PI / 180.0), posD.y + 20.0*sin(rot*PI / 180.0)), Color(192, 192, 192), 2, 2.0, rot, 15.0);
	}

	void shootLarge() {
		fireTimer = 15;
		int tmp = 0;
		while (bullet[tmp].isAlive())
			tmp++;
		Vector2d posD = getPositionDouble();
		Vector2d speedD = getSpeedDouble();
		int rot = getRotation();
		bullet[tmp].launch(posD.x + 20.0*cos(rot*PI / 180.0), posD.y + 20.0*sin(rot*PI / 180.0), rot, Bullet::Large, getUuid());
		particleManager.emit(Vector2d(posD.x + 20.0*cos(rot*PI / 180.0), posD.y + 20.0*sin(rot*PI / 180.0)), Color(192, 192, 192),
			10, 2.0, rot, 30.0);
	}

	void onRender(RenderWindow& win)
	{
		if (!win.isOpen())
			return;
		if (isDead)
			return;
		/*if (isExplode)
		{
			sepl.setTexture(epl[exani / 2]);
			sepl.setPosition(offX, offY);
			win.draw(sepl);
			return;
		}*/
		if (isFire == Back)
			drawFire(true, win);/*
		else if (isFire == Front)
			drawFire(false, win);*/
		drawShip(win);
		drawName(win);
	}

	void onRenderUI(RenderWindow& win)
	{
		if (!win.isOpen())
			return;
		//Display healthbar.
		register RectangleShape shape(Vector2f(202, 12)), dis(Vector2f(200, 10));
		register FloatRect boundRect = fon.getBoundingRect(NFont::Text(2, 2, "Health", Color::White, true, 1, 1, 1));
		fon.displayText(NFont::Text(208, win.getView().getSize().y - 3 * 15 - 3, "Health", Color::White, true, 1, 1, 1), win);
		shape.setFillColor(Color::Black);
		shape.setOutlineColor(Color::White);
		shape.setOutlineThickness(1.0f);
		shape.setPosition(2, win.getView().getSize().y - 3 * 15 + 1);
		win.draw(shape);
		dis.setSize(Vector2f((float)health / (float)maxHealth*200.0, 10));
		dis.setFillColor(Color::Red);
		dis.setOutlineThickness(0.0f);
		dis.setPosition(shape.getPosition().x + 1, shape.getPosition().y + 1);
		win.draw(dis);
		//Display shield status.
		boundRect = fon.getBoundingRect(NFont::Text(2, 2, "Shield", Color::White, true, 1, 1, 1));
		fon.displayText(NFont::Text(208, win.getView().getSize().y - 2 * 15 - 3, "Shield", Color::White, true, 1, 1, 1), win);
		shape.setFillColor(Color::Black);
		shape.setOutlineColor(Color::White);
		shape.setOutlineThickness(1.0f);
		shape.setPosition(2, win.getView().getSize().y - 2 * 15 + 1);
		win.draw(shape);
		dis.setSize(Vector2f((float)shield / (float)maxShield*200.0, 10));
		dis.setFillColor(Color(64, 64, 192));
		dis.setOutlineThickness(0.0f);
		dis.setPosition(shape.getPosition().x + 1, shape.getPosition().y + 1);
		win.draw(dis);
		//Display fuel.
		boundRect = fon.getBoundingRect(NFont::Text(4, 4 + 15 * 2, "Engine Energy", Color::White, true, 1, 1, 1));
		fon.displayText(NFont::Text(208, win.getView().getSize().y - 15 - 3, "Engine Energy", Color::White, true, 1, 1, 1), win);
		shape.setFillColor(Color::Black);
		shape.setOutlineColor((engineOff ? Color::Red : Color::White));
		shape.setOutlineThickness(1.0f);
		shape.setPosition(2, win.getView().getSize().y - 15 + 1);
		win.draw(shape);
		dis.setSize(Vector2f((float)fuel / (float)maxFuel*200.0, 10));
		dis.setFillColor(Color(255, 192, 0));
		dis.setOutlineThickness(0.0f);
		dis.setPosition(shape.getPosition().x + 1, shape.getPosition().y + 1);
		win.draw(dis);
		//Display energy on the lower right.
		boundRect = fon.getBoundingRect(NFont::Text(4, 4 + 15 * 2, "Weapon Energy", Color::White, true, 1, 1, 1));
		fon.displayText(NFont::Text(win.getView().getCenter().x - win.getView().getSize().x / 2.0 +
			win.getView().getSize().x - 308 - boundRect.width,
			win.getView().getSize().y - 3 - boundRect.height,
			"Weapon Energy", Color::White, true, 1, 1, 1), win);
		shape.setSize(Vector2f(302, 12));
		shape.setFillColor(Color::Black);
		shape.setOutlineColor(Color::White);
		shape.setOutlineThickness(1.0f);
		shape.setPosition(win.getView().getSize().x - 304,
			win.getView().getSize().y - 14);
		win.draw(shape);
		dis.setSize(Vector2f((float)energy / (float)maxEnergy*300.0, 10));
		dis.setFillColor(Color::Blue);
		dis.setPosition(win.getView().getSize().x - 303,
			win.getView().getSize().y - 13);
		win.draw(dis);
		fon.displayText(NFont::Text(1, 1, getFormatString(), Color::White, true, 1, 1, 1), win);
	}

	bool useEnergy(int amount) //True if enough.
	{
		if (energy < amount)
			return false;
		else
		{
			energy -= amount;
			return true;
		}
	}

	void vibrate(int timer = 16) {
		vibrateTimer = timer;
	}

	void harm(int amount)
	{
		if (isDead)
			return;
		if (shield >= amount) {
			shield -= amount;
			particleManager.emit(Vector2d(offX, offY), Color(0, 0, 255), 20, 2.5);
		}
		else if (shield < amount)
		{
			amount -= shield;
			shield = 0;
			health -= amount;
			particleManager.emit(Vector2d(offX, offY), Color(0, 0, 255), 5, 2.5);
			particleManager.emit(Vector2d(offX, offY), Color(255, 32, 0), 15, 3.0);
			if (health <= 0)
			{
				health = 0;
				if (!isDead)
					die();
			}
		}
		vibrate();
	}

	void die()
	{
		//isExplode = true;
		isDead = true;
		deadTimer.restart();
		particleManager.emit(Vector2d(offX, offY), Color(255, 240, 0), 250, 4.0);
		particleManager.emit(Vector2d(offX, offY), Color(255, 0, 0), 70, 6.0);
		//exani = 0;
		fuel = energy = 0;
		vibrate();
		vibrate();
	}

	void dockDown(bool type = true)
	{
		if (type)
			spX = spY = 0;
		dock = type;
	}

	bool isDocked() { return dock; }

	Vector2f getPosition() { return Vector2f(offX, offY); }

	Vector2d getPositionDouble() { return Vector2d(offX, offY); }

	int getRotation() { return rot; }

	Vector2f getSpeed() { return Vector2f(spX, spY); }

	Vector2d getSpeedDouble() { return Vector2d(spX, spY); }

	String getName() { return String(name); }

	double getEnergy() { return energy; }

	virtual const FloatRect getHitbox() { return FloatRect(offX - sizeX / 2.0f, offY - sizeY / 2.0f, sizeX, sizeY); }

	void resetLogicClock() { aclcTimer.restart(); }

	void updateGameView(RenderWindow& win)
	{
		View view(Vector2f(offX, offY), Vector2f(win.getSize().x, win.getSize().y));
		if (vibrateTimer > 0)
			view.move(abs(vibrateTimer % 8 - 4) - 2, abs(vibrateTimer % 8 - 4) - 2);
		view.zoom(viewZoomFactor);
		win.setView(view);
	}

	void updateUIView(RenderWindow& win) {
		View view(FloatRect(0, 0, win.getSize().x, win.getSize().y));
		if (vibrateTimer > 0)
			view.move(-abs(vibrateTimer % 8 - 4) + 2, abs(vibrateTimer % 8 - 4) - 2);
		win.setView(view);
	}

private:
	int rot;
	double sp;
	double spX, spY;
	double offX, offY;
	string name;
	double fuel;
	bool engineOff;
	double health, shield;
	double energy;
	bool dock;

	Clock aclcTimer;

	int fireTimer;
	int vibrateTimer;

	bool dockKeyPressed;
	bool isReady;  //init() must be called before rendering.
};

Ship ship;

