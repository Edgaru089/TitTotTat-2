#pragma once

#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "basicObject.hpp"
#include "NFont.hpp"

using namespace std;
using namespace sf;

class Button : public BasicObject
{
public:

	struct ColorType {
		ColorType() :outerRect(64, 64, 64, 128), overlapRect(76, 76, 76, 128) { }
		ColorType(Color outerRectColor, Color overlapRectColor) :outerRect(outerRectColor), overlapRect(overlapRectColor) {}
		Color outerRect, overlapRect;
	};

	Button() :BasicObject(), text(0.0, 0.0, "") {}

	void preLaunchInit() {
		ColorType type = ColorType();
		outerRect.setFillColor(type.outerRect);
		outerRect.setOutlineColor(Color(16, 16, 16, 128));
		outerRect.setOutlineThickness(2.0f);
		overlapRect.setFillColor(type.overlapRect);
		semiMaskRect.setFillColor(Color(229, 241, 251));
	}

	void preLaunchInit(ColorType type, string str, float sizeX, float sizeY) {
		outerRect.setFillColor(type.outerRect);
		outerRect.setOutlineColor(Color(16, 16, 16, 128));
		outerRect.setOutlineThickness(2.0f);
		overlapRect.setFillColor(type.overlapRect);
		semiMaskRect.setFillColor(Color(229, 241, 251));

		text.str = str;
		textBoundingRect = fon.getBoundingRect(text);
		text.originX = textBoundingRect.width / 2;
		text.originY = textBoundingRect.height / 2;
		textBoundingRect = fon.getBoundingRect(text);

		outerRect.setOrigin(sizeX / 2.0f, sizeY / 2.0f);
		outerRect.setSize(Vector2f(sizeX, sizeY));
		overlapRect.setOrigin((sizeX - 4.0f) / 2.0f, (sizeY - 4.0f) / 2.0f);
		overlapRect.setSize(Vector2f(sizeX - 4.0f, sizeY - 4.0f));
		semiMaskRect.setOrigin(sizeX / 2.0, sizeY / 2.0);
		semiMaskRect.setSize(Vector2f(sizeX, sizeY));
	}

	void updatePosition(float posX, float posY) {
		text.x = posX;
		text.y = posY;
		outerRect.setPosition(posX, posY);
		overlapRect.setPosition(posX, posY);
		semiMaskRect.setPosition(posX, posY);
		buttonBoundingRect = FloatRect(posX - outerRect.getSize().x / 2.0f, posY - outerRect.getSize().y / 2.0f, outerRect.getSize().x, outerRect.getSize().y);
	}

	void launch(RenderWindow& win, float posX, float posY) { //posX, posY: Center of Button.
		updatePosition(posX, posY);
		alive = true;
	}

	void updateLogic(RenderWindow& win) {
		if (buttonBoundingRect.contains(Vector2f(Mouse::getPosition(win).x, Mouse::getPosition(win).y)))
			isMouseOver = true;
		else
			isMouseOver = false;
		isPressed = false;
		if (isMouseOver&&win.hasFocus())
		{
			maskDensity += 4;
			if (maskDensity > 80)
				maskDensity = 80;
			if (Mouse::isButtonPressed(Mouse::Left))
			{
				isPressed = true;
				maskDensity = 100;
			}
		}
		else
		{
			maskDensity-=2;
			if (maskDensity < 0)
				maskDensity = 0;
		}
	}

	void onRender(RenderWindow& win) {
		semiMaskRect.setFillColor(Color(128, 128, 128, maskDensity));
		win.draw(outerRect);
		win.draw(overlapRect);
		win.draw(semiMaskRect);
		fon.displayText(text, win);
	}

	bool isButtonPressed() {
		return isPressed;
	}

	void increaseButtonDensity() {
		maskDensity = 100;
	}

private:
	float sizeX, sizeY;
	double offX, offY;
	Uuid id;
	bool alive;

	String str;
	FloatRect textBoundingRect, buttonBoundingRect;
	NFont::Text text;
	RectangleShape outerRect, overlapRect, semiMaskRect;
	int maskDensity; //0~40, 50
	bool isPressed, isMouseOver;
};

Button button[20];
int buttonCount = 20;
