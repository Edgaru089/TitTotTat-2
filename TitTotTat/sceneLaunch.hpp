#pragma once

#include <SFML/Graphics.hpp>
#include <SelbaWard.hpp>
#include "sceneBase.hpp"
#include "sceneGame.hpp"
#include "Button.hpp"
#include "Textbox.hpp"
#include "main.hpp"

using namespace sf;
using namespace sw;

void startConnection(RenderWindow*);

class ConnectionSelectLogic {
public:
	static const int listenMode = 0, connectMode = 1;
	static const int textboxWidth = 360;
	void preLaunchInit() {
		goBack.preLaunchInit(Button::ColorType(), "Return To Main Menu", 200, 24);
		listen.preLaunchInit(Button::ColorType(), "Server Mode", 120, 24);
		connect.preLaunchInit(Button::ColorType(), "Client Mode", 120, 24);
		confirm.preLaunchInit(Button::ColorType(), "Start!", textboxWidth, 32);
		spilt.preLaunchInit(Button::ColorType(), "Play Spilt-Screened", textboxWidth, 32);
		addressBox.preLaunchInit(Textbox::ColorType(), "", "Server Address", textboxWidth, 28);
		portBox.preLaunchInit(Textbox::ColorType(), "", "Port", textboxWidth / 3, 28);
		titleText = NFont::Text(200 + 20, 9, "Network Loader");
		cancel.preLaunchInit(Button::ColorType(), "Cancel", textboxWidth, 28);
		bottomHint = NFont::Text(0, 0, "Connecting...");
		mode = connectMode;
	}
	void launch(RenderWindow& win) {
		goBack.launch(win, 200 / 2 + 5, 24 / 2 + 5);
		listen.launch(win, 15 + 120 / 2, 24 + 35 + 24 / 2);
		connect.launch(win, 15 + 120 / 2, 24 + 35 + 24 + 5 + 24 / 2);
		addressBox.launch(win, win.getView().getSize().x / 2 - addressBox.getSize().x / 2, win.getView().getSize().y / 7 * 2);
		portBox.launch(win, win.getView().getSize().x / 2 - addressBox.getSize().x / 2, win.getView().getSize().y / 7 * 2 + 45);
		confirm.launch(win, 0, 0);
		spilt.launch(win, 0, 0);
		cancel.launch(win, 0, 0);
		win.setView(View(FloatRect(0.0f, 0.0f, win.getSize().x, win.getSize().y)));
		adjustViewportChange(win);
		enabled = false;
	}
	void updateLogic(RenderWindow& win) {
		if (connecting) {
			if (cancel.isButtonPressed() && !Mouse::isButtonPressed(Mouse::Left)) {
				connecting = false;
			}
			cancel.updateLogic(win);
		}
		else {
			if (goBack.isButtonPressed() && !Mouse::isButtonPressed(Mouse::Left))
				stop();
			if (listen.isButtonPressed() && !Mouse::isButtonPressed(Mouse::Left))
				mode = listenMode;
			if (connect.isButtonPressed() && !Mouse::isButtonPressed(Mouse::Left))
				mode = connectMode;
			if (spilt.isButtonPressed() && !Mouse::isButtonPressed(Mouse::Left))
				playSpiltScreened();
			if (confirm.isButtonPressed() && !Mouse::isButtonPressed(Mouse::Left)) {
				thread th(startConnection, &win);
				th.detach();
			}
			goBack.updateLogic(win);
			listen.updateLogic(win);
			connect.updateLogic(win);
			addressBox.updateLogic(win);
			portBox.updateLogic(win);
			confirm.updateLogic(win);
			spilt.updateLogic(win);
			if (mode == listenMode)
				listen.increaseButtonDensity();
			else if (mode == connectMode)
				connect.increaseButtonDensity();
			if (mode == connectMode) {
				addressBox.updatePosition(win.getSize().x / 2 - textboxWidth / 2, win.getSize().y / 7 * 2);
				portBox.updatePosition(win.getSize().x / 2 - textboxWidth / 2, win.getSize().y / 7 * 2 + 75);
			}
			else {
				addressBox.updatePosition(win.getSize().x, win.getSize().y);
				portBox.updatePosition(win.getSize().x / 2 - textboxWidth / 2, win.getSize().y / 7 * 2);
			}
		}
	}
	void onRender(RenderWindow& win) {
		if (connecting) {
			fon.displayText(bottomHint, win);
			cancel.onRender(win);
		}
		else {
			goBack.onRender(win);
			listen.onRender(win);
			connect.onRender(win);
			addressBox.onRender(win);
			portBox.onRender(win);
			confirm.onRender(win);
			spilt.onRender(win);
			fon.displayText(titleText, win);
		}
	}
	void adjustViewportChange(RenderWindow& win) {
		adjustViewportChange(&win);
	}
	void adjustViewportChange(RenderWindow* win) {
		FloatRect rect = fon.getBoundingRect(titleText);
		titleText.x = (win->getSize().x - rect.width) / 2;
		titleText.y = win->getSize().y / 8;
		rect = fon.getBoundingRect(bottomHint);
		bottomHint.x = (win->getSize().x - rect.width) / 2;
		bottomHint.y = win->getSize().y / 8 * 3;
		cancel.updatePosition((win->getSize().x - cancel.getSize().x) / 2, win->getSize().y / 8 * 5);
		confirm.updatePosition((win->getSize().x - confirm.getSize().x) / 2, win->getSize().y / 7 * 2 + 2 * 75);
		spilt.updatePosition((win->getSize().x - confirm.getSize().x) / 2, win->getSize().y / 7 * 2 + 2 * 75 + 40);
		if (mode == connectMode) {
			addressBox.updatePosition(win->getSize().x / 2 - addressBox.getSize().x / 2, win->getSize().y / 7 * 2);
			portBox.updatePosition(win->getSize().x / 2 - portBox.getSize().x / 2, win->getSize().y / 7 * 2 + 75);
		}
		else {
			addressBox.updatePosition(win->getSize().x, win->getSize().y);
			portBox.updatePosition(win->getSize().x / 2 - portBox.getSize().x / 2, win->getSize().y / 7 * 2);
		}
	}

	void enable(int mode = listenMode, bool enable = true) { this->mode = mode; enabled = enable; }
	void stop() { enabled = false; }
	bool isEnabled() { return enabled; }

	void playSpiltScreened() {
		cout << "Playing Spilt-Screened on port ";
		char buf[100];
		port = rand() % 100 + 64000;
		cout << port << " !" << endl;
		sprintf(buf, "%d", port);
		portBox.setString(buf);
		sprintf(buf, "start TitTotTat.exe -connect localhost %d", port);
		thread th0(system, buf);
		th0.detach();
		mode = listenMode;
		thread th1(startConnection, &win);
		th1.detach();
	}

	bool enabled, connecting;
	int mode;
	IpAddress address;
	int port;
	Textbox addressBox, portBox;
	NFont::Text titleText, bottomHint;
	Button goBack, listen, connect, confirm, spilt;
	Button cancel;
};

ConnectionSelectLogic connSelect;

void startConnection(RenderWindow* win) {
	connSelect.bottomHint.str = "Connecting...";
	connSelect.adjustViewportChange(win);
	connSelect.connecting = true;
	connSelect.address = IpAddress(connSelect.addressBox.getString());
	port = atoi(connSelect.portBox.getString().c_str());
	cout << "STARTING CONNECTION TO " << connSelect.address.toString() << ":" << port << endl;
	if (connSelect.mode == ConnectionSelectLogic::connectMode)
		sleep(milliseconds(800));
	else
		sleep(milliseconds(50));
	sceneGame.setNetworkUsed(true);
	connSelect.bottomHint.str = sceneGame.launchNetworkThread(connSelect.mode == ConnectionSelectLogic::listenMode, connSelect.address, port, false);
	connSelect.adjustViewportChange(win);
	if (connSelect.bottomHint.str.c_str()[0] == 'S') {
		sleep(milliseconds(400));
		changeScene(sceneGame);
		connSelect.connecting = false;
	}
}

class SceneLaunch : public SceneBase {
public:

	static const int buttonHeight = 36, buttonWidth = 300, seperate = 6;

	SceneLaunch() :isAlive(false) {}
	void preLaunchInit() {
		connSelect.preLaunchInit();
		char buff[128];
		Image im;
		start.preLaunchInit(Button::ColorType(), "Host A Server", buttonWidth, buttonHeight);
		multplayer.preLaunchInit(Button::ColorType(), "Connect to Others", buttonWidth, buttonHeight);
		exit.preLaunchInit(Button::ColorType(), "Exit", buttonWidth / 4 * 3, buttonHeight);
		sprintf(buff, "TitTotTat TEST RELEASE\nVERSION %d.%d.%d.%d", majorVersion, minorVersion, releaseVersion, build);
		lowerLeft = NFont::Text(2, 2, buff);
		im.loadFromFile("background.png");
		im.createMaskFromColor(Color::Black);
		title.loadFromImage(im);
		spTitle.setTexture(title);
		spTitle.setOrigin(Vector2f(im.getSize().x / 2, im.getSize().y / 2));
		spTitle.setMeshDensity(128);
		sceneMapper[(string)"SceneLaunch"] = this;
	}

	void launch(RenderWindow& win) {
		connSelect.launch(win);
		win.setView(View(FloatRect(0, 0, win.getSize().x, win.getSize().y)));
		adjustViewportChange(win);
		isAlive = true;
	}

	const bool handleEvent(RenderWindow& win, Event& event) {
		return true;
	}

	void adjustViewportChange(RenderWindow& win) {
		Vector2u winSize = win.getSize();
		int leftInterval = winSize.x / 2;
		int topInterval = winSize.y / 5 * 2;
		start.updatePosition(leftInterval, topInterval);
		multplayer.updatePosition(leftInterval, topInterval + buttonHeight + seperate);
		exit.updatePosition(leftInterval, topInterval + 2 * buttonHeight + 2 * seperate);
		spTitle.setPosition(leftInterval, topInterval / 2);
		if (connSelect.isEnabled())
			connSelect.adjustViewportChange(win);
	}

	double calcPos(double val)
	{
		if (val > 10.0)
			val = 10.0;
		if (val < 0.0)
			return -(-(abs(val) - 10.0)*(abs(val) - 10.0) / 10.0 + 10.0);
		else
			return (-(val - 10.0)*(val - 10.0) / 10.0 + 10.0);
	}

	void updateLogic(RenderWindow& win) {
		if (connSelect.isEnabled())
			connSelect.updateLogic(win);
		else {
			if (start.isButtonPressed() && !Mouse::isButtonPressed(Mouse::Left))
				invokeHost();
			if (multplayer.isButtonPressed() && !Mouse::isButtonPressed(Mouse::Left))
				invokeConnect();
			if (exit.isButtonPressed() && !Mouse::isButtonPressed(Mouse::Left))
				invokeExit();
			start.updateLogic(win);
			multplayer.updateLogic(win);
			exit.updateLogic(win);
		}
	}

	void onRender(RenderWindow& win) {
		Vector2u winSize = win.getSize();
		int leftInterval = winSize.x / 2;
		int topInterval = winSize.y / 5;
		win.clear(Color(64, 64, 64));
		if (connSelect.isEnabled())
			connSelect.onRender(win);
		else {
			spTitle.setRotation3d(Vector3f(
				-calcPos((Mouse::getPosition(win).y - topInterval) / win.getView().getSize().y / 2)*35.0,
				calcPos((Mouse::getPosition(win).x - leftInterval) / win.getView().getSize().x / 2)*35.0, 0));
			win.draw(spTitle);
			start.onRender(win);
			multplayer.onRender(win);
			exit.onRender(win);
			fon.displayText(lowerLeft, win);
		}
		win.display();
	}

	void invokeHost() {
		cout << "sceneLaunch: Hosting!" << endl;
		connSelect.enable(ConnectionSelectLogic::listenMode);
	}

	void invokeConnect() {
		cout << "sceneLaunch: Connecting!" << endl;
		//sceneGame.setNetworkUsed(true);
		//changeScene(sceneGame);
		connSelect.enable(ConnectionSelectLogic::connectMode);
	}

	void invokeExit() {
		cout << "sceneLaunch: Byebye!" << endl;
		isAlive = false;
		stop();
	}

	const bool isReady() {
		return isAlive;
	}

private:
	bool isAlive;
	Button start, multplayer, exit;
	NFont::Text lowerLeft, lowerRight;
	Texture title;
	Sprite3d spTitle;
};

SceneLaunch sceneLaunch;
