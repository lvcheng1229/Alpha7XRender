#include "lightsampler.h"

CPowerLightSampler::CPowerLightSampler(std::vector<CLight*> input_lights)
{
	const int ipt_lt_num = input_lights.size();
	lights.resize(ipt_lt_num);
	for (int lt_idx = 0; lt_idx < ipt_lt_num; lt_idx++)
	{
		lights[lt_idx] = input_lights[lt_idx];
	}
}

CLight* CPowerLightSampler::Sample(float u)
{
	return nullptr;
}
