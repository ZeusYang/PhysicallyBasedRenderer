#include <iostream>

#include "Renderer/RenderDevice.h"
#include "Renderer/Camera/TPSCamera.h"
#include "Renderer/Manager/Geometry.h"
#include "Renderer/Drawable/StaticModelDrawable.h"

#include "Scene.h"

using namespace std;
using namespace Renderer;

int main(int argc, char *argv[])
{
	// initialization.
	const int width = 1280, height = 780;
	RenderDevice::ptr window = RenderDevice::getSingleton();
	window->initialize("PBR Renderer", width, height, false);
	RenderSystem::ptr renderSys = window->getRenderSystem();
	
	//renderSys->setSkyDome("./res/skybox/", ".jpg");

	// camera
	//Camera3D::ptr camera = renderSys->createFPSCamera(
	//	glm::vec3(0, 50, 100),
	//	glm::vec3(0.0f));
	Camera3D::ptr camera = renderSys->createTPSCamera(
		glm::vec3(0, 100, 200),
		glm::vec3(0.0f));
	camera->setPerspectiveProject(45.0f, static_cast<float>(width) / height, 0.1f, 3000.0f);
	reinterpret_cast<TPSCamera*>(camera.get())->setWheelSensitivity(50);
	reinterpret_cast<TPSCamera*>(camera.get())->setDistanceLimt(0.1f, 300);
	reinterpret_cast<TPSCamera*>(camera.get())->setDistance(200);
	reinterpret_cast<TPSCamera*>(camera.get())->setPitch(10);
	//reinterpret_cast<FPSCamera*>(camera.get())->setMoveSpeed(150);

	//Scene::ptr sponza = shared_ptr<SponzaScene>(new SponzaScene());
	//sponza->initializeScene(renderSys);

	Scene::ptr fellord = shared_ptr<FelLordScene>(new FelLordScene());
	fellord->initializeScene(renderSys);

	//Scene::ptr hatka = shared_ptr<HatkaScene>(new HatkaScene());
	//hatka->initializeScene(renderSys);

	// set sky map.
	renderSys->setSkyDomeHdr("./res/Hdr/Mans_Outside_2k.hdr");

	while (window->run())
	{
		window->beginFrame();

		renderSys->render();

		window->endFrame();
	}

	window->shutdown();

	return 0;
}