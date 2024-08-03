#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "geometry.h"

struct SLightSample
{
public:
	glm::vec3 wi;
	glm::vec3 L;
	float pdf;

	CInteraction iteraction;
};



class CLightSampleContext
{
public:
	CLightSampleContext(const CInteraction& iteraction)
		:position(iteraction.position)
		, normal(iteraction.norm) {};

	glm::vec3 position;
	glm::vec3 normal;
};

class CLight
{
public:
	virtual SLightSample SampleLi(CLightSampleContext sample_ctx, glm::vec2 u) = 0;
	glm::vec3 l_emit;
};

class CDiffuseAreaLight : public CLight
{
public:
	SLightSample SampleLi(CLightSampleContext sample_ctx, glm::vec2 u);
	
	inline glm::vec3 L(glm::vec3 normal,glm::vec3 w)
	{
		if (glm::dot(normal, w) < 0)
		{
			return glm::vec3(0, 0, 0);
		}
		assert(false);
		return scale * l_emit;
	}

	float scale;
	CTriangle triangle;
};

