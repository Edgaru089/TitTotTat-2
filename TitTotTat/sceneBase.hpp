#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

class SceneBase {
public:
	SceneBase() :isAlive(true) {}
	virtual ~SceneBase() {}
	virtual void preLaunchInit() {};
	virtual void launch(RenderWindow&) {};
	virtual const bool handleEvent(RenderWindow&, Event&) { return true; };
	virtual void adjustViewportChange(RenderWindow&) {}
	virtual void updateLogic(RenderWindow&) {};
	virtual void onRender(RenderWindow&) {};
	virtual void stop() { isAlive = false; }
	virtual const bool isReady() { return isAlive; }
private:
	bool isAlive;
};
