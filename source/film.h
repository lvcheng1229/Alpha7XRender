#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <vector>

class CRGBFilm
{
public:
	CRGBFilm(glm::u32vec2 ipt_img_sz)
		:image_size(ipt_img_sz) 
	{
		output_img.resize(image_size.y * image_size.x);
		memset(output_img.data(),0,sizeof(glm::vec3) * output_img.size());
	};

	inline glm::u32vec2 getImageSize()const { return image_size; }

	inline void addSample(glm::u32vec2 dst_pos,glm::vec3 L)
	{
		assert(dst_pos.x >= 0 && dst_pos.x <= image_size.x);
		assert(dst_pos.y >= 0 && dst_pos.y <= image_size.y);
		int write_idx = dst_pos.x + dst_pos.y * image_size.x;
		output_img[write_idx] += L;
	}

	inline void finalizeRender(float spp)
	{
		out_tga_data.resize(image_size.y * image_size.x);
		for (int idx_x = 0; idx_x < image_size.x; idx_x++)
		{
			for (int idx_y = 0; idx_y < image_size.y; idx_y++)
			{
				int dst_idx = idx_x + idx_y * image_size.x;
				glm::vec3 mapped_data = glm::pow(glm::vec3(output_img[dst_idx] / spp), glm::vec3(1.0f / 2.2f));
				glm::vec3 norm_data = glm::clamp(mapped_data, glm::vec3(0.0), glm::vec3(1.0)) * 255.0f;
				out_tga_data[dst_idx] = norm_data;
			}
		}
	}

	inline void* getFinalData() { return out_tga_data.data(); }

private:
	glm::u32vec2 image_size;
	std::vector<glm::vec3> output_img;
	std::vector<glm::u8vec3> out_tga_data;
};