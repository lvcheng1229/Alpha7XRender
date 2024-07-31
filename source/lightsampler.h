#pragma once
#include <vector>
#include "light.h"

struct SSampledLight
{
	CLight* light;
	float pmf; //probability mass function
};

class CLightSampler
{
public:
	virtual SSampledLight Sample(float u) = 0;
};

class CPowerLightSampler : public CLightSampler
{
public:
	CPowerLightSampler(std::vector<CLight*> input_lights);
	SSampledLight Sample(float u);
private:
	std::vector<CLight*>lights;
	std::vector<float> light_cdf;
	float cdf_sum;
};