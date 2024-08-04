#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/ext/scalar_constants.hpp>
#include "alpha7x_math.h"

inline float powerHeuristic(int nf, float wf, int ng, float wg)
{
    float f = nf * wf, g = ng * wg;
    if (std::isinf((f*f)))
        return 1;
    return (f * f) / ((f * f) + (g * g));
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

inline glm::vec2 sampleUniformDiskConcentric(glm::vec2 u)
{
    glm::vec2 u_offset = u * 2.0f - glm::vec2(1, 1);
    if (u_offset.x == 0.0 || u_offset.y == 0.0)
    {
        return glm::vec2(0.0,0.0);
    }

    float theta, r;
    if (std::abs(u_offset.x) > std::abs(u_offset.y))
    {
        r = u_offset.x;
        theta = glm::pi<float>() / 4.0 * (u_offset.y / u_offset.x);
    }
    else
    {
        r = u_offset.y;
        theta = glm::pi<float>() / 2.0 * (u_offset.x / u_offset.y);
    }
    return r * glm::vec2(std::cos(theta), std::sin(theta));
}

inline glm::vec3 sampleConsineHemisphere(glm::vec2 u)
{
    glm::vec2 d = sampleUniformDiskConcentric(u);
    float z = std::sqrt(std::max(0.0, 1.0 - d.x * d.x - d.y * d.y));
    return glm::vec3(d.x, d.y, z);
}

inline float cosineHemispherePDF(float cosTheta)
{
    return cosTheta / glm::pi<float>();
}

static constexpr int NSobolDimensions = 1024;
static constexpr int SobolMatrixSize = 52;
extern const uint32_t SobolMatrices32[NSobolDimensions * SobolMatrixSize];
extern const uint64_t VdCSobolMatrices[][SobolMatrixSize];
extern const uint64_t VdCSobolMatricesInv[][SobolMatrixSize];

inline uint32_t fastOwenScrambler(uint32_t v)
{
    
    uint32_t seed = std::hash<uint32_t>()(42u);
    v = reverseBits32(v);
    v ^= v * 0x3d20adea;
    v += seed;
    v *= (seed >> 16) | 1;
    v ^= v * 0x05526c56;
    v ^= v * 0x53a22864;
    return reverseBits32(v);
}

inline float sobolSample(int64_t a, int dimension, std::function<uint32_t(uint32_t)> randomizer)
{
    uint32_t v = 0;
    for (int i = dimension * SobolMatrixSize; a != 0; a >>= 1, i++)
        if (a & 1)
            v ^= SobolMatrices32[i];

    v = randomizer(v);
    return std::min(v * 0x1p-32f, float_one_minus_epsilon);
}


inline uint64_t sobolIntervalToIndex(uint32_t m, uint32_t frame, glm::uvec2 p)
{
    if (m == 0)
        return frame;

    const uint32_t m2 = m << 1;
    uint64_t index = uint64_t(frame) << m2;

    uint64_t delta = 0;
    for (int c = 0; frame; frame >>= 1, ++c)
        if (frame & 1)  
            delta ^= VdCSobolMatrices[m - 1][c];

    uint64_t b = (((uint64_t)((uint32_t)p.x) << m) | ((uint32_t)p.y)) ^ delta;

    for (int c = 0; b; b >>= 1, ++c)
        if (b & 1)
            index ^= VdCSobolMatricesInv[m - 1][c];

    return index;
}