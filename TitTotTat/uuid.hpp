#pragma once

#include <string>
#include "main.hpp"

int curUuidPos;

class Uuid  //in form of "1~99999"
{
public:

	Uuid() {}
	Uuid(int id) :uuid(id) {}

	static void init() { curUuidPos = 1; }
	static Uuid getUuid() { return Uuid(curUuidPos++); }

	const bool operator== (Uuid x) { return uuid == x.uuid; }

private:
	int uuid;

};

