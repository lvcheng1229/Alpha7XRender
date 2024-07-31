#include "geometry.h"
#include "scene.h"

void errorFunction(void* userPtr, enum RTCError error, const char* str)
{
	printf("error %d: %s\n", error, str);
}

CAccelerator::CAccelerator()
{
	rt_device = rtcNewDevice(NULL);
	if (!rt_device)
	{
		printf("error %d: cannot create device\n", rtcGetDeviceError(NULL));
	}
	rtcSetDeviceErrorFunction(rt_device, errorFunction, NULL);

	rt_scene = rtcNewScene(rt_device);
}

CAccelerator::~CAccelerator()
{
	for (auto* p_iter : scene_materials)
	{
		delete p_iter;
	}

	for (auto& geo_iter : scene_geometries)
	{
		rtcReleaseGeometry(geo_iter.geometry);
	}

	rtcReleaseScene(rt_scene);
	rtcReleaseDevice(rt_device);
}

SShapeInteraction CAccelerator::intersection(CRay ray)
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

	rtcIntersect1(rt_scene, &embree_ray, &args);

	SShapeInteraction shape_interaction;

	if (embree_ray.hit.geomID != RTC_INVALID_GEOMETRY_ID)
	{
		const SA7XGeometry& scene_geometry = scene_geometries[embree_ray.hit.geomID];
		glm::vec3 hit_normal = glm::normalize(glm::vec3(embree_ray.hit.Ng_x, embree_ray.hit.Ng_y, embree_ray.hit.Ng_z));
		int mat_idx = scene_geometry.material_idx;
		
		//rtcAttachGeometryByID

		shape_interaction.hit_t = embree_ray.ray.tfar;
		shape_interaction.sface_interaction.norm = faceForward(ray.direction, hit_normal);
		shape_interaction.sface_interaction.material = scene_materials[mat_idx];
		shape_interaction.sface_interaction.wo = ray.direction;
	}

	return shape_interaction;
}

RTCGeometry CAccelerator::createRTCGeometry(SShapeSceneEntity* shape_entity, int ID)
{
	if (shape_entity->name == "trianglemesh")
	{
		std::vector<int> indices = shape_entity->parameters.GetIntArray("indices");
		std::vector<glm::vec3> positions = shape_entity->parameters.GetPoint3fArray("P");
		std::vector<glm::vec2> uvs = shape_entity->parameters.GetPoint2fArray("uv");

		RTCGeometry geom = rtcNewGeometry(rt_device, RTC_GEOMETRY_TYPE_TRIANGLE);
		float* geo_vertices = (float*)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float), positions.size());
		unsigned* geo_indices = (unsigned*)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(unsigned), indices.size() / 3);
		memcpy(geo_vertices, positions.data(), positions.size() * sizeof(glm::vec3));
		for (int idx = 0; idx < indices.size(); idx++)
		{
			geo_indices[idx] = unsigned(indices[idx]);
		}
		rtcCommitGeometry(geom);
		rtcAttachGeometryByID(rt_scene, geom, ID);
		return geom;
		
	}
	else if (shape_entity->name == "plymesh")
	{
		assert(false);
	}
	return RTCGeometry();
}

void CAccelerator::finalizeRtSceneCreate()
{
	rtcCommitScene(rt_scene);
}

