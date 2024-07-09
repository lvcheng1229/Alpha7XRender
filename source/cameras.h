#pragma once
#include "film.h"

class CCamera
{
public:
	inline CRGBFilm* getFilm() { return rgb_film; }
private:
	CRGBFilm* rgb_film;
};

class CPerspectiveCamera : public CCamera
{
public:
private:
};