#pragma once
#include "interaction.h"
struct SShapeInteraction
{
	SShapeInteraction() :hit_t(std::numeric_limits<float>::max()) {};

	CSurfaceInterraction sface_interaction;
	float hit_t;
};