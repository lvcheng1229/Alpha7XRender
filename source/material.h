#pragma once
#include "bxdf.h"

class CMaterial
{
public:
};

class CDiffuseMaterial : public CMaterial
{
public:

	CDiffuseBxDF getBxDF()
	{
		return CDiffuseBxDF(reflectance);
	}
private:
	glm::vec3 reflectance;
};

class CDielectricMaterial : public CMaterial
{
public:

private:
	
};