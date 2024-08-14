#include "integrators.h"
#include "parallel.h"
#include "sampling.h"
#include "stb_image_write.h"
#include "glm-aabb/AABB.hpp"

SShapeInteraction CIntegrator::intersect(CRay ray) const
{
	return accelerator->intersection(ray);
}


CPathIntegrator::CPathIntegrator(int max_depth, CPerspectiveCamera* camera, CSampler* sampler, CAccelerator* ipt_accelerator, std::vector<std::shared_ptr<CLight>> lights)
	: CIntegrator(ipt_accelerator)
	, max_depth(max_depth)
	, camera(camera)
	, sampler_prototype(sampler)
{
	light_sampler = std::make_shared<CPowerLightSampler>(lights);
}

void CPathIntegrator::render()
{
	CRGBFilm* rgb_film = camera->getFilm();
	const glm::u32vec2 image_size = rgb_film->getImageSize();
	
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
				sampler->initPixelSample(pix_pos, spp_idx);
				glm::vec3 L = evaluatePixelSample(pix_pos, sampler);
				rgb_film->addSample(pix_pos, L);
			}
		}

		//parallelFor2D(glm::u32vec2(0, 0), image_size, [&](glm::u32vec2 bound_min, glm::u32vec2 bound_max) {
		//});
	}

	rgb_film->finalizeRender(spp);
	stbi_write_tga("H:/Alpha7XRender/resource/test.tga", image_size.x, image_size.y, 3, rgb_film->getFinalData());

}

glm::vec3 CPathIntegrator::evaluatePixelSample(glm::vec2 pixel_pos, CSampler* sampler)
{
	glm::vec3 ray_origin = camera->getCameraPos();
	glm::vec3 ray_direction = camera->getPixelRayDirection(pixel_pos + sampler->getPixel2D());
	CRay ray(ray_origin, ray_direction);
	glm::vec3 L = Li(ray, sampler);
	return L;
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

		if ((depth++) == max_depth)
		{
			break;
		}

		// if no specular surface sample direct illumination
		if(isNonSpecular(bsdf.flags()))
		{
			glm::vec3 radiance_direct = SampleLd(sf_interaction, &bsdf, sampler);
			radiance += radiance_direct * beta;
		}

		// sample BSDF and generate a new direction
		{
			glm::vec3 wo = -ray.direction;
			float u = sampler->get1D();
			std::shared_ptr<SBSDFSample> bsdf_sample = bsdf.sample_f(wo, u, sampler->get2D());

			if (bsdf_sample->pdf == 0)
			{
				break;
			}

			// update path thoughput
			beta *= (bsdf_sample->f * glm::abs(glm::dot(bsdf_sample->wi, sf_interaction.norm)) / bsdf_sample->pdf);

			if (bsdf_sample->isTransmission())
			{
				eta_scale *= bsdf_sample->eta;
			}

			ray = sf_interaction.spawnRay(bsdf_sample->wi);
		}

		// Russian roulette
		{
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
		}
	}

	return radiance;
}

glm::vec3 CPathIntegrator::SampleLd(const CSurfaceInterraction& sf_interaction, const CBSDF* bsdf, CSampler* sampler)
{
	CLightSampleContext sample_ctx(sf_interaction);

	float u = sampler->get1D();
	SSampledLight sampled_light = light_sampler->Sample(u);
	std::shared_ptr<CLight> light = sampled_light.light;
	if (!light)
	{
		return glm::vec3(0, 0, 0);
	}

	glm::vec2 u_light = sampler->get2D();
	SLightSample light_sample = light->SampleLi(sample_ctx, u_light);
	if (light_sample.L == glm::vec3(0, 0, 0) && light_sample.pdf == 0)
	{
		return glm::vec3(0, 0, 0);
	}

	glm::vec3 wo = sf_interaction.wo;
	glm::vec3 wi = light_sample.wi;
	glm::vec3 f = bsdf->f(wo, wi)* glm::abs(glm::dot(wi, sf_interaction.norm));

	bool visible = traceVisibilityRay(CRay(sf_interaction.position, wi), glm::distance(sf_interaction.position, light_sample.iteraction.position));
	if (!visible)
	{
		return glm::vec3(0, 0, 0);
	}

	float p_light = sampled_light.pmf* light_sample.pdf;
	
	// if is delta light {}
	// else
	{
		float p_bsdf = bsdf->pdf(wo, wi);

		// mis
		float w_l = powerHeuristic(1, p_light, 1, p_bsdf);
		return w_l * light_sample.L * f / p_light;
	}
}


struct SPPMPixel
{
	struct SVisiblePoint
	{
		glm::vec3 position;
		glm::vec3 wo;
		CBSDF bsdf;
		glm::vec3 beta;
	};

	float radius = 0.0;
	SVisiblePoint visible_point;
	glm::vec3 l_d;
};

struct SPPMPixelListNode
{
	SPPMPixel* pixel;
	SPPMPixelListNode* node;
};

void CSPPMIntegrator::render()
{
	CRGBFilm* rgb_film = camera->getFilm();
	const glm::u32vec2 image_size = rgb_film->getImageSize();
	const int image_area = image_size.x * image_size.y;

	std::unique_ptr<SPPMPixel[]> pixels(new SPPMPixel[image_area]);

	for (int iter_idx = 0; iter_idx < iteration_num; iter_idx++)
	{

		glm::u32vec2 bound_min = glm::u32vec2(0, 0);
		glm::u32vec2 bound_max = image_size;
		for (glm::uint32 pixel_x = bound_min.x; pixel_x < bound_max.x; pixel_x++)
		{
			for (glm::uint32 pixel_y = bound_min.y; pixel_y < bound_max.y; pixel_y++)
			{
				glm::u32vec2 pix_pos = glm::u32vec2(pixel_x, pixel_y);
				CSampler* sampler = sampler_prototype; // todo: TLS
				sampler->initPixelSample(pix_pos, iter_idx);

				glm::vec3 ray_origin = camera->getCameraPos();
				glm::vec3 ray_direction = camera->getPixelRayDirection(glm::vec2(pix_pos) + sampler->getPixel2D());
				CRay ray(ray_origin, ray_direction);

				glm::ivec2 pixel_offset = pix_pos - bound_min;
				int pixel_idx = pixel_offset.x + pixel_offset.y * (bound_max.x - bound_min.x);

				SPPMPixel& pixel = pixels[pixel_idx];
				int depth = 0;
				glm::vec3 beta = glm::vec3(1, 1, 1);
				bool find_visble_point = false;
				float eta_scale = 1.0;

				while(true)
				{
					SShapeInteraction sp_interaction = intersect(ray);

					if (sp_interaction.hit_t == std::numeric_limits<float>::max())
					{
						break;
					}

					CSurfaceInterraction& sf_interaction = sp_interaction.sface_interaction;
					CBSDF bsdf = sf_interaction.getBSDF();

					if ((depth++) == max_depth || find_visble_point)
					{
						break;
					}

					{
						glm::vec3 radiance_direct = SampleLd(sf_interaction, &bsdf, sampler);
						pixel.l_d += radiance_direct * beta;
					}

					glm::vec3 wo = -ray.direction;
					EBxDFFlags bxdf_flag =  bsdf.flags();
					if (isDiffuse(bxdf_flag) || (isGlossy(bxdf_flag) && (depth == max_depth)/*todo why?*/))
					{
						pixel.visible_point = SPPMPixel::SVisiblePoint{ sf_interaction.position,sf_interaction.wo, bsdf ,beta };
						find_visble_point = true;
					}

					float u = sampler->get1D();
					std::shared_ptr<SBSDFSample> bsdf_sample = bsdf.sample_f(wo, u, sampler->get2D());

					if (bsdf_sample->pdf == 0)
					{
						break;
					}

					// update path thoughput
					beta *= (bsdf_sample->f * glm::abs(glm::dot(bsdf_sample->wi, sf_interaction.norm)) / bsdf_sample->pdf);


					if (bsdf_sample->isTransmission())
					{
						eta_scale *= (bsdf_sample->eta * bsdf_sample->eta);
					}

					ray = sf_interaction.spawnRay(bsdf_sample->wi);

					// Russian roulette
					{
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
					}
				}
			}
		}
		// build light photon map

		std::vector<SPPMPixelListNode*> grid(image_area);

		glm::AABB grid_bound;
		float max_radius = 0.0;
		for (int idx = 0; idx < image_area; idx++)
		{
			SPPMPixel& pixel = pixels[idx];
			grid_bound.extend(pixel.visible_point.position);
			max_radius = (std::max)(max_radius, pixel.radius);
		}
		
		glm::vec3 diagonal = grid_bound.getDiagonal();
		float max_diag = std::max(diagonal.x, std::max(diagonal.y, diagonal.z));
		int base_grid_res = int(max_diag / max_radius);

		int grid_res[3];
		for (int i = 0; i < 3; ++i)
		{
			grid_res[i] = std::max<int>(base_grid_res * diagonal[i] / max_diag, 1);
		}
			
		
	}
}

glm::vec3 CSPPMIntegrator::SampleLd(const CSurfaceInterraction& sf_interaction, const CBSDF* bsdf, CSampler* sampler)
{
	return glm::vec3();
}
