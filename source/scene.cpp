#include "scene.h"

void Alpha7XSceneBuilder::Scale(float sx, float sy, float sz)
{
}

void Alpha7XSceneBuilder::Shape(const std::string& name, pbrt::ParsedParameterVector params)
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

void Alpha7XSceneBuilder::Transform(float transform[16])
{
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
}

void Alpha7XSceneBuilder::Accelerator(const std::string& name, pbrt::ParsedParameterVector params)
{
}

void Alpha7XSceneBuilder::Integrator(const std::string& name, pbrt::ParsedParameterVector params)
{
}

void Alpha7XSceneBuilder::Camera(const std::string&, pbrt::ParsedParameterVector params)
{
}

void Alpha7XSceneBuilder::MakeNamedMedium(const std::string& name, pbrt::ParsedParameterVector params)
{
}

void Alpha7XSceneBuilder::MediumInterface(const std::string& insideName, const std::string& outsideName)
{
}

void Alpha7XSceneBuilder::Sampler(const std::string& name, pbrt::ParsedParameterVector params)
{
}

void Alpha7XSceneBuilder::WorldBegin()
{
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
