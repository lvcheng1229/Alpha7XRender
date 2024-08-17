#include "light.h"
#include "alpha7x_math.h"

SLightSample CDiffuseAreaLight::SampleLi(CLightSampleContext sample_ctx, glm::vec2 u)
{
	SShapeSample shape_sample = triangle.sample(SShapeSampleDesc{ sample_ctx.position }, u);
	if (shape_sample.pdf == 0 || glm::length(glm::abs(shape_sample.inter.position - sample_ctx.position)) == 0)
	{
		assert(false);
		return SLightSample();
	}

	glm::vec3 wi = glm::normalize(shape_sample.inter.position - sample_ctx.position);
	glm::vec3 le = L(shape_sample.inter.norm, -wi);

	return SLightSample{ wi,le, shape_sample.pdf, shape_sample.inter.position };
}

glm::vec3 CDiffuseAreaLight::sampleLe(const glm::vec2& u1, const glm::vec2& u2, CRay& ray, glm::vec3& normal_light, float& pdf_pos, float& pdf_dir)
{
	pdf_dir = 0;

	SShapeSample shape_sample = triangle.sample(u1);
	glm::vec3 out_dir = sampleConsineHemisphere(u2);

	CTangentBasis tangent_basis = CTangentBasis::fromZ(shape_sample.inter.norm);
	out_dir = tangent_basis.fromLocal(out_dir);
	glm::vec3 le = L(shape_sample.inter.norm, out_dir);
	
	ray = CRay(shape_sample.inter.position, out_dir);
	normal_light = shape_sample.inter.norm;
	pdf_pos = shape_sample.pdf;
	pdf_dir = cosineHemispherePDF(out_dir.z);
	return le;
}
