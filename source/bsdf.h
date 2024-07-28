#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "bxdf.h"

class CBSDF
{
public:
	glm::vec3 f(glm::vec3 wo, glm::vec3 wi)const
	{
		assert(false);
		return glm::vec3(0, 0, 0);
	}

	float pdf(glm::vec3 wo, glm::vec3 wi)const
	{
		return 0;
	}

	SBSDFSample sample_f(glm::vec3 wo, float u, glm::vec2 u2)
	{
		return SBSDFSample{};
	}

private:
	glm::vec3 normal;
	CBxDF* bxdf;
};
