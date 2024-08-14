#include "render.h"
#include "integrators.h"

void renderScene(CAlpa7XScene& a7x_scene)
{
	std::vector<std::shared_ptr<CLight>> lights;
	CPerspectiveCamera* camera = a7x_scene.getCamera();
	CSampler* sampler = a7x_scene.getSampler();
	CAccelerator* accel = a7x_scene.createAccelerator(lights);
	std::unique_ptr<CIntegrator> integrator = a7x_scene.createIntegrator(camera, sampler, accel, lights);
	integrator->render();
}
