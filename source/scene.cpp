#include <iterator>
#include "scene.h"

void Alpha7XSceneBuilder::Scale(float sx, float sy, float sz)
{
}

void Alpha7XSceneBuilder::Shape(const std::string& name, pbrt::ParsedParameterVector params)
{
	pbrt::ParameterDictionary dict(std::move(params));

	int areaLightIndex = -1;
	if (!graphics_state.area_light_name.empty())
	{
		assert(false);
	}

	SShapeSceneEntity shape_entity(name, dict, graphics_state.material_name);
	shapes.push_back(std::move(shape_entity));
}

Alpha7XSceneBuilder::Alpha7XSceneBuilder(CAlpa7XScene* scene)
	:scene(scene)
{
}

void Alpha7XSceneBuilder::Option(const std::string& name, const std::string& value)
{
}

void Alpha7XSceneBuilder::Identity()
{
}

void Alpha7XSceneBuilder::Translate(float dx, float dy, float dz)
{
}

void Alpha7XSceneBuilder::Rotate(float angle, float ax, float ay, float az)
{
}

void Alpha7XSceneBuilder::LookAt(float ex, float ey, float ez, float lx, float ly, float lz, float ux, float uy, float uz)
{
}

void Alpha7XSceneBuilder::ConcatTransform(float transform[16])
{
}

void Alpha7XSceneBuilder::Transform(float* transform)
{
	glm::vec4 v0 = glm::vec4(transform[0], transform[1], transform[2], transform[3]);
	glm::vec4 v1 = glm::vec4(transform[4], transform[5], transform[6], transform[7]);
	glm::vec4 v2 = glm::vec4(transform[8], transform[9], transform[10], transform[11]);
	glm::vec4 v3 = glm::vec4(transform[12], transform[13], transform[14], transform[15]);
	graphics_state.transform = glm::mat4x4(v0, v1, v2, v3);
}

void Alpha7XSceneBuilder::CoordinateSystem(const std::string&)
{
}

void Alpha7XSceneBuilder::CoordSysTransform(const std::string&)
{
}

void Alpha7XSceneBuilder::ActiveTransformAll()
{
}

void Alpha7XSceneBuilder::ActiveTransformEndTime()
{
}

void Alpha7XSceneBuilder::ActiveTransformStartTime()
{
}

void Alpha7XSceneBuilder::TransformTimes(float start, float end)
{
}

void Alpha7XSceneBuilder::ColorSpace(const std::string& n)
{
}

void Alpha7XSceneBuilder::PixelFilter(const std::string& name, pbrt::ParsedParameterVector params)
{
}

void Alpha7XSceneBuilder::Film(const std::string& type, pbrt::ParsedParameterVector params)
{
	pbrt::ParameterDictionary dict(std::move(params));
	film = SSceneEntity(type, std::move(dict));
}

void Alpha7XSceneBuilder::Accelerator(const std::string& name, pbrt::ParsedParameterVector params)
{
}

void Alpha7XSceneBuilder::Integrator(const std::string& name, pbrt::ParsedParameterVector params)
{
	pbrt::ParameterDictionary dict(std::move(params));
	integrator = SSceneEntity(name, std::move(dict));
}

void Alpha7XSceneBuilder::Camera(const std::string& name, pbrt::ParsedParameterVector params)
{
	camera = SCameraSceneEntity(graphics_state.transform, name, params);
}

void Alpha7XSceneBuilder::MakeNamedMedium(const std::string& name, pbrt::ParsedParameterVector params)
{
}

void Alpha7XSceneBuilder::MediumInterface(const std::string& insideName, const std::string& outsideName)
{
}

void Alpha7XSceneBuilder::Sampler(const std::string& name, pbrt::ParsedParameterVector params)
{
	pbrt::ParameterDictionary dict(std::move(params));
	sampler = SSceneEntity(name, std::move(dict));
}

void Alpha7XSceneBuilder::WorldBegin()
{
	scene->SetOptions(filter, film, camera, sampler, integrator, accelerator);
}

void Alpha7XSceneBuilder::AttributeBegin()
{
}

void Alpha7XSceneBuilder::AttributeEnd()
{
}

void Alpha7XSceneBuilder::Attribute(const std::string& target, pbrt::ParsedParameterVector params)
{
}

void Alpha7XSceneBuilder::Texture(const std::string& name, const std::string& type, const std::string& texname, pbrt::ParsedParameterVector params)
{
}

void Alpha7XSceneBuilder::Material(const std::string& name, pbrt::ParsedParameterVector params)
{
}

void Alpha7XSceneBuilder::MakeNamedMaterial(const std::string& name, pbrt::ParsedParameterVector params)
{
	pbrt::ParameterDictionary dict(std::move(params));
	scene->named_materials.push_back(std::pair<std::string,SSceneEntity>(name, SSceneEntity(name, dict)));
}

void Alpha7XSceneBuilder::NamedMaterial(const std::string& name)
{
	graphics_state.material_name = name;
}

void Alpha7XSceneBuilder::LightSource(const std::string& name, pbrt::ParsedParameterVector params)
{
}

void Alpha7XSceneBuilder::AreaLightSource(const std::string& name, pbrt::ParsedParameterVector params)
{
}

void Alpha7XSceneBuilder::ReverseOrientation()
{
}

void Alpha7XSceneBuilder::ObjectBegin(const std::string& name)
{
}

void Alpha7XSceneBuilder::ObjectEnd()
{
}

void Alpha7XSceneBuilder::ObjectInstance(const std::string& name)
{
}

void Alpha7XSceneBuilder::EndOfFiles()
{
	if (!shapes.empty())
	{
		std::move(std::begin(shapes), std::end(shapes), std::back_inserter(scene->shapes));
	}
}

CAlpa7XScene::~CAlpa7XScene()
{
	delete camera;
	delete sampler;
	delete rgb_film;
	delete accelerator;
}

std::unique_ptr<CIntegrator> CAlpa7XScene::createIntegrator(CPerspectiveCamera* camera, CSampler* sampler, CAccelerator* ipt_scene_inter_cpt, std::vector<CLight*> lights)
{
	if (integrators.name == "path")
	{
		int max_depth = integrators.parameters.GetOneInt("maxdepth", 5);
		return std::make_unique<CPathIntegrator>(max_depth, camera, sampler, ipt_scene_inter_cpt, lights);
	}
	else
	{
		assert(false);
	}
	return std::unique_ptr<CIntegrator>(nullptr);
}

void CAlpa7XScene::SetOptions(SSceneEntity ipt_filter, SSceneEntity ipt_film, SCameraSceneEntity ipt_camera, SSceneEntity ipt_sampler, SSceneEntity ipt_integrator, SSceneEntity ipt_accelerator)
{
	integrators = ipt_integrator;
	
	int img_sz_x = ipt_film.parameters.GetOneInt("xresolution", 1280);
	int img_sz_y = ipt_film.parameters.GetOneInt("yresolution", 720);
	rgb_film = new CRGBFilm(glm::uvec2(img_sz_x, img_sz_y));

	float fov = ipt_camera.parameters.GetOneFloat("fov",90);
	camera = new CPerspectiveCamera(ipt_camera.camera_trans_mat, fov,rgb_film);

	int spp = ipt_sampler.parameters.GetOneInt("pixelsamples", 4);
	if (ipt_sampler.name == "sobol")
	{
		sampler = new CSobelSampler(spp, glm::ivec2(img_sz_x, img_sz_y));
	}
	else
	{
		assert(false);
	}
}

CAccelerator* CAlpa7XScene::createAccelerator()
{
	if (accelerator == nullptr)
	{
		accelerator = new CAccelerator();
		for (int mat_idx = 0; mat_idx < named_materials.size(); mat_idx++)
		{
			SSceneEntity& scene_entity = named_materials[mat_idx].second;
			
			CMaterial* new_material = nullptr;
			std::string material_type = scene_entity.parameters.GetOneString("type", "");
			if (material_type == "diffuse")
			{
				for (const pbrt::ParsedParameter* p : scene_entity.parameters.getParameters())
				{
					if (p->name == "reflectance" && p->type == "rgb")
					{
						new_material = new CDiffuseMaterial(glm::vec3(p->floats[0], p->floats[1], p->floats[2]));
					}
				}
			}
			else
			{
				assert(false);
			}
			accelerator->scene_materials.push_back(new_material);
			accelerator->mat_name_idx_map.insert(std::pair(named_materials[mat_idx].first, mat_idx));
		}

		for (int shape_idx = 0; shape_idx < shapes.size(); shape_idx++)
		{
			SShapeSceneEntity& shape_entity = shapes[shape_idx];
			CLight* area_light = nullptr;
			if (area_light == nullptr)
			{
				auto mat_map_iter = accelerator->mat_name_idx_map.find(shape_entity.material_name);
				if (mat_map_iter != accelerator->mat_name_idx_map.end())
				{
					SA7XGeometry scene_geometry;
					scene_geometry.geometry = accelerator->createRTCGeometry(&shape_entity, shape_idx);
					scene_geometry.material_idx = mat_map_iter->second;
					accelerator->scene_geometries.push_back(scene_geometry);
				}
				else
				{
					assert(false);
				}

			}
			else
			{
				assert(false);
			}
		}
		accelerator->finalizeRtSceneCreate();
	}
	
	return accelerator;
}
