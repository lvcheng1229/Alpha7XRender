#pragma once
#include <memory>
#include "pbrt_parser/parser.h"
#include "pbrt_parser/paramdict.h"
#include "film.h"
#include "integrators.h"
#include "cameras.h"
#include "samplers.h"
#include "glm/matrix.hpp"

struct SSceneEntity
{
    SSceneEntity() = default;
    SSceneEntity(const std::string& name, pbrt::ParameterDictionary parameters)
        : name(name), parameters(parameters){}

   std::string name;
   pbrt::ParameterDictionary parameters;
};

struct SCameraSceneEntity : public SSceneEntity
{
    SCameraSceneEntity() = default;
    SCameraSceneEntity(glm::mat4x4 trans_mat, const std::string& name, pbrt::ParameterDictionary parameters)
        :camera_trans_mat(trans_mat)
        , SSceneEntity(name, parameters) {}

    glm::mat4x4 camera_trans_mat;
};

class CAlpa7XScene
{
public:
    ~CAlpa7XScene();

    std::unique_ptr<CIntegrator> createIntegrator(CPerspectiveCamera* camera,CSampler* sampler,std::vector<CLight*> lights);

    void SetOptions(SSceneEntity ipt_filter, SSceneEntity ipt_film, SCameraSceneEntity ipt_camera, SSceneEntity ipt_sampler, SSceneEntity ipt_integrator, SSceneEntity ipt_accelerator);

    inline CPerspectiveCamera* getCamera() { return camera; }
    inline CSampler* getSampler() { return sampler; }

    SSceneEntity integrators;

    CPerspectiveCamera* camera;
    CSampler* sampler;
    CRGBFilm* rgb_film;

private:
};

class Alpha7XSceneBuilder : public pbrt::ParserTarget
{
public:

    void Scale(float sx, float sy, float sz);

    void Shape(const std::string& name, pbrt::ParsedParameterVector params);

    Alpha7XSceneBuilder(CAlpa7XScene* scene);
    ~Alpha7XSceneBuilder() {};

    void Option(const std::string& name, const std::string& value);

    void Identity();
    void Translate(float dx, float dy, float dz);
    void Rotate(float angle, float ax, float ay, float az);
    void LookAt(float ex, float ey, float ez, float lx, float ly, float lz, float ux, float uy, float uz);
    void ConcatTransform(float transform[16]);
    void Transform(float* transform);
    void CoordinateSystem(const std::string&);
    void CoordSysTransform(const std::string&);
    void ActiveTransformAll();
    void ActiveTransformEndTime();
    void ActiveTransformStartTime();
    void TransformTimes(float start, float end);

    void ColorSpace(const std::string& n);
    void PixelFilter(const std::string& name, pbrt::ParsedParameterVector params);
    void Film(const std::string& type, pbrt::ParsedParameterVector params);
    void Accelerator(const std::string& name, pbrt::ParsedParameterVector params);
    void Integrator(const std::string& name, pbrt::ParsedParameterVector params);
    void Camera(const std::string&, pbrt::ParsedParameterVector params);
    void MakeNamedMedium(const std::string& name, pbrt::ParsedParameterVector params);
    void MediumInterface(const std::string& insideName, const std::string& outsideName);
    void Sampler(const std::string& name, pbrt::ParsedParameterVector params);

    void WorldBegin();
    void AttributeBegin();
    void AttributeEnd();
    void Attribute(const std::string& target, pbrt::ParsedParameterVector params);
    void Texture(const std::string& name, const std::string& type,
        const std::string& texname, pbrt::ParsedParameterVector params);
    void Material(const std::string& name, pbrt::ParsedParameterVector params);
    void MakeNamedMaterial(const std::string& name, pbrt::ParsedParameterVector params);
    void NamedMaterial(const std::string& name);
    void LightSource(const std::string& name, pbrt::ParsedParameterVector params);
    void AreaLightSource(const std::string& name, pbrt::ParsedParameterVector params);
    void ReverseOrientation();
    void ObjectBegin(const std::string& name);
    void ObjectEnd();
    void ObjectInstance(const std::string& name);

    void EndOfFiles();

	friend void parse(pbrt::ParserTarget* scene, std::unique_ptr<pbrt::Tokenizer> t);

private:
    struct SGraphicsState
    {
        glm::mat4x4 transform;
    };
    SGraphicsState graphics_state;
    std::vector<SGraphicsState> pushed_graphics_states;


    CAlpa7XScene* scene;

   

    SSceneEntity sampler;
    SSceneEntity film, integrator, filter, accelerator;
    SCameraSceneEntity camera;
};