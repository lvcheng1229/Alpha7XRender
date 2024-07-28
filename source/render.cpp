#include "render.h"
#include "integrators.h"

void renderScene(CAlpa7XScene& a7x_scene)
{
	CPerspectiveCamera* camera = a7x_scene.getCamera();
	CSampler* sampler = a7x_scene.getSampler();
	std::unique_ptr<CIntegrator> integrator = a7x_scene.createIntegrator(camera, sampler, std::vector<CLight*>());
	integrator->render();
}
