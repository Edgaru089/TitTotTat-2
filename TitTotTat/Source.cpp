#include "Overall.hpp"

#ifdef TEST_INSTED_OF_MAIN

#include <cstdlib>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "main.hpp"
#include "Textbox.hpp"

using namespace std;
using namespace sf;

float offX, offY;
Vector2i screenSize = Vector2i(1280, 700);
const float g = 0.98f, aclc = 2.0f, dc = 0.02f;
const float pi = 3.14;
float speedX = 0.0f, speedY = 0.0f; //+speedX: Towards right. +speedY: Faces down.
float radius = 15.0f;

bool isFullscreen = false;
ContextSettings settings = ContextSettings(0u, 0u, 4u, 1u, 1u, 0u);

CircleShape circle;
//RenderWindow win;

Textbox box;

void preInit() {
	fon.loadFromBinary("ibmank0.bin");

	circle.setFillColor(Color::White);
	circle.setOrigin(radius, radius);
	circle.setRadius(radius);
	circle.setOutlineThickness(0.0f);
	circle.setPosition(offX = 1280 / 2, offY = 700 / 2);
	circle.setPointCount(12);

	box.preLaunchInit(Textbox::ColorType(), "HINT HINT", "TEXTBOX TITLE", 300.0f, 24.0f);
}

void launchInit() {
	win.create(VideoMode(1280, 700), "Window", Style::Close | Style::Titlebar | Style::Resize, settings);
	win.setFramerateLimit(60);

	box.launch(win, 100.0f, 100.0f);
}

void dispatchEvent() {
	Event event;
	while (win.pollEvent(event)) {
		if (event.type == Event::Closed)
			win.close();
		else if (event.type == Event::Resized) {
			win.setView(View(FloatRect(0, 0, event.size.width, event.size.height)));
		}
		else if (event.type == Event::KeyPressed)
			if (event.key.code == Keyboard::F11) {
				if (!isFullscreen) {
					win.create(VideoMode::getDesktopMode(), "Window", Style::Fullscreen, settings);
					win.setVerticalSyncEnabled(true);
					isFullscreen = true;
				}
				else {
					win.create(VideoMode(1280, 700), "Window", Style::Close | Style::Titlebar | Style::Resize, settings);
					win.setVerticalSyncEnabled(true);
					isFullscreen = false;
				}
				screenSize = Vector2i(win.getSize().x, win.getSize().y);
			}
			else if (event.key.code == Keyboard::Space)
				speedY -= 10 * aclc;
	}
}

void gameLoop() {
	if (Keyboard::isKeyPressed(Keyboard::Left))
		speedX -= aclc / 2;
	if (Keyboard::isKeyPressed(Keyboard::Right))
		speedX += aclc / 2;
	if (isSame(offY + radius, (double)win.getSize().x))
		speedX -= dc*speedX;
	else
		speedY += g;
	if (speedX<0.0 && speedX>-0.05)
		speedX = 0.0;
	else if (speedX > 0.0&&speedX < 0.05)
		speedX = 0.0;
	if (speedY<0.0 && speedY>-0.05)
		speedY = 0.0;
	else if (speedY > 0.0&&speedY < 0.05)
		speedY = 0.0;
	offX += speedX;
	offY += speedY;
	if (offY - radius < 0.0f || offY + radius >(double)win.getSize().y) {
		if (offY - radius < 0.0f)
			offY = radius;
		if (offY + radius > (double)win.getSize().y)
			offY = (double)win.getSize().y - radius;
		speedY = 0.0f;
	}
	if (offX - radius < 0.0f || offX + radius >(double)win.getSize().x) {
		if (offX - radius < 0.0f)
			offX = radius;
		if (offX + radius > (double)win.getSize().x)
			offX = (double)win.getSize().x - radius;
		speedX = 0.0f;
	}
	circle.setPosition(offX, offY);
	if (box.isEnterPressed() && !Keyboard::isKeyPressed(Keyboard::Return)) {
		//Cout << "Textbox Responce: " << box.getString() << endl;
		Keyboard::setVirtualKeyboardVisible(true);
	}
	box.updateLogic(win);

	Vector2f center = Vector2f(win.getSize().x / 2.0f, win.getSize().y / 2.0f);
	Vector2f pos = Vector2f(offX, offY);
	RectangleShape rect;
	CircleShape csh;
	rect.setFillColor(Color(128, 128, 128));
	rect.setPosition(center);
	rect.setSize(Vector2f(getDis(center.x, center.y, pos.x, pos.y), 1));
	csh.setRadius(getDis(center.x, center.y, pos.x, pos.y));
	csh.setOrigin(getDis(center.x, center.y, pos.x, pos.y), getDis(center.x, center.y, pos.x, pos.y));
	csh.setPosition(center);
	csh.setOutlineThickness(1.0f);
	csh.setOutlineColor(Color(128, 128, 128));
	csh.setFillColor(Color::Transparent);
	//Cout << atan((pos.y - center.y) / (pos.x - center.x)) * 180.0 / 3.14159 << endl;
	double deg = atan((pos.y - center.y) / (pos.x - center.x)) * 180.0 / 3.14159;
	if (pos.x < center.x)
		deg += 180.0;
	rect.setRotation(deg);

	win.clear();
	box.onRender(win);
	win.draw(rect);
	win.draw(csh);
	win.draw(circle);
	win.display();

}

int main(int argc, char* argv[]) {
	sleep(milliseconds(500));
	preInit();
	launchInit();
	while (win.isOpen()) {
		dispatchEvent();
		gameLoop();
	}
	return EXIT_SUCCESS;
}

#endif
