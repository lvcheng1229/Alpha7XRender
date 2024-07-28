#include "geometry.h"

SShapeInteraction CSceneIntersectionCompute::intersection(CRay ray)
{
	RTCIntersectArguments args;
	rtcInitIntersectArguments(&args);

	RTCRayHit embree_ray;
	embree_ray.ray.org_x = ray.origin.x;
	embree_ray.ray.org_y = ray.origin.y;
	embree_ray.ray.org_z = ray.origin.z;
	embree_ray.ray.dir_x = ray.direction.x;
	embree_ray.ray.dir_y = ray.direction.y;
	embree_ray.ray.dir_z = ray.direction.z;
	embree_ray.ray.tnear = 0;
	embree_ray.ray.tfar = 1e30f;
	embree_ray.ray.time = 0;
	embree_ray.ray.mask = -1;
	embree_ray.hit.u = embree_ray.hit.v = 0;
	embree_ray.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	embree_ray.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
	embree_ray.hit.primID = RTC_INVALID_GEOMETRY_ID;

	rtcIntersect1(scene, &embree_ray, &args);

	SShapeInteraction shape_interaction;

	if (embree_ray.hit.geomID != RTC_INVALID_GEOMETRY_ID)
	{
		const SA7XGeometry& scene_geometry = scene_geometries[embree_ray.hit.geomID];
		glm::vec3 hit_normal(embree_ray.hit.Ng_x, embree_ray.hit.Ng_y, embree_ray.hit.Ng_z);
		int mat_idx = scene_geometry.material_idx;
		
		//rtcAttachGeometryByID

		shape_interaction.hit_t = embree_ray.ray.tfar;
		shape_interaction.sface_interaction.norm = hit_normal;
		shape_interaction.sface_interaction.material = &scene_materials[mat_idx];
		shape_interaction.sface_interaction.wo = ray.direction;
	}

	return shape_interaction;
}
