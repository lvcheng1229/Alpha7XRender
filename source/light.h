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
	SLightSample SampleLi(CLightSampleContext sample_ctx, glm::vec2 u);
	glm::vec3 L;
};

class CDiffuseAreaLight : public CLight
{
public:
	
};

