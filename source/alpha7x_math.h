#pragma once
#include <glm/vec3.hpp>
inline bool sameHemiSphere(glm::vec3 wo, glm::vec3 wi)
{
	return (wo.z * wi.z) > 0.0f;
}