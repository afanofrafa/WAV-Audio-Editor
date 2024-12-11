#pragma once
#include <windows.h>
class Selection
{
private:
	bool isActive;
	bool isDown;
	POINT start;
	POINT end;
public:
	Selection();
	Selection(bool isActive, POINT start, POINT end);

	bool get_isActive();
	void set_isActive(bool isActive);

	bool get_isDown();
	void set_isDown(bool isDown);

	POINT get_start();
	void set_start(POINT start);

	POINT get_end();
	void set_end(POINT end);

	unsigned short get_left();
	unsigned short get_right();
	unsigned short get_top();
	unsigned short get_bottom();

	~Selection();
};

