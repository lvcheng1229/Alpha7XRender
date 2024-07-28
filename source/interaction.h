#pragma once
#include "glm/vec3.hpp"
#include "ray.h"
#include "material.h"
#include "bsdf.h"

class CSurfaceInterraction
{
public:
	glm::vec3 norm;
	glm::vec3 wo;
	CMaterial* material;

	inline CRay spawnRay(glm::vec3 direction)
	{
		return CRay(glm::vec3(0, 0, 0), direction);
	};

	CBSDF getBSDF();
};