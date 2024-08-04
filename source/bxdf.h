#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include "alpha7x_math.h"
#include "sampling.h"

enum EBxDFFlags
{
	BXDF_FG_None = 0,
    BXDF_FG_Reflection = 1 << 0,
    BXDF_FG_Transmission = 1 << 1,

    BXDF_FG_Diffuse = 1 << 2,
    BXDF_FG_Glossy = 1 << 3,
    BXDF_FG_Specular = 1 << 4,

    BXDF_FG_DiffuseReflection = BXDF_FG_Diffuse | BXDF_FG_Reflection,
};

enum EBxDFReflectFlags
{
    BXDF_RF_Unset = 0,
    BXDF_RF_Reflection = 1 << 0,
    BXDF_RF_Transmission = 1 << 1,
    BXDF_RF_All = BXDF_RF_Reflection | BXDF_RF_Transmission
};

inline bool isNonSpecular(EBxDFFlags flag) { return flag & (EBxDFFlags::BXDF_FG_Diffuse | EBxDFFlags::BXDF_FG_Glossy); }

struct SBSDFSample
{
    SBSDFSample() = default;
    SBSDFSample(glm::vec3 f, glm::vec3 wi, float pdf, EBxDFFlags flag, float eta = 1.0)
        :f(f), wi(wi), eta(eta), pdf(pdf), flag(flag) {};

    inline bool isTransmission() { return false; };

    glm::vec3 f;
    glm::vec3 wi;
    float eta;
    float pdf = 0.0;
    EBxDFFlags flag;
};

enum ETransportMode
{
    TM_Radiance,
    TM_Importance,
};

// include BSDF and BTDF
class CBxDF
{
public:
    CBxDF() = default;
    virtual glm::vec3 f(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode) = 0;
    virtual float pdf(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag) = 0;
    virtual  std::shared_ptr<SBSDFSample> sample_f(glm::vec3 wo, float u, glm::vec2 u2, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag) = 0;
    virtual EBxDFFlags flags()const = 0;
private:
};

class CDiffuseBxDF : public CBxDF
{
public:
    CDiffuseBxDF(glm::vec3 reflectance)
        :reflectance(reflectance) {}

    inline glm::vec3 f(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode)
    {
        if (!sameHemiSphere(wo, wi))
        {
            return glm::vec3(0, 0, 0);
        }
        return reflectance;
    }

    inline float pdf(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag = EBxDFReflectFlags::BXDF_RF_Reflection)
    {
        if (!sameHemiSphere(wo, wi) || !(reflect_flag & EBxDFReflectFlags::BXDF_RF_Reflection))
        {
            return 0;
        }

        return cosineHemispherePDF(std::abs(wi.z));
    }

    inline std::shared_ptr<SBSDFSample> sample_f(glm::vec3 wo, float u, glm::vec2 u2, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag)
    {
        if (!(reflect_flag & EBxDFReflectFlags::BXDF_RF_Reflection))
        {
            return nullptr;
        }

        glm::vec3 wi = sampleConsineHemisphere(u2);
        if (wo.z < 0)
        {
            wi.z *= -1.0;
        }

        float pdf = cosineHemispherePDF(std::abs(wi.z));
        return std::make_shared<SBSDFSample>(reflectance, wi, pdf, EBxDFFlags::BXDF_FG_DiffuseReflection);
    }

    EBxDFFlags flags()const
    {
        return BXDF_FG_DiffuseReflection;
    }
private:
    glm::vec3 reflectance;
};

class CDielectricBxDF : public CBxDF
{
public:
    glm::vec3 f(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode);
    float pdf(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag);
    std::shared_ptr<SBSDFSample> sample_f(glm::vec3 wo, float u, glm::vec2 u2, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag);
    EBxDFFlags flags()const
    {
        assert(false);
        return BXDF_FG_None;
    }
};

