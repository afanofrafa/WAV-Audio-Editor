#include "Selection.h"

Selection::Selection()
{
	this->isActive = false;
	this->start = { 0, 0};
	this->end = { 0, 0 };
}

Selection::Selection(bool isActive, POINT start, POINT end)
{
	this->isActive = isActive;
	this->start = start;
	this->end = end;
}

bool Selection::get_isActive()
{
	return this->isActive;
}

void Selection::set_isActive(bool isActive)
{
	this->isActive = isActive;
}

bool Selection::get_isDown()
{
	return this->isDown;
}

void Selection::set_isDown(bool isDown)
{
	this->isDown = isDown;
}

POINT Selection::get_start()
{
	return this->start;
}

void Selection::set_start(POINT start)
{
	this->start = start;
}

POINT Selection::get_end()
{
	return this->end;
}

void Selection::set_end(POINT end)
{
	this->end = end;
}

unsigned short Selection::get_left()
{
	return min(start.x, end.x);
}

unsigned short Selection::get_right()
{
	return max(start.x, end.x);
}

unsigned short Selection::get_top()
{
	return min(start.y, end.y);
}

unsigned short Selection::get_bottom()
{
	return max(start.y, end.y);
}

Selection::~Selection()
{
}
