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
		vx = glm::normalize(glm::vec3(tran_mat[0].x, tran_mat[0].y, tran_mat[0].z));
		vy = glm::normalize(glm::vec3(tran_mat[1].x, tran_mat[1].y, tran_mat[1].z));
		glm::vec2 img_wh = glm::vec2(rgb_film->getImageSize().x, rgb_film->getImageSize().y);

		const float fov_scale = 1.0f / tanf(glm::radians(fov) * 0.5);
		film_left_bottom = -vx * 0.5f * img_wh.x -vy * 0.5f * img_wh.y + 0.5f * img_wh.y * fov_scale * glm::normalize(glm::vec3(tran_mat[2].x, tran_mat[2].y, tran_mat[2].z));
		camera_pos = glm::vec3(tran_mat[3].x, tran_mat[3].y, tran_mat[3].z);
	}

	inline CRGBFilm* getFilm() { return rgb_film; }
	
	inline glm::vec3 getCameraPos()const { return camera_pos; }
	inline glm::vec3 getPixelRayDirection(glm::vec2 pixel_pos) { return glm::normalize(glm::vec3(pixel_pos.x * vx + pixel_pos.y * vy + film_left_bottom)); };

private:
	// derived from the transform matrix
	glm::vec3 vx; // normalized
	glm::vec3 vy; // normalized
	glm::vec3 film_left_bottom; 
	glm::vec3 camera_pos;
	float fov;

	glm::mat4x4 camera_to_world;
	CRGBFilm* rgb_film;
};