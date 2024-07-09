#pragma once
#include <glm/vec2.hpp>

class CRGBFilm
{
public:
	inline glm::u32vec2 getImageSize()const { return image_size; }
private:
	glm::u32vec2 image_size;
};