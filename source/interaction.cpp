#include "interaction.h"

CBSDF CSurfaceInterraction::getBSDF()
{
    if (!material)
    {
        assert(false);
        return CBSDF();
    }

    CBSDF bsdf = CBSDF(norm, material->getBxdf());
    return bsdf;
}
