#pragma once
#include <embree4/rtcore.h>
#include <map>
#include <string>
#include <filesystem>

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

struct SShapeSampleDesc
{
	glm::vec3 position;
};

class CTriangle
{
public:

	inline float area()
	{
		const std::shared_ptr<STriangleMesh> tri_mesh = triangle_mesh;
		const std::vector<int>& indices = tri_mesh->indices;
		const std::vector<glm::vec3> points = tri_mesh->points;
		glm::i32vec3 vtx_indices(indices[tri_index * 3 + 0], indices[tri_index * 3 + 1], indices[tri_index * 3 + 2]);
		glm::vec3 positions[3] = { points[vtx_indices.x],points[vtx_indices.y], points[vtx_indices.z] };
		return 0.5f * glm::length(glm::cross(positions[1] - positions[0], positions[2] - positions[0]));
	}

	inline SShapeSample sample(glm::vec2 u)
	{
		const std::shared_ptr<STriangleMesh> tri_mesh = triangle_mesh;
		const std::vector<int>& indices = tri_mesh->indices;
		const std::vector<glm::vec3> points = tri_mesh->points;
		const std::vector<glm::vec3> normals = tri_mesh->normals;

		glm::i32vec3 vtx_indices(indices[tri_index * 3 + 0], indices[tri_index * 3 + 1], indices[tri_index * 3 + 2]);
		glm::vec3 positions[3] = { points[vtx_indices.x],points[vtx_indices.y], points[vtx_indices.z] };
		glm::vec3 normal[3] = { normals[vtx_indices.x],normals[vtx_indices.y], normals[vtx_indices.z] };
		glm::vec3 barycentric_coords = sampleUniformTriangle(u); //!

		glm::vec3 sampled_pos = positions[0] * barycentric_coords.x + positions[1] * barycentric_coords.y + positions[2] * barycentric_coords.z;
		glm::vec3 sampled_normal = normal[0] * barycentric_coords.x + normal[1] * barycentric_coords.y + normal[2] * barycentric_coords.z;
		sampled_normal = glm::normalize(sampled_normal);

		return SShapeSample{ CInteraction {sampled_pos,sampled_normal},area()};
	}

	inline SShapeSample sample(SShapeSampleDesc sample_desc, glm::vec2 u)
	{
		//const std::shared_ptr<STriangleMesh> tri_mesh = triangle_mesh;//!
		//const std::vector<int>& indices = tri_mesh->indices;//!
		//const std::vector<glm::vec3> points = tri_mesh->points;//!
		//const std::vector<glm::vec3> normals = tri_mesh->normals;//!
		//
		//glm::i32vec3 vtx_indices(indices[tri_index * 3 + 0], indices[tri_index * 3 + 1], indices[tri_index * 3 + 2]);//!
		//glm::vec3 positions[3] = { points[vtx_indices.x],points[vtx_indices.y], points[vtx_indices.z] };//!
		//glm::vec3 normal[3] = { normals[vtx_indices.x],normals[vtx_indices.y], normals[vtx_indices.z] };//!
		//
		//glm::vec3 barycentric_coords = sampleUniformTriangle(u); //!
		//glm::vec3 sampled_pos = positions[0] * barycentric_coords.x + positions[1] * barycentric_coords.y + positions[2] * barycentric_coords.z;//!
		//glm::vec3 sampled_normal = normal[0] * barycentric_coords.x + normal[1] * barycentric_coords.y + normal[2] * barycentric_coords.z;//!
		//sampled_normal = glm::normalize(sampled_normal);//!

		SShapeSample shape_sample = sample(u);

		glm::vec3 wi = shape_sample.inter.position - sample_desc.position;
		float squared_length = (wi.x * wi.x + wi.y * wi.y + wi.z * wi.z);
		if (squared_length == 0)
		{
			return SShapeSample{ CInteraction(),0 };
		}

		wi = glm::normalize(wi);

		float distance = glm::distance(shape_sample.inter.position, sample_desc.position);

		float tri_area = area();
		float cos_theta = std::abs(glm::dot(shape_sample.inter.norm, -wi));
		float sample_pdf = (distance * distance) / (tri_area * cos_theta);

		if (std::isinf(sample_pdf))
		{
			return SShapeSample{ CInteraction(),0 };
		}

		return SShapeSample{ CInteraction {shape_sample.inter.position,shape_sample.inter.position},sample_pdf };
	}

	std::shared_ptr<STriangleMesh> triangle_mesh;
	int tri_index = -1;

};

struct SShapeInteraction
{
	SShapeInteraction() :hit_t(std::numeric_limits<float>::max()) {};

	CSurfaceInterraction sface_interaction;

	//todo: unused variable
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

	RTCGeometry createRTCGeometry(SShapeSceneEntity* shape_entity, int ID,const std::filesystem::path& file_path);
	void finalizeRtSceneCreate();

private:
	RTCGeometry readPLY(const std::string& file_name, int ID);

	friend class CAlpa7XScene;

	RTCScene rt_scene;
	RTCDevice rt_device;

	std::map<std::string, int> mat_name_idx_map;
	std::vector<CMaterial*> scene_materials;
	std::vector<SA7XGeometry> scene_geometries;
	std::vector<STriangleMesh> lights_triangles;
};