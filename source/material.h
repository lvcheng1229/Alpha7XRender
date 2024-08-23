#pragma once
#include <memory>
#include "bxdf.h"


class CMaterial
{
public:
	inline std::shared_ptr<CBxDF> getBxdf() { return bxdf; }
protected:
	std::shared_ptr<CBxDF> bxdf;
};

class CDiffuseMaterial : public CMaterial
{
public:
	CDiffuseMaterial(glm::vec3 reflectance)
		:reflectance(reflectance) 
	{
		bxdf = std::make_shared<CDiffuseBxDF>(reflectance);
	};

private:
	glm::vec3 reflectance;
};

class CDielectricMaterial : public CMaterial
{
public:
	CDielectricMaterial(float eta, bool roughness_remapping)
		:eta(eta)
	{
		if (roughness_remapping == false)
		{
			bxdf = std::make_shared<CDielectricBxDF>(eta, CTrowbridgeReitzDistribution(0, 0));
		}
	};
private:
	float eta;
	bool roughness_remapping;
};