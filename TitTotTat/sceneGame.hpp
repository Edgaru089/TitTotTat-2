#pragma once

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SelbaWard.hpp>

#include "sceneBase.hpp"
#include "main.hpp"
#include "ship.hpp"
#include "bullet.hpp"
#include "Button.hpp"
#include "networkMain.hpp"
#include "objectCollisonManager.hpp"
#include "Particle.hpp"
#include "Tower.hpp"
//#include "tower.hpp"

using namespace std;
using namespace sf;

class PausedSceneGame {
public:
	void preLaunchInit() {
		isPaused = false;
		resume.preLaunchInit(Button::ColorType(), "Resume To Game", 280, 36);
		quit.preLaunchInit(Button::ColorType(), "Quit To Title", 280, 36);
		rect.setFillColor(Color(0, 0, 0, 128));
		rect.setPosition(0, 0);
	}

	void pauseGame(RenderWindow& win) {
		if (isPaused) {
			resumeGame(win);
			return;
		}
		isPaused = true;
		adjustViewportChange(win);
	}

	void resumeGame(RenderWindow& win) {
		isPaused = false;
		ship.resetLogicClock();
	}

	void quitToTitle(RenderWindow& win) {
		changeScene(sceneMapper["SceneLaunch"]);
		isPaused = false;
	}

	void adjustViewportChange(RenderWindow& win) {
		Vector2u winSize = win.getSize();
		int leftInterval = winSize.x / 2;
		int topInterval = winSize.y / 4;
		FloatRect srect = fon.getBoundingRect(NFont::Text(0, 0, "Game Paused", Color(255, 255, 255), true));
		pausedTip = NFont::Text(leftInterval, topInterval, "Game Paused", Color::White, true, 1, 1, 1, srect.width / 2, srect.height / 2);
		int buttonTopInterval = topInterval + srect.height + 60;
		resume.updatePosition(leftInterval, buttonTopInterval);
		quit.updatePosition(leftInterval, buttonTopInterval + 36 + 5);
		rect.setSize(Vector2f(winSize.x, winSize.y));
	}

	void pausedGameLogic(RenderWindow& win) {
		if (resume.isButtonPressed() && !Mouse::isButtonPressed(Mouse::Left))
			resumeGame(win);
		if (quit.isButtonPressed() && !Mouse::isButtonPressed(Mouse::Left))
			quitToTitle(win);
		resume.updateLogic(win);
		quit.updateLogic(win);
	}

	void pausedGameRender(RenderWindow& win) {
		//GameScene Rendering should be completed by now.
		win.draw(rect);
		fon.displayText(pausedTip, win);
		resume.onRender(win);
		quit.onRender(win);
		win.display();
	}

	bool paused() {
		return isPaused;
	}
private:
	bool isPaused;
	Button resume, quit;
	NFont::Text pausedTip;
	RectangleShape rect;
};

class SceneGame :public SceneBase
{
public:

	SceneGame() :isAlive(false), isNetworkUsed(true), outerBound(Quads, 4) {}

	void preLaunchInit()
	{
		outerBound[0].position = Vector2f(0, 0);
		outerBound[0].color = Color(41, 57, 86);
		outerBound[1].position = Vector2f(0, arenaHeight);
		outerBound[1].color = Color(41, 57, 86);
		outerBound[2].position = Vector2f(arenaWidth, arenaHeight);
		outerBound[2].color = Color(41, 57, 86);
		outerBound[3].position = Vector2f(arenaWidth, 0);
		outerBound[3].color = Color(41, 57, 86);
		if (!stone.loadFromFile("stone.png")); //TODO: Exception handling.
		stone.setRepeated(true);
		stone.setSmooth(false);
		stoneSprite.setTexture(stone);
		stoneSprite.setTextureRect(IntRect(0, 0, arenaWidth, arenaHeight));
		//fon.loadFromFile("ascii.txt", "ascii.png");
		isNetworkRunning = false;
		pausedScene.preLaunchInit();
		ship.preLaunchInit();
		isSpiltControled = false;
		sceneMapper[(string)"SceneGame"] = this;
	}
	string launchNetworkThread(bool listen, IpAddress& ip, unsigned short port, bool useStdIo = false) {
		if (!isNetworkUsed)
			return "FAILED: Network not enabled.";
		string str= NetworkInterface::startThread(listen, ip, port, useStdIo);
		cout << "NETWORK: " << str << endl;
		return str;
		isNetworkRunning = true;
	}
	void launch(RenderWindow& win)
	{
		win.clear(Color(64, 64, 64));
		fon.displayText(NFont::Text(2, 2,
			(isNetworkUsed ? "initializing Game Scene...\nPlease refer to console for Network Settings." : "Initalizing Game Scene..."),
			Color::White), win);
		win.display();
		ship.launch(rand() % arenaWidth, rand() % arenaHeight, 0, "TestApp");
		win.setFramerateLimit(60);
		particleManager.clearList();
		towerList.clear();
		isFullscreen = false;
		isSpiltControled = false;
		//if (isNetworkUsed&&!isNetworkRunning) {
			//cout << "NETWORK: " << NetworkInterface::startThread(false, IpAddress(), 2555, true) << endl;
		//}
		//win.setSize(Vector2u(600, 480));
		win.setView(View(FloatRect(0, 0, win.getSize().x, win.getSize().y)));
		win.setVisible(true);
		isAlive = true;
	}

	void onRender(RenderWindow& win)
	{
		if (!win.isOpen())
			return;
		win.clear();
		/*----- Scene Rendering Starting -----*/
		ship.updateGameView(win);
		View view;
		//win.draw(outerBound);
		win.draw(stoneSprite);
		for (int i = 0; i < bulletCount; i++)
			if (bullet[i].isAlive())
				bullet[i].onRender(win);
		particleManager.onRender(win);
		NetworkInterface::threadRenderer(win);
		ship.onRender(win);
		towerList.onRender(win);
		//for (int i = 0; i < towerCount; i++)
		//	if (tower[i].isAlive())
		//		tower[i].onRender(win);
		/*----- Scene Rendering Ended; Starting UI Rendering -----*/
		ship.updateUIView(win);
		ship.onRenderUI(win);
		if (pausedScene.paused()) {
			pausedScene.pausedGameRender(win);
			return;
		}
		win.display();
	}

	const bool handleEvent(RenderWindow& win, Event& event)
	{
		if (event.type == Event::KeyPressed&&event.key.code == Keyboard::H)
			ship.harm(200);
		else if (event.type == Event::KeyReleased&&event.key.code == Keyboard::Escape)
			pausedScene.pauseGame(win);
		else if (event.type == Event::KeyReleased&&event.key.code == Keyboard::K)
			isSpiltControled = !isSpiltControled;
		else if (event.type == Event::KeyReleased&&event.key.code == Keyboard::J)
			towerList.addTower(ship.getPositionDouble());
		else
			return false;
		return true;
	}

	void updateLogic(RenderWindow& win)
	{
		if (!win.isOpen())
		{
			isAlive = false;
			return;
		}
		if (pausedScene.paused()) {
			pausedScene.pausedGameLogic(win);
			if (!isNetworkUsed)
				return;
		}
		logicDataLock.lock();
		for (int i = 0; i < bulletCount; i++)
			if (bullet[i].isAlive())
				bullet[i].updateLogic(win);
		ship.updateLogic(win);
		towerList.updateLogic(win);
		particleManager.updateLogic(win);
		logicDataLock.unlock();
		//NetworkInterface::threadWorkerNoThread();
	}

	void stop()
	{
		isAlive = false;
		isStopping = true;
		NetworkInterface::stopThread();
		particleManager.clearList();
	}

	void setNetworkUsed(bool used) {
		isNetworkUsed = used;
	}

	const bool isReady()
	{
		return isAlive;
	}

private:
	bool isNetworkUsed,isNetworkRunning;
	bool isAlive;
	bool isFullscreen;
	Texture curScene, stone;
	Sprite stoneSprite;
	VertexArray outerBound;
	PausedSceneGame pausedScene;
};

SceneGame sceneGame;
