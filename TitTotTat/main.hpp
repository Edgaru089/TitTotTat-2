#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <mutex>
#include <map>
#include "MainResDef.hpp"
#include "basicObject.hpp"
#include "sceneBase.hpp"

using namespace sf;
using namespace std;

#define USE_ASYNC_RENDERING

typedef Vector2<double> Vector2d;

const int arenaWidth = 1600, arenaHeight = 900;
const int majorVersion = 1, minorVersion = 5, releaseVersion = 4, build = 4020;

const double PI = 3.14159265359;
Image shipPng;
float viewZoomFactor = 1.0f;

RenderWindow win;

recursive_mutex logicDataLock;

SceneBase* mainScenePtr;

bool doesSceneChange;
SceneBase* changeToScenePtr;

map<string, SceneBase*> sceneMapper; //TODO: Map the scenes into the container.

int logicTickPerSecond, logicTickCounter, framePerSecond, frameCounter;
Clock logicTickCounterClock, frameCounterClock;


void changeScene(SceneBase& scene) {
	logicDataLock.lock();
	changeToScenePtr = &scene;
	doesSceneChange = true;
	logicDataLock.unlock();
}

void changeScene(SceneBase* scene) {
	logicDataLock.lock();
	changeToScenePtr = scene;
	doesSceneChange = true;
	logicDataLock.unlock();
}

double getDis(double posX1, double posY1, double posX2, double posY2)
{
	return sqrt(pow(abs(posX1 - posX2), 2.0) + pow(abs(posY1 - posY2), 2.0));
}

bool isSame(double x, double y)
{
	if (abs(x - y) < 1e-4)
		return true;
	else
		return false;
}

int systemString(string command) {
	return system(command.c_str());
}
