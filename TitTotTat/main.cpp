#include "Overall.hpp"

#ifndef TEST_INSTED_OF_MAIN

#include <cstdlib>
#include <iostream>
#include <thread>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "main.hpp"
#include "sceneGame.hpp"
#include "mainMenu.hpp"
#include "sceneLaunch.hpp"

using namespace std;
using namespace sf;

void threadRendering(recursive_mutex* renderLock) {
	win.setActive(true);
	win.setVerticalSyncEnabled(false);
	win.setFramerateLimit(60);
	while (mainScenePtr->isReady()) {
		renderLock->lock();
		mainScenePtr->onRender(win);
		frameCounter++;
		if (frameCounterClock.getElapsedTime() > seconds(1.0f)) {
			frameCounterClock.restart();
			framePerSecond = frameCounter;
			frameCounter = 0;
			printf("\rTPS: %d, FPS: %d", logicTickPerSecond, framePerSecond);
		}
		renderLock->unlock();
	}
}

void threadLogicUpdate() {
	Clock logicCycleClock;
	while (mainScenePtr->isReady()) {
		logicCycleClock.restart();
		mainScenePtr->updateLogic(win);
		Time t;
		logicTickCounter++;
		if (logicTickCounterClock.getElapsedTime() > seconds(1.0f)) {
			logicTickCounterClock.restart();
			logicTickPerSecond = logicTickCounter;
			logicTickCounter = 0;
		}
		if ((t = logicCycleClock.restart()) < microseconds(20000))
			sleep(microseconds(20000) - t);
	}
}

int main(int argc, char* argv[])
{
	mainScenePtr = &sceneLaunch;

	cout << "Pre-Initalazing...";
	srand(time(NULL));
	bool isFullscreen = false;
	//fon.loadFromBinary("ibmank0.bin");
	fon.loadFromFile("ascii.txt", "ascii.png");
	sceneLaunch.preLaunchInit();
	sceneGame.preLaunchInit();
	cout << " Done." << endl;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;
	win.create(VideoMode(1080, 608), "TitTotTat", Style::Default, settings);
	win.setVerticalSyncEnabled(true);
	win.setVisible(true);
	cout << "Post-Initalazing...";
	mainScenePtr->launch(win);
	if (argc == 4 && strcmp(argv[1], "-connect") == 0) {
		cout << "Connecting..." << endl;
		connSelect.enable(ConnectionSelectLogic::connectMode, true);
		sceneGame.setNetworkUsed(true);
		connSelect.addressBox.setString(argv[2]);
		connSelect.portBox.setString(argv[3]);
		thread th(startConnection, &win);
		th.detach();
	}
	cout << " Done." << endl;
#ifdef USE_ASYNC_RENDERING
	cout << "Async Rendering/Logic Update Enabled. Unstable. Aware." << endl;
	recursive_mutex renderLock;
	win.setActive(false);
	thread render(threadRendering, &renderLock);
	thread logic(threadLogicUpdate);
#endif
	
	while (mainScenePtr->isReady())
	{
		
		Event event;
		while (win.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				logicDataLock.lock();
				mainScenePtr->stop();
				win.close();
			}
			else if (event.type == Event::Resized) {
				win.setView(View(FloatRect(0, 0, event.size.width, event.size.height)));
				mainScenePtr->adjustViewportChange(win);
			}
			else if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::F11)
					if (!isFullscreen)
					{
#ifdef USE_ASYNC_RENDERING
						renderLock.lock();
#endif
						win.create(VideoMode::getDesktopMode(), "TitTotTat", Style::Fullscreen, settings);
						win.setVerticalSyncEnabled(true);
#ifdef USE_ASYNC_RENDERING
						win.setActive(false);
						renderLock.unlock();
#endif
						isFullscreen = true;
						mainScenePtr->adjustViewportChange(win);
					}
					else
					{
#ifdef USE_ASYNC_RENDERING
						renderLock.lock();
#endif
						win.create(VideoMode(1080, 608), "TitTotTat", Style::Close | Style::Titlebar | Style::Resize, settings);
						win.setVerticalSyncEnabled(true);
#ifdef USE_ASYNC_RENDERING
						win.setActive(false);
						renderLock.unlock();
#endif
						isFullscreen = false;
						mainScenePtr->adjustViewportChange(win);
					}
				else if (event.key.code == Keyboard::Equal)
					viewZoomFactor *= 0.5f;
				else if (event.key.code == Keyboard::Dash)
					viewZoomFactor *= 2.0f;
				else
					mainScenePtr->handleEvent(win, event);
			}
			else
				mainScenePtr->handleEvent(win, event);
		}
		if (!win.isOpen())
			break;
#ifndef USE_ASYNC_RENDERING
		mainScenePtr->updateLogic(win)
		mainScenePtr->onRender(win);
#endif
		if (doesSceneChange) {
			doesSceneChange = false;
			mainScenePtr->stop();
			mainScenePtr = changeToScenePtr;
			mainScenePtr->launch(win);
			continue;
		}
	}
	logicDataLock.unlock();
	cout << "Shutdown In Progress..." << endl;
#ifdef USE_ASYNC_RENDERING
	cout << "[*] Joining Render Thread...";
	render.join();
	cout << "[*] Joining Logic Update Thread...";
	logic.join();
	cout << " Complete." << endl;
#endif
	cout << "Byebye!" << endl;
	return EXIT_SUCCESS;
}

#endif
