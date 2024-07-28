#pragma once
#include <vector>
#include "light.h"

struct SSampledLight
{
	CLight* light;
	float p;
};

class CLightSampler
{
public:
	virtual CLight* Sample(float u) = 0;
};

class CPowerLightSampler : public CLightSampler
{
public:
	CPowerLightSampler(std::vector<CLight*> input_lights);
	CLight* Sample(float u);
private:
	std::vector<CLight*>lights;
};