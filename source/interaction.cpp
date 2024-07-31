#include "interaction.h"

CBSDF CSurfaceInterraction::getBSDF()
{
    if (!material)
    {
        return CBSDF();
    }

    CBSDF bsdf = CBSDF(norm, material->getBxdf());
    return bsdf;
}
