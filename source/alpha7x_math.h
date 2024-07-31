#pragma once
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

inline bool sameHemiSphere(glm::vec3 wo, glm::vec3 wi)
{
	return (wo.z * wi.z) > 0.0f;
}

inline glm::vec3 faceForward(const glm::vec3& dir, const glm::vec3& ipt_normal)
{
	const glm::vec3 normal = ipt_normal;
	return glm::dot(dir, normal) < 0.0f ? normal : -(normal);
}