#include "scene.h"

void Alpha7XSceneBuilder::Scale(float sx, float sy, float sz)
{
}

void Alpha7XSceneBuilder::Shape(const std::string& name, pbrt::ParsedParameterVector params)
{
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
}

void Alpha7XSceneBuilder::NamedMaterial(const std::string& name)
{
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
}

CAlpa7XScene::~CAlpa7XScene()
{
	delete camera;
	delete sampler;
	delete rgb_film;
}

std::unique_ptr<CIntegrator> CAlpa7XScene::createIntegrator(CPerspectiveCamera* camera, CSampler* sampler, std::vector<CLight*> lights)
{
	if (integrators.name == "path")
	{
		int max_depth = integrators.parameters.GetOneInt("maxdepth", 5);
		return std::make_unique<CPathIntegrator>(max_depth, camera, sampler, lights);
	}
	else
	{
		assert(false);
	}
	return std::unique_ptr<CIntegrator>();
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
		sampler = new CSobelSampler(spp);
	}
	else
	{
		assert(false);
	}
}
