#pragma once
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
#include "film.h"

class CPerspectiveCamera
{
public:
	CPerspectiveCamera(glm::mat4x4 tran_mat, float ipt_fov, CRGBFilm* ipt_rgb_film)
		:camera_to_world(tran_mat)
		, fov(ipt_fov)
		, rgb_film(ipt_rgb_film) 
	{

		// tran_mat = camera from world
		glm::mat4 world_from_camera = glm::inverse(tran_mat);

		vx = glm::normalize(glm::vec3(world_from_camera[0].x, world_from_camera[0].y, world_from_camera[0].z));
		vy = glm::normalize(glm::vec3(world_from_camera[1].x, world_from_camera[1].y, world_from_camera[1].z));
		glm::vec2 img_wh = glm::vec2(rgb_film->getImageSize().x, rgb_film->getImageSize().y);

		const float fov_scale = 1.0f / tanf(glm::radians(fov) * 0.5);
		film_right_bottom = -vx * 0.5f * img_wh.x - vy * 0.5f * img_wh.y + 0.5f * img_wh.y * fov_scale * glm::normalize(glm::vec3(world_from_camera[2].x, world_from_camera[2].y, world_from_camera[2].z));
		camera_pos = glm::vec3(world_from_camera[3].x, world_from_camera[3].y, world_from_camera[3].z);
	}

	inline CRGBFilm* getFilm() { return rgb_film; }
	
	inline glm::vec3 getCameraPos()const { return camera_pos; }
	inline glm::vec3 getPixelRayDirection(glm::vec2 pixel_pos) { return glm::normalize(glm::vec3(pixel_pos.x * vx + pixel_pos.y * vy + film_right_bottom)); };

private:
	// derived from the transform matrix
	glm::vec3 vx; // normalized
	glm::vec3 vy; // normalized
	glm::vec3 film_right_bottom;
	glm::vec3 camera_pos;
	float fov;

	glm::mat4x4 camera_to_world;
	CRGBFilm* rgb_film;
};