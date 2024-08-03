#pragma once
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

inline bool sameHemiSphere(glm::vec3 wo, glm::vec3 wi)
{
	return (wo.z * wi.z) > 0.0f;
}

inline glm::vec3 faceForward(const glm::vec3& dir, const glm::vec3& ipt_normal)
{
	const glm::vec3 normal = ipt_normal;
	return glm::dot(dir, normal) < 0.0f ? normal : -(normal);
}

inline float sphericalTriangleArea(glm::vec3 dir_a, glm::vec3 dir_b, glm::vec3 dir_c)
{
	return glm::abs(2.0 * glm::atan(glm::dot(dir_a, glm::cross(dir_b, dir_c)), 1 + glm::dot(dir_a, dir_b) + glm::dot(dir_a, dir_c) + glm::dot(dir_b, dir_c)));
}