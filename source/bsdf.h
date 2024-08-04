#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "bxdf.h"

class CBSDF
{
public:
	CBSDF() :normal(glm::vec3(0, 0, 0)) {};
	CBSDF(glm::vec3 ipt_normal, std::shared_ptr<CBxDF> ipt_bxdf)
		:normal(ipt_normal)
		, bxdf(ipt_bxdf) {};

	glm::vec3 f(glm::vec3 wo, glm::vec3 wi, ETransportMode transport_mode = ETransportMode::TM_Radiance)const
	{
		glm::vec3 wo_local = tangen_basis.toLocal(wo);
		glm::vec3 wi_local = tangen_basis.toLocal(wi);
		if (wo_local.z == 0)
		{
			return glm::vec3(0,0,0);
		}
		return bxdf->f(wo_local, wi_local, transport_mode);
	}

	float pdf(glm::vec3 ipt_wo, glm::vec3 ipt_wi, ETransportMode transport_mode = ETransportMode::TM_Radiance, EBxDFReflectFlags sample_flags = EBxDFReflectFlags::BXDF_RF_All)const
	{
		glm::vec3 wo_local = tangen_basis.toLocal(ipt_wo);
		glm::vec3 wi_local = tangen_basis.toLocal(ipt_wi);
		if (wo_local.z == 0)
		{
			return 0;
		}
		return bxdf->pdf(wo_local,wi_local, transport_mode, sample_flags);
	}

	std::shared_ptr<SBSDFSample> sample_f(glm::vec3 wo_world, float u, glm::vec2 u2, ETransportMode transport_mode = ETransportMode::TM_Radiance, EBxDFReflectFlags flags = EBxDFReflectFlags::BXDF_RF_All)
	{
		glm::vec3 wo_local = tangen_basis.toLocal(wo_world);
		if (wo_local.z == 0 || !(bxdf->flags() & BXDF_RF_All))
		{
			return nullptr;
		}
		std::shared_ptr<SBSDFSample> bs = bxdf->sample_f(wo_local, u, u2, transport_mode, flags);
		bs->wi = tangen_basis.fromLocal(bs->wi);
		return bs;
	}

private:
	CFTangentBasis  tangen_basis;
	glm::vec3 normal;
	std::shared_ptr<CBxDF> bxdf;
};
