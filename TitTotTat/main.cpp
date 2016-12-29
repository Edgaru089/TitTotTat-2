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

int main(int argc, char* argv[])
{
	mainScenePtr = &sceneLaunch;

	win.setFramerateLimit(60);
	cout << "Pre-Initalazing...";
	srand(time(NULL));
	bool isFullscreen = false;
	fon.loadFromBinary("ibmank0.bin");
	//fon.loadFromFile("ascii.txt", "ascii.png");
	sceneLaunch.preLaunchInit();
	sceneGame.preLaunchInit();
	cout << " Done." << endl;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;
	win.create(VideoMode(1080, 608), "TitTotTat", Style::Default, settings);
	win.setFramerateLimit(60);
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
	while (mainScenePtr->isReady())
	{
		Event event;
		while (win.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
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
						win.create(VideoMode::getDesktopMode(), "TitTotTat", Style::Fullscreen, settings);
						win.setFramerateLimit(60);
						isFullscreen = true;
						mainScenePtr->adjustViewportChange(win);
					}
					else
					{
						win.create(VideoMode(1080, 608), "TitTotTat", Style::Close | Style::Titlebar | Style::Resize, settings);
						win.setFramerateLimit(60);
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
		mainScenePtr->updateLogic(win);
		mainScenePtr->onRender(win);
		if (doesSceneChange) {
			doesSceneChange = false;
			mainScenePtr->stop();
			mainScenePtr = changeToScenePtr;
			mainScenePtr->launch(win);
			continue;
		}
	}
	return EXIT_SUCCESS;
}

#endif
