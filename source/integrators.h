#pragma once
#include "samplers.h"
#include "cameras.h"

class CIntegrator
{
public:
	virtual void render() = 0;
private:
	
};

class CPathIntegrator : public CIntegrator
{
public:
	void render();
private:
	CCamera* camera;
	CSampler* sampler;
};

