#pragma once
#include <embree4/rtcore.h>
#include "ray.h"
#include "shapes.h"
#include "material.h"

struct SA7XGeometry
{
	RTCGeometry geometry;
	int material_idx;
};

class CSceneIntersectionCompute
{
public:
	SShapeInteraction intersection(CRay ray);

private:
	RTCScene scene;

	std::vector<CMaterial> scene_materials;
	std::vector<SA7XGeometry> scene_geometries;
};