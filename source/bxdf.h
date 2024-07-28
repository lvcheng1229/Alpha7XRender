#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "alpha7x_math.h"

struct SBSDFSample
{
    inline bool isTransmission() { return false; };

    glm::vec3 f;
    glm::vec3 wi;
    float eta;
    float pdf;
};

enum EBxDFFlags
{
	BXDF_FG_None = 0,
    BXDF_FG_Reflection = 1 << 0,
    BXDF_FG_Transmission = 1 << 1,
    BXDF_FG_Diffuse = 1 << 2,
    BXDF_FG_Glossy = 1 << 3,
    BXDF_FG_Specular = 1 << 4,
};

enum EBxDFReflectFlags
{
    BXDF_RF_Unset = 0,
    BXDF_RF_Reflection = 1 << 0,
    BXDF_RF_Transmission = 1 << 1,
    BXDF_RF_All = BXDF_RF_Reflection | BXDF_RF_Transmission
};

inline bool isNonSpecular(EBxDFFlags flag) { return flag & (EBxDFFlags::BXDF_FG_Diffuse | EBxDFFlags::BXDF_FG_Glossy); }

enum class ETransportMode
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
    virtual SBSDFSample* sample_f(glm::vec3 wo, float u, glm::vec2 u2, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag) = 0;
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

    float pdf(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag);

    SBSDFSample* sample_f(glm::vec3 wo, float u, glm::vec2 u2, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag)
    {
        if (!(reflect_flag & EBxDFReflectFlags::BXDF_RF_Reflection))
        {
            return nullptr;
        }
        assert(false);
        return nullptr;
    }

private:
    glm::vec3 reflectance;
};

class CDielectricBxDF : public CBxDF
{
public:
    glm::vec3 f(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode);
    float pdf(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag);
    SBSDFSample* sample_f(glm::vec3 wo, float u, glm::vec2 u2, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag);
};

