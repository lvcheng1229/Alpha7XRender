#pragma once
#include "pbrt_parser/parser.h"
#include "pbrt_parser/paramdict.h"
#include "film.h"

class CAlpa7XScene
{
public:
private:
    CRGBFilm rgb_film;
};

class Alpha7XSceneBuilder : public pbrt::ParserTarget
{
public:

    void Scale(float sx, float sy, float sz);

    void Shape(const std::string& name, pbrt::ParsedParameterVector params);

    ~Alpha7XSceneBuilder() {};

    void Option(const std::string& name, const std::string& value);

    void Identity();
    void Translate(float dx, float dy, float dz);
    void Rotate(float angle, float ax, float ay, float az);
    void LookAt(float ex, float ey, float ez, float lx, float ly, float lz, float ux, float uy, float uz);
    void ConcatTransform(float transform[16]);
    void Transform(float transform[16]);
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
};