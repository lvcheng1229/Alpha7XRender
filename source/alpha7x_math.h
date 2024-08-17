#pragma once
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

static constexpr float float_one_minus_epsilon = 0x1.fffffep-1;

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

inline int log2Int(uint32_t v)
{
	unsigned long lz = 0;
	if (_BitScanReverse(&lz, v))
		return lz;
	return 0;
}

inline constexpr int32_t roundUpPow2(int32_t v) 
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	return v + 1;
}

inline uint32_t reverseBits32(uint32_t n) 
{
	n = (n << 16) | (n >> 16);
	n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
	n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
	n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
	n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
	return n;
}

inline void coordinateSystem(glm::vec3 v1, glm::vec3& v2, glm::vec3& v3)
{
	float sign = v1.z > 0 ? 1.0 : -1.0;
	float a = (-1.0) / (sign + v1.z);
	float b = v1.x * v1.y * a;
	v2 = glm::vec3(1 + sign * a * (v1.x * v1.x), sign * b, -sign * v1.x);
	v3 = glm::vec3(b, sign + a * (v1.y * v1.y), -v1.y);
}

class CTangentBasis
{
public:
	CTangentBasis() :x(glm::vec3(1, 0, 0)), y(glm::vec3(0, 1, 0)), z(glm::vec3(0, 0, 1)) {};
	CTangentBasis(glm::vec3 x, glm::vec3 y, glm::vec3 z) :x(x), y(y), z(z) {};

	static CTangentBasis fromZ(glm::vec3 ipt_z)
	{
		glm::vec3 opt_x, opt_y;
		coordinateSystem(ipt_z, opt_x, opt_y);
		return CTangentBasis(opt_x, opt_y, ipt_z);
	}


	glm::vec3 toLocal(glm::vec3 v)const
	{
		return glm::vec3(glm::dot(v,x), glm::dot(v, y), glm::dot(v, z));
	}

	glm::vec3 fromLocal(glm::vec3 v)const
	{
		return glm::vec3(v.x * x + v.y * y + v.z * z);
	}

	glm::vec3 x, y, z;
};
