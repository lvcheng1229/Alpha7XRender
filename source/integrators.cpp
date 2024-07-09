#include "integrators.h"

void CPathIntegrator::render()
{
	CRGBFilm* rgb_film = camera->getFilm();
	const glm::u32vec2 image_size = rgb_film->getImageSize();
	const int image_area = image_size[0] * image_size[1];
	
	int spp = sampler->getSamplersPerPixel();

	for (int spp_idx = 0; spp_idx < spp; spp_idx++)
	{
		for (int pixel_idx = 0; pixel_idx < image_area; pixel_idx++)
		{
			glm::u32vec2 pixel_idx_2d = glm::u32vec2(pixel_idx % image_size[0], pixel_idx / image_size[1]);

		}
	}



}
