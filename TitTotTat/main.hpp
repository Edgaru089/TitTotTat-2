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

#define TEST_INSTED_OF_MAIN

typedef Vector2<double> Vector2d;

const int arenaWidth = 1600, arenaHeight = 900;
const int majorVersion = 0, minorVersion = 4, releaseVersion = 2, build = 2809;

const double PI = 3.14159265359;
Image shipPng;
float viewZoomFactor = 1.0f;

RenderWindow win;

recursive_mutex logicDataLock;

SceneBase* mainScenePtr;

bool doesSceneChange;
SceneBase* changeToScenePtr;

map<string, SceneBase*> sceneMapper; //TODO: Map the scenes into the container.


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
