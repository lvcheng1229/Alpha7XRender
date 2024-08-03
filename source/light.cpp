#include "light.h"

SLightSample CDiffuseAreaLight::SampleLi(CLightSampleContext sample_ctx, glm::vec2 u)
{
	SShapeSample shape_sample =  triangle.sample(u);
	if (shape_sample.pdf == 0 || glm::length(glm::abs(shape_sample.inter.position - sample_ctx.position)) == 0)
	{
		assert(false);
		return SLightSample();
	}

	glm::vec3 wi = glm::normalize(shape_sample.inter.position - sample_ctx.position);
	glm::vec3 le = L(shape_sample.inter.norm, -wi);

	return SLightSample{ wi,le, shape_sample.pdf, shape_sample.inter.position };
}
