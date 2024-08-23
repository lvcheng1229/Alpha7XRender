#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>
#include "integrators.h"
#include "parallel.h"
#include "sampling.h"
#include "stb_image_write.h"
#include "glm-aabb/AABB.hpp"
#include "lowdiscrepancy.h"

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

CSPPMIntegrator::CSPPMIntegrator(int max_depth, float initial_radius, CPerspectiveCamera* camera, CSampler* sampler, CAccelerator* ipt_accelerator, std::vector<std::shared_ptr<CLight>> lights)
	: CIntegrator(ipt_accelerator)
	, max_depth(max_depth)
	, initial_radius(initial_radius)
	, camera(camera)
	, sampler_prototype(sampler)
{
	light_sampler = std::make_shared<CPowerLightSampler>(lights);
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

	glm::vec3 phi;

	float radius = 0.0;
	SVisiblePoint visible_point;
	glm::vec3 l_d;
	
	glm::vec3 tau = glm::vec3(0,0,0);

	int m = 0;
	float n = 0.0;
};

struct SPPMPixelListNode
{
	SPPMPixel* pixel;
	SPPMPixelListNode* next_node = nullptr;
};

static bool toGrid(glm::vec3 p, const glm::AABB& bounds, const int grid_res[3], glm::ivec3& out_point)
{
	bool in_bounds = true;
	glm::vec3 norm_point = bounds.offset_normalized(p);
	for (int idx = 0; idx < 3; idx++)
	{
		out_point[idx] = int(grid_res[idx] * norm_point[idx]);
		in_bounds &= (out_point[idx] >= 0 && out_point[idx] < grid_res[idx]);
		out_point[idx] = glm::clamp(out_point[idx], 0, grid_res[idx] - 1);
	}
	return in_bounds;
}

uint32_t hashVisPoint(glm::ivec3 photon_grid_index, int hash_size)
{
	return (uint32_t)((photon_grid_index.x * 73856093) ^ (photon_grid_index.y * 19349663) ^ (photon_grid_index.z * 83492791)) % hash_size;
}

void CSPPMIntegrator::render()
{
	CRGBFilm* rgb_film = camera->getFilm();
	const glm::u32vec2 image_size = rgb_film->getImageSize();
	const int image_area = image_size.x * image_size.y;
	int photons_per_iteration = image_area;

	std::vector<SPPMPixel>pixels(image_area);
	for (auto& pixel : pixels)
	{
		pixel.radius = initial_radius;
	}

	const glm::u32vec2 bound_min = glm::u32vec2(0, 0);
	const glm::u32vec2 bound_max = image_size;

	for (int iter_idx = 0; iter_idx < iteration_num; iter_idx++)
	{
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
		memset(grid.data(), 0, sizeof(SPPMPixelListNode*) * image_area);

		std::allocator<SPPMPixelListNode> node_allocator;

		glm::AABB grid_bound;
		float max_radius = 0.0;
		for (int idx = 0; idx < image_area; idx++)
		{
			SPPMPixel& pixel = pixels[idx];
			grid_bound.extend(pixel.visible_point.position);
			max_radius = (std::max)(max_radius, pixel.radius);
		}
		
		glm::vec3 diagonal = grid_bound.getDiagonal();
		int grid_res[3];
		for (int i = 0; i < 3; ++i)
		{
			grid_res[i] = std::max<int>(std::ceil(diagonal[i] / max_radius), 1);
		}
		
		{
			for (glm::uint32 pixel_x = bound_min.x; pixel_x < bound_max.x; pixel_x++)
			{
				for (glm::uint32 pixel_y = bound_min.y; pixel_y < bound_max.y; pixel_y++)
				{
					glm::u32vec2 pix_pos = glm::u32vec2(pixel_x, pixel_y);
					glm::ivec2 pixel_offset = pix_pos - bound_min;
					int pixel_idx = pixel_offset.x + pixel_offset.y * (bound_max.x - bound_min.x);
					SPPMPixel& pixel = pixels[pixel_idx];
					
					glm::vec3 vp_beta = pixel.visible_point.beta;
					if (vp_beta.x > 0 || vp_beta.y > 0 || vp_beta.z > 0)
					{
						float r = pixel.radius;
						glm::ivec3 p_min;
						glm::ivec3 p_max;
						toGrid(pixel.visible_point.position - glm::vec3(r, r, r), grid_bound, grid_res, p_min);
						toGrid(pixel.visible_point.position + glm::vec3(r, r, r), grid_bound, grid_res, p_max);

						for (int z = p_min.z; z <= p_max.z; z++)
						{
							for (int y = p_min.y; y <= p_max.y; y++)
							{
								for (int x = p_min.x; x <= p_max.x; x++)
								{
									uint32_t node_hash = hashVisPoint(glm::ivec3(x, y, z), image_area);
									SPPMPixelListNode* pixel_node = node_allocator.allocate(1);
									pixel_node->pixel = &pixel;
									pixel_node->next_node = grid[node_hash];
									grid[node_hash] = pixel_node;
								}
							}
						}
					}
				}
			}
		}

		{
			CDigitPermutationArrayPtr permutation_array = computeRadicalInversePermutation(0);
			
			for (int idx = 0; idx < photons_per_iteration; idx++)
			{
				uint64_t haltton_idx = iter_idx * photons_per_iteration + idx;

				uint64_t halton_dim = 0;
				auto sample_1d = [&]() {
					float u = scrambledRadicalInverse(halton_dim, haltton_idx,(*permutation_array)[halton_dim]);
					++halton_dim;
					return u;
					};

					auto sample_2d = [&]() {
						glm::vec2 u
						{
							scrambledRadicalInverse(halton_dim, haltton_idx,(*permutation_array)[halton_dim]),
							scrambledRadicalInverse(halton_dim + 1, haltton_idx,(*permutation_array)[halton_dim + 1])
						};
						halton_dim += 2;
						return u;
						};

					float u = sample_1d();
					SSampledLight sampled_light = light_sampler->Sample(u);
					std::shared_ptr<CLight> light = sampled_light.light;
					if (!light)
					{
						continue;
					}


					float pdf_light = sampled_light.pmf;

					glm::vec2 u_light0 = sample_2d();
					glm::vec2 u_light1 = sample_2d();
					float u_light_time = sample_1d();

					CRay ray;
					glm::vec3 light_normal;
					float pdf_position;
					float pdf_direction;
					glm::vec3 Le = light->sampleLe(u_light0, u_light1, ray, light_normal, pdf_position, pdf_direction);

					glm::vec3 beta = std::abs(glm::dot(light_normal, ray.direction)) * Le / (pdf_light * pdf_position * pdf_direction);
					if (beta.x == 0 && beta.y == 0 && beta.z == 0)
					{
						continue;
					}

					CRay photon_ray(ray.origin, ray.direction);

					for (int depth = 0; depth < max_depth; depth++)
					{
						SShapeInteraction sp_interaction = intersect(ray);
						CSurfaceInterraction& surface_iteraction = sp_interaction.sface_interaction;

						if (sp_interaction.hit_t == std::numeric_limits<float>::max())
						{
							break;
						}

						if (depth > 0)
						{
							glm::ivec3 photon_grid_index;
							if (toGrid(surface_iteraction.position, grid_bound, grid_res, photon_grid_index))
							{
								int photon_hash_value = hashVisPoint(photon_grid_index, image_area);

								for (SPPMPixelListNode* pixel_node = grid[photon_hash_value]; pixel_node != nullptr; pixel_node = pixel_node->next_node)
								{
									SPPMPixel* pixel = pixel_node->pixel;
									float radius = pixel->radius;
									float photon_distance = glm::distance(pixel->visible_point.position, surface_iteraction.position);
									if (photon_distance > radius)
									{
										continue;
									}

									glm::vec3 wi = -photon_ray.direction;
									glm::vec3 phi = beta * pixel->visible_point.beta * pixel->visible_point.bsdf.f(pixel->visible_point.wo, wi);
									pixel->phi += phi;
									pixel->m++;
								}

								glm::vec3 wo = photon_ray.direction;
								CBSDF bsdf = surface_iteraction.getBSDF();
								std::shared_ptr<SBSDFSample> bsdf_sample = bsdf.sample_f(wo, sample_1d(), sample_2d(), ETransportMode::TM_Importance);

								if (bsdf_sample.get() == nullptr)
								{
									break;
								}

								glm::vec3 beta_new = beta_new * bsdf_sample->f * std::abs(glm::dot(bsdf_sample->wi, surface_iteraction.norm)) / bsdf_sample->pdf;

								float beta_ratio = glm::compMax(beta_new) / glm::compMax(beta);
								float q = std::max<float>(0, 1 - beta_ratio);
								if (sample_1d() < q)
									break;
								beta = beta_new / (1 - q);

								photon_ray = CRay(surface_iteraction.position, bsdf_sample->wi);
							}
						}
					}
			}
		}

		{
			for (glm::uint32 pixel_x = bound_min.x; pixel_x < bound_max.x; pixel_x++)
			{
				for (glm::uint32 pixel_y = bound_min.y; pixel_y < bound_max.y; pixel_y++)
				{
					glm::u32vec2 pix_pos = glm::u32vec2(pixel_x, pixel_y);
					glm::ivec2 pixel_offset = pix_pos - bound_min;
					int pixel_idx = pixel_offset.x + pixel_offset.y * (bound_max.x - bound_min.x);
					SPPMPixel& pixel = pixels[pixel_idx];
					int m = pixel.m;
					if (m > 0)
					{
						float gamma = 2.0 / 3.0;
						float n_new = pixel.n + gamma * m;
						float radius_new = pixel.radius * std::sqrt(n_new / (pixel.n + m));

						pixel.tau = (pixel.tau + pixel.phi) * radius_new * radius_new / (pixel.radius * pixel.radius);

						pixel.n = n_new;
						pixel.radius = radius_new;
						pixel.m = 0;
						pixel.phi = glm::vec3(0, 0, 0);
					}

					pixel.visible_point.beta = glm::vec3(0, 0, 0);
				}
			}
		}

	}

	{
		for (glm::uint32 pixel_x = bound_min.x; pixel_x < bound_max.x; pixel_x++)
		{
			for (glm::uint32 pixel_y = bound_min.y; pixel_y < bound_max.y; pixel_y++)
			{
				glm::u32vec2 pix_pos = glm::u32vec2(pixel_x, pixel_y);
				glm::ivec2 pixel_offset = pix_pos - bound_min;
				int pixel_idx = pixel_offset.x + pixel_offset.y * (bound_max.x - bound_min.x);
				SPPMPixel& pixel = pixels[pixel_idx];

				float num_photons = float(iteration_num) * photons_per_iteration;
				glm::vec3 L = pixel.l_d / float(iteration_num) + pixel.tau / (num_photons * glm::pi<float>() * (pixel.radius * pixel.radius));
				rgb_film->addSample(pix_pos, L);
			}
		}

		rgb_film->finalizeRender(1);
		stbi_write_tga("H:/Alpha7XRender/resource/test.tga", image_size.x, image_size.y, 3, rgb_film->getFinalData());
	}
}

glm::vec3 CSPPMIntegrator::SampleLd(const CSurfaceInterraction& sf_interaction, const CBSDF* bsdf, CSampler* sampler)
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
	glm::vec3 f = bsdf->f(wo, wi) * glm::abs(glm::dot(wi, sf_interaction.norm));

	bool visible = traceVisibilityRay(CRay(sf_interaction.position, wi), glm::distance(sf_interaction.position, light_sample.iteraction.position));
	if (!visible)
	{
		return glm::vec3(0, 0, 0);
	}

	float p_light = sampled_light.pmf * light_sample.pdf;

	// if is delta light {}
	// else
	{
		float p_bsdf = bsdf->pdf(wo, wi);

		// mis
		float w_l = powerHeuristic(1, p_light, 1, p_bsdf);
		return w_l * light_sample.L * f / p_light;
	}

	return glm::vec3(0, 0, 0);
}
