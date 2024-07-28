#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

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
};

class CLight
{
public:
	SLightSample SampleLi(CLightSampleContext sample_ctx,glm::vec2 u);
	float p;
};

class CDiffuseAreaLight : public CLight
{
public:
};

