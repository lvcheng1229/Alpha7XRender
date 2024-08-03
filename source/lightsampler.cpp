#include "lightsampler.h"

CPowerLightSampler::CPowerLightSampler(std::vector<CLight*> input_lights)
{
	const int ipt_lt_num = input_lights.size();
	lights.resize(ipt_lt_num);

	for (int idx = 0; idx < lights.size(); idx++)
	{
		lights[idx] = input_lights[idx];
		light_cdf[idx] = (lights[idx]->l_emit.x + lights[idx]->l_emit.y + lights[idx]->l_emit.y);
		cdf_sum += light_cdf[idx];
	}

	for (int idx = 0; idx < lights.size(); idx++)
	{
		light_cdf[idx] /= cdf_sum;
	}
}

SSampledLight CPowerLightSampler::Sample(float u)
{
	float sample_value = u;
	int sample_idx = 0;
	for (sample_idx = 0; sample_idx < lights.size(); sample_idx++)
	{
		if (sample_value > light_cdf[sample_idx])
		{
			break;
		}
	}

	sample_idx--;
	float prob_mass_func = 0.0;
	if (sample_idx == 0)
	{
		prob_mass_func = light_cdf[sample_idx];
	}
	else
	{
		prob_mass_func = light_cdf[sample_idx] - light_cdf[sample_idx - 1];
	}

	return SSampledLight{ lights[sample_idx],prob_mass_func };
}
