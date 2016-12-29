#pragma once

//#define GLASS_BLUR_IN_COMPUTING
#define GLASS_BLUR_SAMPLE_SIZE 2

//#define USE_GLASS_TEXTURE
//#define USE_SPRITE3D

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SelbaWard/Sprite3d.hpp>

#include "main.hpp"
#include "sceneBase.hpp"

using namespace sf;
using namespace std;
using namespace sw;

void blurImage(Image& image, int density = 8)
{
	for (int i = 0; i < image.getSize().x; i++)
		for (int j = 0; j < image.getSize().y; j++)
		{
			int sumr = 0, sumg = 0, sumb = 0, suma = 0, size = 0;
			for (int k = -density; k <= density; k++)
				for (int l = -density; l <= density; l++)
				{
					if (i + k < 0 || i + k >= image.getSize().x || j + l < 0 || j + l >= image.getSize().y)
						continue;
					sumr += image.getPixel(i + k, j + l).r;
					sumg += image.getPixel(i + k, j + l).g;
					sumb += image.getPixel(i + k, j + l).b;
					suma += image.getPixel(i + k, j + l).a;
					size++;
				}
			image.setPixel(i, j, Color(sumr / size, sumg / size, sumb / size, suma / size));
		}
}

class MainMenu :public SceneBase
{
public:
	Texture background;
	Texture title;
	Texture glassTexture;
	bool isGlassComputed;
	static const int glassSizeWidth = 1080;
	static const int glassSizeHeight = 608;
	double overallLength;

	MainMenu() :isAlive(false) { isGlassComputed = false; }

	double calcPos(double val)
	{
		if (val > 10.0)
			val = 10.0;
		if (val < 0.0)
			return -(-(abs(val) - 10.0)*(abs(val) - 10.0) / 10.0 + 10.0);
		else
			return (-(val - 10.0)*(val - 10.0) / 10.0 + 10.0);
	}

	void computeGlassTexture()
	{
		Image glassImage;
		double dTmp;
		glassImage.create(glassSizeWidth, glassSizeHeight);
		overallLength = sqrt(glassSizeWidth*glassSizeWidth + glassSizeHeight*glassSizeHeight);
		for (int i = 0; i < glassImage.getSize().x; i++)
			for (int j = 0; j < glassImage.getSize().y; j++)
			{
				double ans;
				double disLT = sqrt(i*i + j*j), disRB = sqrt((glassSizeWidth - i)*(glassSizeWidth - i) + (glassSizeHeight - j)*(glassSizeHeight - j));
				ans = 0.0;
				ans += 10.0 - calcPos(disLT / overallLength * 2.0*10.0);
				ans += 10.0 - calcPos(disRB / overallLength * 2.0*10.0);
				if (i == 0 || j == 0 || i == glassSizeWidth - 1 || j == glassSizeHeight - 1)
					ans += 1.0;
				glassImage.setPixel(i, j, Color(186 + rand() % 8, 186 + rand() % 8, 186 + rand() % 8, ans * 12.8 + rand() % 8 + 24));
			}
#ifdef GLASS_BLUR_IN_COMPUTING
		blurImage(glassImage, GLASS_BLUR_SAMPLE_SIZE);
#endif // GLASS_BLUR_IN_COMPUTING
		glassTexture.loadFromImage(glassImage);
		glassSprite.setTexture(glassTexture);
		glassSprite.setOrigin(Vector2f(glassSizeWidth / 2.0f, glassSizeHeight / 2.0f));
	}

	void preLaunchInit()
	{
#ifdef USE_GLASS_TEXTURE
		computeGlassTexture();
#else
		Image glassImage;
		glassImage.loadFromFile("background.png");
		//blurImage(glassImage,4);
		glassTexture.loadFromImage(glassImage);
		glassTexture.setSmooth(true);
		glassSprite.setTexture(glassTexture);
		glassSprite.setOrigin(Vector2f(glassImage.getSize().x / 2.0f, glassImage.getSize().y / 2.0f));
#endif
	}

	void launch(RenderWindow& win)
	{
		sf::ContextSettings settings;
		settings.antialiasingLevel = 4;
		win.setFramerateLimit(60);
		glassSprite.setPosition(win.getView().getCenter());
		isAlive = true;
		startTimer.restart();
	}

	const bool handleEvent(RenderWindow& win, Event& event)
	{
		return false;
	}

	void updateLogic(RenderWindow& win)
	{
		if (!win.isOpen())
			isAlive = false;
	}

	void onRender(RenderWindow& win)
	{
		win.clear();
#ifdef USE_SPRITE3D
		glassSprite.setRotation3d(Vector3f(
			-calcPos((Mouse::getPosition(win).y - win.getView().getCenter().y) / win.getView().getSize().y / 2)*5.0*1.5,
			calcPos((Mouse::getPosition(win).x - win.getView().getCenter().x) / win.getView().getSize().x / 2)*5.0, 0));
#endif
		glassSprite.setPosition(win.getView().getSize().x / 2.0f, win.getView().getSize().y / 2.0f);
		if (startTimer.getElapsedTime() < seconds(4.0f))
			glassSprite.setScale(1 + 1 - calcPos(startTimer.getElapsedTime().asMicroseconds() / 4000000.0*10.0) / 10.0,
				1 + 1 - calcPos(startTimer.getElapsedTime().asMicroseconds() / 4000000.0*10.0) / 10.0);
		//else
			//glassSprite.setScale(1.0f, 1.0f);
#ifdef USE_GLASS_TEXTURE
		win.draw(glassSprite);
#else
		win.draw(glassSprite);
#endif
		win.display();
	}

	const bool isReady() { return isAlive; }
private:
#ifdef USE_SPRITE3D
	Sprite3d glassSprite;
#else
	Sprite glassSprite;
#endif
	bool isAlive;
	bool isFullscreen;
	Clock startTimer;
};

MainMenu mainMenu;
