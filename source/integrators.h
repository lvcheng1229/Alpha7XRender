#pragma once
#include "samplers.h"
#include "cameras.h"
#include "ray.h"
#include "lightsampler.h"
#include "interaction.h"
#include "bsdf.h"
#include "geometry.h"

class CIntegrator
{
public:
	CIntegrator(CAccelerator* ipt_accelerator)
		:accelerator(ipt_accelerator) {};

	virtual void render() = 0;
	SShapeInteraction intersect(CRay ray)const;
	bool traceVisibilityRay(CRay ray, float max_t)
	{
		return accelerator->traceVisibilityRay(ray, max_t);
	}
private:
	CAccelerator* accelerator;
};

class CPathIntegrator : public CIntegrator
{
public:
	CPathIntegrator(int max_depth, CPerspectiveCamera* camera, CSampler* sampler, CAccelerator* ipt_accelerator, std::vector<std::shared_ptr<CLight>> lights);

	void render();
private:

	glm::vec3 evaluatePixelSample(glm::vec2 pixel_pos, CSampler* sampler);
	glm::vec3 Li(CRay ray, CSampler* sampler);

	glm::vec3 SampleLd(const CSurfaceInterraction& sf_interaction, const CBSDF* bsdf, CSampler* sampler);

	int max_depth;
	std::shared_ptr<CLightSampler> light_sampler;
	CPerspectiveCamera* camera;
	CSampler* sampler_prototype;
};

class CSPPMIntegrator : public CIntegrator
{
public:
	CSPPMIntegrator(int max_depth, CPerspectiveCamera* camera, CSampler* sampler, CAccelerator* ipt_accelerator, std::vector<std::shared_ptr<CLight>> lights);
	void render();

private:
	glm::vec3 SampleLd(const CSurfaceInterraction& sf_interaction, const CBSDF* bsdf, CSampler* sampler);

	float initial_radius;
	std::shared_ptr<CLightSampler> light_sampler;
	int max_depth;
	CPerspectiveCamera* camera;
	int iteration_num = 0;
	CSampler* sampler_prototype;
};

