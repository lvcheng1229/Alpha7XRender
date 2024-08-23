#include "bxdf.h"

inline bool refract(glm::vec3 wi, glm::vec3 n, float eta, float* etap, glm::vec3* wt) 
{
    float cos_theta_i = glm::dot(n, wi);
    
    if (cos_theta_i < 0) 
    {
        eta = 1 / eta;
        cos_theta_i = -cos_theta_i;
        n = -n;
    }

    float sin2_theta_i = std::max<float>(0, 1 - (cos_theta_i * cos_theta_i));
    float sin2_theta_t = sin2_theta_i / (eta * eta);
    if (sin2_theta_t >= 1) { return false; }
    
    float cosTheta_t = std::sqrt(1 - sin2_theta_t);

    *wt = -wi / eta + (cos_theta_i / eta - cosTheta_t) * glm::vec3(n);
    if (etap) { *etap = eta; };

    return true;
}

inline float fresnelDielectric(float cos_theta_i, float eta) 
{
    cos_theta_i = glm::clamp(cos_theta_i, -1.0f, 1.0f);
    
    if (cos_theta_i < 0) 
    {
        eta = 1 / eta;
        cos_theta_i = -cos_theta_i;
    }

    float sin2_theta_i = 1 - cos_theta_i * cos_theta_i;
    float sin2_theta_t = sin2_theta_i / (eta * eta);
    if (sin2_theta_t >= 1) { return 1.f; }

    float cos_theta_t = std::sqrt(std::max(0.f, (1 - sin2_theta_t)));

    float r_parl = (eta * cos_theta_i - cos_theta_t) / (eta * cos_theta_i + cos_theta_t);
    float r_perp = (cos_theta_i - eta * cos_theta_t) / (cos_theta_i + eta * cos_theta_t);
    return ((r_parl * r_parl) + (r_perp * r_perp)) / 2;
}

glm::vec3 CDielectricBxDF::f(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode)
{
    if (eta == 1 || tdr_distrib.isAbsoluteSpecular()) { return glm::vec3(0, 0, 0); }

    assert(false);
    return glm::vec3();
}

float CDielectricBxDF::pdf(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode, EBxDFReflTransFlags reflect_flag)
{
    if (eta == 1 || tdr_distrib.isAbsoluteSpecular()) { return 0.0f; }

    assert(false);
    return 0.0f;
}

std::shared_ptr<SBSDFSample> CDielectricBxDF::sample_f(glm::vec3 wo, float u, glm::vec2 u2, ETransportMode transport_mode, EBxDFReflTransFlags refle_trans_flag)
{
    if (eta == 1 || tdr_distrib.isAbsoluteSpecular())
    {
        float reflection = fresnelDielectric((wo.z/*cos theta*/), eta);
        float transmission = 1 - reflection;

        float prob_r = reflection;
        float prob_t = transmission;

        if (!(refle_trans_flag & EBxDFReflTransFlags::BXDF_RF_Reflection)) { prob_r = 0; }
        if (!(refle_trans_flag & EBxDFReflTransFlags::BXDF_RF_Transmission)) { prob_t = 0; }
        if (prob_t == 0 && prob_t == 0) { return nullptr; }
            
        if (u < prob_r / (prob_r + prob_t)) // reflection
        {
            glm::vec3 wi(-wo.x, -wo.y, wo.z);
            glm::vec3 fr(reflection / std::abs(wi.z/*cos theta*/));
            return std::make_shared<SBSDFSample>(fr, wi, prob_r / (prob_r + prob_t), EBxDFFlags::BXDF_FG_SpecularReflection);
        }
        else 
        {
            glm::vec3 wi;
            float etap;
            bool valid = refract(wo, glm::vec3(0, 0, 1), eta, &etap, &wi);
            if (!valid) { return nullptr; };
            glm::vec3 ft(transmission / std::abs(wi.z/*cos theta*/));
            if (transport_mode == ETransportMode::TM_Radiance) { ft /= etap * etap; }
            return std::make_shared<SBSDFSample>(ft, wi, prob_t / (prob_r + prob_t), EBxDFFlags::BXDF_FG_SpecularTransmission, etap);
        }

    }
    assert(false);
    return nullptr;
}
