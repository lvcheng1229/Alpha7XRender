#pragma once
#include <glm/vec2.hpp>

class CSampler
{
public:
	CSampler(int ipt_spp) :samplers_per_pixel(ipt_spp) {};

	virtual float get1D() = 0;
	virtual glm::vec2 get2D() = 0;
	virtual glm::vec2 getPixel2D() = 0;

	inline int getSamplersPerPixel()const { return samplers_per_pixel; }
private:
	int samplers_per_pixel;
};


class CSobelSampler : public CSampler
{
public:
	CSobelSampler(int ipt_spp) :CSampler(ipt_spp) {};

	virtual float get1D() { return 0; };
	virtual glm::vec2 get2D() { return glm::vec2(0, 0); };
	virtual glm::vec2 getPixel2D() { return glm::vec2(0, 0); };
};
