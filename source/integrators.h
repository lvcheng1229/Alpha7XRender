#pragma once
#include "samplers.h"
#include "cameras.h"
#include "ray.h"
#include "lightsampler.h"
#include "interaction.h"
#include "bsdf.h"
#include "shapes.h"

class CIntegrator
{
public:
	virtual void render() = 0;
	SShapeInteraction intersect(CRay ray, float t_max = std::numeric_limits<float>::max())const;
private:
	
};

class CPathIntegrator : public CIntegrator
{
public:
	CPathIntegrator(int max_depth, CPerspectiveCamera* camera, CSampler* sampler, std::vector<CLight*> lights);

	void render();
private:
	void evaluatePixelSample(glm::vec2 pixel_pos, CSampler* sampler);
	glm::vec3 Li(CRay ray, CSampler* sampler);

	glm::vec3 SampleLd(const CSurfaceInterraction& sf_interaction, const CBSDF* bsdf, CSampler* sampler);

	int max_depth;
	CLightSampler* light_sampler;
	CPerspectiveCamera* camera;
	CSampler* sampler_prototype;
};

