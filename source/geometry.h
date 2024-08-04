#pragma once
#include <embree4/rtcore.h>
#include <map>
#include <string>

#include "ray.h"
#include "material.h"
#include "sampling.h"
#include "interaction.h"

struct SA7XGeometry
{
	RTCGeometry geometry;
	int material_idx;
};

struct STriangleMesh
{
	std::vector<int> indices;
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
};

struct SShapeSample
{
	CInteraction inter;
	float pdf;
};


class CTriangle
{
public:

	inline float area()
	{
		const STriangleMesh* tri_mesh = triangle_mesh;
		const std::vector<int>& indices = tri_mesh->indices;
		const std::vector<glm::vec3> points = tri_mesh->points;
		glm::i32vec3 vtx_indices(indices[tri_index * 3 + 0], indices[tri_index * 3 + 1], indices[tri_index * 3 + 2]);
		glm::vec3 positions[3] = { points[vtx_indices.x],points[vtx_indices.y], points[vtx_indices.z] };
		return 0.5f * glm::length(glm::cross(positions[1] - positions[0], positions[2] - positions[0]));
	}

	inline SShapeSample sample(glm::vec2 u)
	{
		const STriangleMesh* tri_mesh = triangle_mesh;
		const std::vector<int>& indices = tri_mesh->indices;
		const std::vector<glm::vec3> points = tri_mesh->points;
		const std::vector<glm::vec3> normals = tri_mesh->normals;
		const std::vector<glm::vec2> uvs = tri_mesh->uvs;

		glm::i32vec3 vtx_indices(indices[tri_index * 3 + 0], indices[tri_index * 3 + 1], indices[tri_index * 3 + 2]);
		glm::vec3 positions[3] = { points[vtx_indices.x],points[vtx_indices.y], points[vtx_indices.z] };

		glm::vec3 barycentric_coords = sampleUniformTriangle(u); //!
		glm::vec3 sampled_pos = positions[0] * barycentric_coords.x + positions[1] * barycentric_coords.y + positions[2] * barycentric_coords.z;
		glm::vec3 sampled_normal = normals[0] * barycentric_coords.x + normals[1] * barycentric_coords.y + normals[2] * barycentric_coords.z;

		return SShapeSample{ CInteraction {sampled_pos,sampled_normal}, 1.0f / area()};
	}

	STriangleMesh* triangle_mesh;
	int tri_index = -1;

};

struct SShapeInteraction
{
	SShapeInteraction() :hit_t(std::numeric_limits<float>::max()) {};

	CSurfaceInterraction sface_interaction;
	float hit_t;
};

class SShapeSceneEntity;
class CAccelerator
{
public:
	CAccelerator();
	~CAccelerator();
	
	SShapeInteraction intersection(CRay ray);

	// true = visible
	// false = occluded
	bool traceVisibilityRay(CRay ray, float max_t);

	RTCGeometry createRTCGeometry(SShapeSceneEntity* shape_entity, int ID);
	void finalizeRtSceneCreate();

private:
	friend class CAlpa7XScene;

	RTCScene rt_scene;
	RTCDevice rt_device;

	std::map<std::string, int> mat_name_idx_map;
	std::vector<CMaterial*> scene_materials;
	std::vector<SA7XGeometry> scene_geometries;
	std::vector<STriangleMesh> lights_triangles;
};