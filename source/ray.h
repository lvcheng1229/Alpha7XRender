#pragma once
#include <glm/vec3.hpp>

class CRay
{
public:
	CRay() = default;
	CRay(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction) {}

	glm::vec3 origin;
	glm::vec3 direction;
private:

};