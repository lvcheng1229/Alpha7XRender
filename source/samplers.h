#pragma once
#include <glm/vec2.hpp>
#include "alpha7x_math.h"
#include "sampling.h"

class CSampler
{
public:
	CSampler(int ipt_spp) :samplers_per_pixel(ipt_spp) {};

	virtual void initPixelSample(glm::u32vec2 pos, int sample_index, int dim = 0) = 0;
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
	CSobelSampler(int ipt_spp,glm::ivec2 full_resolution) :CSampler(ipt_spp) 
	{
		scale = roundUpPow2(std::max(full_resolution.x, full_resolution.y));
	};

	inline void initPixelSample(glm::u32vec2 pos, int sample_index, int dim = 0)
	{
		pixel = pos;
		dimension = std::max(2, dim);
		sobol_index = sobolIntervalToIndex(log2Int(scale), sample_index, pixel);
	};
	virtual float get1D() // 0 - 1
	{
		if (dimension >= NSobolDimensions)
		{
			dimension = 2;
		}
		return sampleDimension(dimension++);
	};

	virtual glm::vec2 get2D() // 0 - 1
	{ 
		if (dimension + 1 >= NSobolDimensions)
		{
			dimension = 2;
		}
		glm::vec2 u(sampleDimension(dimension), sampleDimension(dimension + 1));
		dimension += 2;
		return u;
	};

	virtual glm::vec2 getPixel2D()
	{
		auto no_randomizer = [](uint32_t ipt)->uint32_t {return ipt; };
		glm::vec2 u(sobolSample(sobol_index, 0, no_randomizer), sobolSample(sobol_index, 1, no_randomizer));
		for (int dim = 0; dim < 2; ++dim)
		{
			u[dim] = glm::clamp(u[dim] * scale - pixel[dim], 0.0f, float_one_minus_epsilon);
		}
		return u;
	};

private:

	float sampleDimension(int dimension)const
	{
		return sobolSample(sobol_index, dimension, fastOwenScrambler);
	}

	float scale;
	glm::u32vec2 pixel;
	int dimension;
	int64_t sobol_index;
};
