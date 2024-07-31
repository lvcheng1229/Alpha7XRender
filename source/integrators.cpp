#include "integrators.h"
#include "parallel.h"
#include "sampling.h"

SShapeInteraction CIntegrator::intersect(CRay ray) const
{
	return accelerator->intersection(ray);
}


CPathIntegrator::CPathIntegrator(int max_depth, CPerspectiveCamera* camera, CSampler* sampler, CAccelerator* ipt_accelerator, std::vector<CLight*> lights)
	: CIntegrator(ipt_accelerator)
	, max_depth(max_depth)
	, camera(camera)
	, sampler_prototype(sampler)
{
}

void CPathIntegrator::render()
{
	CRGBFilm* rgb_film = camera->getFilm();
	const glm::u32vec2 image_size = rgb_film->getImageSize();
	const int image_area = image_size[0] * image_size[1];
	
	int spp = sampler_prototype->getSamplersPerPixel();

	for (int spp_idx = 0; spp_idx < spp; spp_idx++)
	{
		glm::u32vec2 bound_min = glm::u32vec2(0, 0);
		glm::u32vec2 bound_max = image_size;
		for (glm::uint32 pixel_x = bound_min.x; pixel_x < bound_max.x; pixel_x++)
		{
			for (glm::uint32 pixel_y = bound_min.y; pixel_y < bound_max.y; pixel_y++)
			{
				glm::u32vec2 pix_pos = glm::u32vec2(pixel_x, pixel_y);
				CSampler* sampler = sampler_prototype; // todo: TLS
				evaluatePixelSample(pix_pos, sampler);
			}
		}

		//parallelFor2D(glm::u32vec2(0, 0), image_size, [&](glm::u32vec2 bound_min, glm::u32vec2 bound_max) {
		//});
	}


}

void CPathIntegrator::evaluatePixelSample(glm::vec2 pixel_pos, CSampler* sampler)
{
	glm::vec3 ray_origin = camera->getCameraPos();
	glm::vec3 ray_direction = camera->getPixelRayDirection(pixel_pos + sampler->getPixel2D());
	CRay ray(ray_origin, ray_direction);
	glm::vec3 L = Li(ray, sampler);


}

glm::vec3 CPathIntegrator::Li(CRay ray, CSampler* sampler)
{
	glm::vec3 radiance(0, 0, 0);
	glm::vec3 beta(1, 1, 1);

	int depth = 0;
	float eta_scale = 1.0f;
	while (true)
	{
		SShapeInteraction sp_interaction = intersect(ray);

		if (sp_interaction.hit_t == std::numeric_limits<float>::max())
		{
			break;
		}

		CSurfaceInterraction& sf_interaction = sp_interaction.sface_interaction;
		CBSDF bsdf = sf_interaction.getBSDF();

		if (depth == max_depth)
		{
			break;
		}

		// if no specular surface
		// sample direct illumination
		glm::vec3 radiance_direct = SampleLd(sf_interaction, &bsdf, sampler);
		radiance += radiance_direct * beta;

		// sample BSDF and generate a new direction
		glm::vec3 wo = -ray.direction;
		float u = sampler->get1D();
		SBSDFSample bsdf_sample = bsdf.sample_f(wo,u, sampler->get2D());

		// if xx

		// update path thoughput
		beta *= bsdf_sample.f * glm::abs(glm::dot(bsdf_sample.wi, sf_interaction.norm)) * bsdf_sample.pdf;

		if (bsdf_sample.isTransmission())
		{
			eta_scale *= bsdf_sample.eta;
		}

		ray = sf_interaction.spawnRay(bsdf_sample.wi);

		glm::vec3 rr_beta = beta * eta_scale;
		float max_comp = glm::max(glm::max(rr_beta.x, rr_beta.y), rr_beta.z);
		if (max_comp < 1 && depth > 1)
		{
			float q = glm::max(0.0f, 1.0f - max_comp);
			if (sampler->get1D() < q)
			{
				break;
			}
			beta /= 1 - q;
		}

		depth++;
	}

	return glm::vec3(0, 0, 0);
}

glm::vec3 CPathIntegrator::SampleLd(const CSurfaceInterraction& sf_interaction, const CBSDF* bsdf, CSampler* sampler)
{
	glm::vec3 radiance;

	CLightSampleContext sample_ctx(sf_interaction);

	float u = sampler->get1D();
	SSampledLight sampled_light = light_sampler->Sample(u);
	CLight* light = sampled_light.light;
	if (!light)
	{
		return glm::vec3(0, 0, 0);
	}

	glm::vec2 u_light = sampler->get2D();
	SLightSample light_sample = light->SampleLi(sample_ctx, u_light);
	// if (xxx)

	glm::vec3 wo = sf_interaction.wo;
	glm::vec3 wi = light_sample.wi;
	glm::vec3 f = bsdf->f(wo, wi)* glm::abs(glm::dot(wi, sf_interaction.norm));

	// if unoccluded

	float p_light = sampled_light.pmf* light_sample.pdf;
	// if is delta light
	// else

	{
		float p_bsdf = bsdf->pdf(wo, wi);

		// mis
		float w_l = powerHeuristic(1, p_light, 1, p_bsdf);
		return w_l * light_sample.L * f / p_light;
	}


	return radiance;
}

