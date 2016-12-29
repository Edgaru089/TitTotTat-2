#pragma once

#include <cstdlib>
#include <cmath>
#include <list>
#include <queue>
#include <mutex>

#include <SFML/Graphics.hpp>
#include "main.hpp"

using namespace std;
using namespace sf;

#define VIBRATE (rand() / (double)(RAND_MAX/1.5))
#define DEFAULT_PARTICLE_SPEED (Vector2d((rand() - RAND_MAX / 2) / (double)(RAND_MAX / 2), (rand() - RAND_MAX / 2) / (double)(RAND_MAX / 2)))
#define PARTICLE_DEACLC 0.04

class Particle {
public:
	Particle() { alive = false; }
	Particle(Color color, Vector2d pos, Vector2d speed = DEFAULT_PARTICLE_SPEED, bool slowDown = true) {
		//Default to launched after construction.
		this->color = color;
		this->spX = speed.x;
		this->spY = speed.y;
		offX = pos.x;
		offY = pos.y;
		alive = true;
		aclcTimer.restart();
		////Cout << "New particle created & launched." << endl;
	}
	void onRender(RenderWindow& win) {
		rect.setFillColor(color);
		rect.setSize(Vector2f(2.0f, 2.0f));
		rect.setOrigin(1.0f, 1.0f);
		rect.setPosition(offX, offY);
		win.draw(rect);
	}
	void updateLogic(RenderWindow& win) {
		Time aclcTime = aclcTimer.restart();
		double aclcMultply = aclcTime.asMilliseconds() / 20.0;
		offX += spX;
		offY += spY;
		spX -= PARTICLE_DEACLC*spX;
		spY -= PARTICLE_DEACLC*spY;
		if (abs(spX)<=0.002 && abs(spY)<=0.002)
			kill();
	}
	void kill() {
		alive = false;
	}
	bool isAlive() {
		return alive;
	}
private:
	Color color;
	double spX, spY;
	double offX, offY;
	bool slowDown;
	bool alive;
	RectangleShape rect;
	Clock aclcTimer;
};

class ParticleManager {
public:
	static const int randomSpeedAndDirection = 0,
		randomDirection = 1,
		setSpeedAndDirection = 2,
		shockwave = 3;


	struct ParticleEmitTrace
	{
		ParticleEmitTrace() {}
		ParticleEmitTrace(Vector2d pos, Color color, int count) {
			type = randomSpeedAndDirection;
			posX = pos.x;
			posY = pos.y;
			this->color = color;
			this->count = count;
		}
		ParticleEmitTrace(Vector2d pos, Color color, int count, double speed) {
			type = randomDirection;
			posX = pos.x;
			posY = pos.y;
			this->color = color;
			this->count = count;
			this->speed = speed;
		}
		ParticleEmitTrace(Vector2d pos, Color color, int count, double speed, double angle, double angleFlow) {
			type = setSpeedAndDirection;
			posX = pos.x;
			posY = pos.y;
			this->color = color;
			this->count = count;
			this->speed = speed;
			this->angle = angle;
			this->angleFlow = angleFlow;
		}
		double posX, posY, speed, angle, angleFlow;
		Color color;
		int count;
		int type;
	};

	ParticleManager() {
		partList.clear();
	}
	void emit(Vector2d pos, Color color, int count, bool logIntoQueue = true) { //Speed random, direction random.
		////Cout << "Emiting particles via randomSpeedAndDirection mode." << endl;
		int count1 = count;
		partListLock.lock();
		while (count1--) {
			partList.push_back(Particle(color, pos));
		}
		partListLock.unlock();
		if (logIntoQueue) {
			partEmitTraceLock.lock();
			partEmitTrace.push(ParticleEmitTrace(pos, color, count));
			partEmitTraceLock.unlock();
		}
	}
	void emit(Vector2d pos, Color color, int count, double speed, bool logIntoQueue = true) { //Speed set, direction random.
		//Cout << "Emiting particles via randomDirection mode." << endl;
		int count1 = count;
		partListLock.lock();
		while (count1--) {
			double setAngle = (rand() - RAND_MAX / 2) / (float)RAND_MAX * 360;
			partList.push_back(Particle(color, pos, Vector2d(speed*VIBRATE*cos(PI*setAngle / 180.0f), speed*VIBRATE*sin(PI*setAngle / 180.0f))));
		}
		partListLock.unlock();
		if (logIntoQueue) {
			partEmitTraceLock.lock();
			partEmitTrace.push(ParticleEmitTrace(pos, color, count, speed));
			partEmitTraceLock.unlock();
		}
	}
	void emit(Vector2d pos, Color color, int count, double speed, double angle, double angleFlow = 10.0, bool logIntoQueue = true) { //Speed set, direction & alternations set.
		//Cout << "Emiting particles via setSpeedAndDirection mode. Count: " << count << endl;
		int count1 = count;
		partListLock.lock();
		while (count1--) {
			double setAngle = angle + (rand() - RAND_MAX / 2) / (double)(RAND_MAX / 2) * angleFlow;
			double setSpeed = speed*VIBRATE;
			partList.push_back(Particle(color, pos, Vector2d(setSpeed*cos(PI*setAngle / 180.0f), setSpeed*sin(PI*setAngle / 180.0f))));
		}
		partListLock.unlock();
		if (logIntoQueue) {
			partEmitTraceLock.lock();
			partEmitTrace.push(ParticleEmitTrace(pos, color, count, speed, angle, angleFlow));
			partEmitTraceLock.unlock();
			//Cout << "Logged into queue, COUNT: " << count;
		}
	}
	void emit(ParticleEmitTrace trace) {
		//Cout << "Trying to emit particle via traces." << endl;
		if (trace.type == randomSpeedAndDirection)
			emit(Vector2d(trace.posX, trace.posY), trace.color, trace.count, false);
		else if (trace.type == randomDirection)
			emit(Vector2d(trace.posX, trace.posY), trace.color, trace.count, trace.speed, false);
		else if (trace.type == setSpeedAndDirection)
			emit(Vector2d(trace.posX, trace.posY), trace.color, trace.count, trace.speed, trace.angle, trace.angleFlow, false);
	}
	void onRender(RenderWindow& win) {
		partListLock.lock();
		for (list<Particle>::iterator i = partList.begin(); i != partList.end(); i++) {
			i->onRender(win);
		}
		partListLock.unlock();
	}
	void updateLogic(RenderWindow& win) {
		partListLock.lock();
		for (list<Particle>::iterator i = partList.begin(); i != partList.end();) {
			i->updateLogic(win);
			if (!i->isAlive()) {
				i = partList.erase(i);
			}
			else {
				i++;
			}
		}
		partListLock.unlock();
	}
	void clearList() { partList.clear(); while (!partEmitTrace.empty())partEmitTrace.pop(); }
	queue<ParticleEmitTrace> partEmitTrace;
	mutex partListLock, partEmitTraceLock;
private:
	list<Particle> partList;
};

ParticleManager particleManager;
