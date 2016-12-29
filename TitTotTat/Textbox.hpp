#pragma once

#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "basicObject.hpp"
#include "NFont.hpp"

using namespace std;
using namespace sf;

class Textbox : public BasicObject
{
public:

	struct ColorType {
		ColorType() :outerRect(40, 40, 40, 128), overlapRect(64, 64, 64, 128) { }
		ColorType(Color outerRectColor, Color overlapRectColor) :outerRect(outerRectColor), overlapRect(overlapRectColor) {}
		Color outerRect, overlapRect;
	};

	Textbox() :BasicObject() {}

	void preLaunchInit() {
		ColorType type = ColorType();
		outerRect.setFillColor(type.outerRect);
		outerRect.setOutlineColor(Color(96, 96, 96, 128));
		outerRect.setOutlineThickness(2.0f);
		overlapRect.setFillColor(type.overlapRect);
	}

	void preLaunchInit(ColorType type, string hint, string title, float sizeX, float sizeY) {
		outerRect.setFillColor(type.outerRect);
		outerRect.setOutlineColor(Color(16, 16, 16, 128));
		outerRect.setOutlineThickness(1.0f);
		overlapRect.setFillColor(type.overlapRect);

		this->hint = NFont::Text(0, 0, hint);
		hintBoundingRect = fon.getBoundingRect(this->hint);
		this->title = NFont::Text(0, 0, title);
		titleBoundingRect = fon.getBoundingRect(this->title);

		this->content = NFont::Text(0, 0, "");
		contentBoundingRect = fon.getBoundingRect(this->content);

		outerRect.setSize(Vector2f(sizeX, sizeY));
		overlapRect.setSize(Vector2f(sizeX - 2.0f, sizeY - 2.0f));
	}

	void updatePosition(float posX, float posY) { //LEFT-TOP CORNER POSITION.
		offX = posX;
		offY = posY;
		hint.x = posX;
		hint.y = posY;
		title.x = posX;
		title.y = posY - titleBoundingRect.height - 2.0f;
		hintBoundingRect = fon.getBoundingRect(this->hint);
		titleBoundingRect = fon.getBoundingRect(this->title);
		outerRect.setPosition(posX, posY);
		overlapRect.setPosition(posX + 1.0f, posY + 1.0f);
		textboxBoundingRect = FloatRect(posX, posY, outerRect.getSize().x, outerRect.getSize().y);
		contentBoundingRect = fon.getBoundingRect(this->content);
		content.x = posX + 2;
		content.y = posY - (contentBoundingRect.height - outerRect.getSize().y) / 2;
		contentBoundingRect = fon.getBoundingRect(this->content);
	}

	void launch(RenderWindow& win, float posX, float posY) { //posX, posY: Center of Button.
		updatePosition(posX, posY);
		memset(inputDelay, 0, sizeof(inputDelay));
		backspaceDelay = 0;
		alive = true;
	}

	void updateLogicApperence(RenderWindow& win) {
		isMouseOver = false;
		if (textboxBoundingRect.contains(Vector2f(Mouse::getPosition(win).x, Mouse::getPosition(win).y)))
			isMouseOver = true;
		if (isMouseOver)
		{
			if (win.hasFocus()) {
				maskDensity += 6;
				if (maskDensity > 100)
					maskDensity = 100;
				if (Mouse::isButtonPressed(Mouse::Left))
					isSelected = true;
			}
		}
		else
		{
			maskDensity -= 6;
			if (maskDensity < 0)
				maskDensity = 0;
			if (Mouse::isButtonPressed(Mouse::Left)) {
				isSelected = false;
				maskDensity = 0;
			}
		}
		isEnter = false;
		if (isSelected) {
			isEnter = Keyboard::isKeyPressed(Keyboard::Return);
			maskDensity = 150;
		}
	}

	void updateLogicTextInput(RenderWindow& win) {
		if (!win.hasFocus() || !isSelected)
			return;
		if (backspaceDelay > 0)
			backspaceDelay--;
		for (int i = 0; i < 120; i++)
			if (inputDelay[i] > 0)
				inputDelay[i]--;
		if (Keyboard::isKeyPressed(Keyboard::BackSpace)) {
			if (backspaceDelay == 0) {
				backspaceDelay = 10;
				if (!content.str.empty())
					content.str.pop_back();
			}
		}
		else
			backspaceDelay = 0;
		contentBoundingRect = fon.getBoundingRect(this->content);
		content.x = offX + 2;
		content.y = offY - (contentBoundingRect.height - outerRect.getSize().y) / 2;
		if (contentBoundingRect.width + fon.ix[65].width * 2 >= textboxBoundingRect.width)
			return;
		//Keyboard::Key codes
		//0~25 A~Z
		//26~35 0,1,2,3,4,5,6,7,8,9
		//57 Space
		//56 Dash"-"  56+SHIFT "_"
		//A~Z Alphabet
		for (int i = 0; i <= 25; i++) {
			if (Keyboard::isKeyPressed((Keyboard::Key)i)) {
				if (inputDelay[i] == 0) {
					inputDelay[i] = 15;
					content.str += (char)(i + ((Keyboard::isKeyPressed(Keyboard::LShift) || Keyboard::isKeyPressed(Keyboard::RShift)) ? 65 : 97));
				}
			}
			else
				inputDelay[i] = 0;
		}
		//Numbers
		for (int i = 26; i <= 35; i++) {
			if (Keyboard::isKeyPressed((Keyboard::Key)i)) {
				if (inputDelay[i] == 0) {
					inputDelay[i] = 15;
					content.str += (char)(i + 48 - 26);
				}
			}
			else
				inputDelay[i] = 0;
		}
		//Dash && Underdash
		if (Keyboard::isKeyPressed(Keyboard::Dash)) {
			if (inputDelay[Keyboard::Dash] == 0) {
				inputDelay[Keyboard::Dash] = 15;
				if (Keyboard::isKeyPressed(Keyboard::LShift) || Keyboard::isKeyPressed(Keyboard::RShift))
					content.str += '_';
				else
					content.str += '-';
			}
		}
		else
			inputDelay[Keyboard::Dash] = 0;
		//Dot "."
		if (Keyboard::isKeyPressed(Keyboard::Period)) {
			if (inputDelay[Keyboard::Period] == 0) {
				inputDelay[Keyboard::Period] = 15;
				content.str += '.';
			}
		}
		else
			inputDelay[Keyboard::Period] = 0;
		//Space
		if (Keyboard::isKeyPressed(Keyboard::Space)) {
			if (inputDelay[Keyboard::Space] == 0) {
				inputDelay[Keyboard::Space] = 15;
				content.str += ' ';
			}
		}
		else
			inputDelay[Keyboard::Space] = 0;
		//Cleanup
		contentBoundingRect = fon.getBoundingRect(this->content);
		content.x = offX + 2;
		content.y = offY - (contentBoundingRect.height - outerRect.getSize().y) / 2;
	}

	void updateLogic(RenderWindow& win) {
		updateLogicApperence(win);
		updateLogicTextInput(win);
	}

	void onRender(RenderWindow& win) {
		outerRect.setOutlineColor(Color(96 + maskDensity, 96 + maskDensity, 96 + maskDensity, 128));
		win.draw(outerRect);
		win.draw(overlapRect);
		if (isMouseOver) {
			hint.x = Mouse::getPosition().x;
			hint.y = Mouse::getPosition().y;
			hint.originX = hint.originY = 0.0f;
			fon.displayText(hint, win);
		}
		NFont::Text text = content;
		if (isSelected)
			text.str += '_';
		fon.displayText(title, win);
		fon.displayText(text, win);
	}

	bool isEnterPressed() {return isEnter;}

	string getString() {return content.str;}
	void setString(string str) { content.str = str; }

	Vector2f getSize() { return Vector2f(sizeX, sizeY); }

private:
	float sizeX, sizeY;
	double offX, offY;
	Uuid id;
	bool alive;

	String str;
	FloatRect hintBoundingRect, titleBoundingRect, contentBoundingRect, textboxBoundingRect;
	NFont::Text hint, title, content;
	RectangleShape outerRect, overlapRect;
	int maskDensity; //0~40, 50
	bool isEnter, isMouseOver, isSelected;
	int inputDelay[120], backspaceDelay;
};