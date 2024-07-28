#pragma once
#include <glm/vec2.hpp>

class CRGBFilm
{
public:
	CRGBFilm(glm::u32vec2 ipt_img_sz)
		:image_size(ipt_img_sz) {};
	inline glm::u32vec2 getImageSize()const { return image_size; }
private:
	glm::u32vec2 image_size;
};