#include "bxdf.h"

glm::vec3 CDielectricBxDF::f(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode)
{
    assert(false);
    return glm::vec3();
}

float CDielectricBxDF::pdf(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag)
{
    assert(false);
    return 0.0f;
}

std::shared_ptr<SBSDFSample> CDielectricBxDF::sample_f(glm::vec3 wo, float u, glm::vec2 u2, ETransportMode transport_mode, EBxDFReflectFlags reflect_flag)
{
    assert(transport_mode == ETransportMode::TM_Radiance);
    assert(false);
    return nullptr;
}
