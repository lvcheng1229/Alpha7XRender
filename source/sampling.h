#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

inline float powerHeuristic(int nf, float wf, int ng, float wg)
{
	assert(false);
	return 0;
}

inline glm::vec3 sampleUniformTriangle(glm::vec2 u) 
{
    float b0, b1;
    if (u[0] < u[1]) {
        b0 = u[0] / 2;
        b1 = u[1] - b0;
    }
    else {
        b1 = u[1] / 2;
        b0 = u[0] - b1;
    }
    return { b0, b1, 1 - b0 - b1 };
}