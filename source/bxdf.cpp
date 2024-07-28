#include "bxdf.h"
float CDiffuseBxDF::pdf(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag)
{
    return 0.0f;
}

glm::vec3 CDielectricBxDF::f(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode)
{
    return glm::vec3();
}

float CDielectricBxDF::pdf(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag)
{
    return 0.0f;
}

SBSDFSample* CDielectricBxDF::sample_f(glm::vec3 wo, float u, glm::vec2 u2, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag)
{
    return nullptr;
}
