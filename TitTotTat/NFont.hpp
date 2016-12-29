#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <string>
#include <fstream>
#include <cstring>

using namespace sf;
using namespace std;

#define abs(a) ((a)>0?(a):(-(a)))

class NFont
{
public:

	struct Text
	{
		Text()
		{
			x = y = 0;
			str = "";
			color = Color::Black;
			hasShadow = true;
			linespace = 1;
			charspace = 2;
			zoom = 2;
		}

		Text(int X, int Y, string Str, Color col = Color::White, bool HasShadow = true,
			float Zoom = 1.0f, int Linespace = 1, int Charspace = 1, float OriginX = 0, float OriginY = 0)
			:x(X), y(Y), str(Str), color(col), hasShadow(HasShadow),
			linespace(Linespace), charspace(Charspace), zoom(Zoom), originX(OriginX), originY(OriginY) {}

		int x, y;
		string str;
		Color color;
		bool hasShadow;
		int linespace, charspace;
		float zoom;
		float originX, originY;
	};

	NFont()
	{
		memset(exist, 0, sizeof(exist));
	}

	bool loadFromFile(string infoFilename, string TextureFilename)
	{
		whole.loadFromFile(TextureFilename);
		ifstream info(infoFilename.c_str());
		int a, b, c, d, e;
		int count = 0;
		while (info >> a >> b >> c >> d >> e)
		{
			if (a > 255 || a < 0 || exist[a])
				continue;
			count++;
			height = c;
			exist[a] = true;
			ix[a] = IntRect(d, e, b, c);
		}
		if (count < 255)
			return false;
		else
			return true;
	}

	bool loadFromBinary(string filename)
	{
		FILE* bin = fopen(filename.c_str(), "rb");
		if (bin == NULL)
			return false;
		char c;
		height = 16;
		int width = 8;
		Image buff;
		buff.create(width * 256, height, Color(0, 0, 0, 0));
		for (int i = 0; i < 256; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				c = fgetc(bin);
				for (int k = 0; k < 8; k++)
					buff.setPixel(i * 8 + k, j, ((int)c &(1 << (7 - k))) ? Color::White : Color::Transparent);
				exist[i] = true;
				ix[i] = IntRect(i * 8, 0, width, height);
			}
		}
		whole.loadFromImage(buff);
	}

	bool loadFromText(string filename) {
		FILE* bin = fopen(filename.c_str(), "r");
		if (bin == NULL)
			return false;
		int c;
		height = 16;
		int width = 8;
		Image buff;
		buff.create(width * 256, height, Color(0, 0, 0, 0));
		for (int i = 0; i < 256; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				c = fgetc(bin);
				for (int k = 0; k < 8; k++)
					buff.setPixel(i * 8 + k, j, (c &(1 << (7 - k))) >> (7 - k) ? Color::White : Color::Transparent);
				exist[i] = true;
				ix[i] = IntRect(i * 8, 0, width, height);
			}
		}
		whole.loadFromImage(buff);
	}

	void displayText(NFont::Text txt, RenderTarget& win)
	{
		//int x,int y,string str,Color color,bool hasShadow,int linespace,int charspace,float zoom=2
		Color shdColor(0, 0, 0, 128);
		int shdSpace = 1;
		int curx, cury;
		curx = txt.x - txt.originX;
		cury = txt.y - txt.originY;
		for (int i = 0; i < txt.str.length(); i++)
		{
			if (txt.str[i] == '\n')
			{
				curx = txt.x;
				cury += height*txt.zoom;
				cury += txt.linespace*txt.zoom;
			}
			else if (txt.str[i] == '\t')
			{
				curx += 4 * (ix[32].width)*txt.zoom;
				curx += txt.charspace*txt.zoom;
			}
			else
			{
				Sprite sp(whole);
				sp.setTextureRect(ix[(int)txt.str[i]]);
				sp.setPosition(curx, cury);
				sp.setColor(txt.color);
				sp.setScale(txt.zoom, txt.zoom);

				//Shadow first.
				if (txt.hasShadow)
				{
					sp.move(shdSpace*txt.zoom, shdSpace*txt.zoom);
					sp.setColor(shdColor);
					win.draw(sp);
				}

				sp.setPosition(curx, cury);
				sp.setColor(txt.color);
				win.draw(sp);

				curx += ix[(int)txt.str[i]].width*txt.zoom;
				curx += txt.charspace*txt.zoom;
			}
		}
	}

	FloatRect getBoundingRect(NFont::Text txt)
	{
		int curx, cury;
		curx = txt.originX;
		cury = txt.originY;
		for (int i = 0; i < txt.str.length(); i++)
		{
			if (txt.str[i] == '\n')
			{
				curx = txt.x;
				cury += height*txt.zoom;
				cury += txt.linespace*txt.zoom;
			}
			else if (txt.str[i] == '\t')
			{
				curx += 4 * (ix[32].width)*txt.zoom;
				curx += txt.charspace*txt.zoom;
			}
			else
			{
				curx += ix[(int)txt.str[i]].width*txt.zoom;
				curx += txt.charspace*txt.zoom;
			}
		}
		return FloatRect(txt.x, txt.y, curx, cury + height);
	}

	Texture whole;
	IntRect ix[255];
	bool exist[255];
	int height;
};

NFont fon;