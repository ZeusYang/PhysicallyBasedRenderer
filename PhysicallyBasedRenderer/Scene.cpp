#include "Scene.h"

#include <time.h>
#include "Renderer/Drawable/StaticModelDrawable.h"

using namespace Renderer;

void SponzaScene::initializeScene(Renderer::RenderSystem::ptr renderSys)
{
	// resource manager.
	MeshMgr::ptr meshMgr = renderSys->getMeshMgr();
	ShaderMgr::ptr shaderMgr = renderSys->getShaderMgr();
	TextureMgr::ptr textureMgr = renderSys->getTextureMgr();

	// shaders.
	unsigned int blinPhongShader = shaderMgr->loadShader("blinn_phong",
		"./glsl/blinn_phong.vert", "./glsl/blinn_phong.frag");

	unsigned int pbrShader = shaderMgr->loadShader("pbrShader",
		"./glsl/pbrShader.vert", "./glsl/pbrShader.frag");

	StaticModelDrawable *sponza = new StaticModelDrawable(pbrShader,
		"./res/sponza_pbr/sponza_pbr.obj");
	//StaticModelDrawable *spaceShip = new StaticModelDrawable(pbrShader,
	//	"./res/SpaceShip/Intergalactic_Spaceship-(Wavefront).obj");
	StaticModelDrawable *seashell = new StaticModelDrawable(pbrShader,
		"./res/seashell_obj/seashell_obj.obj");
	StaticModelDrawable *knife = new StaticModelDrawable(pbrShader,
		"./res/knife/KnifeLow.obj");
	StaticModelDrawable *FelLord = new StaticModelDrawable(pbrShader,
		"./res/FelLord/Full_low.obj");
	sponza->getTransformation()->setScale(glm::vec3(0.2, 0.2, 0.2));
	//spaceShip->getTransformation()->setScale(glm::vec3(10.0f));
	//spaceShip->getTransformation()->translate(glm::vec3(0, 10, 0));
	seashell->getTransformation()->translate(glm::vec3(60, 0, 0));
	seashell->getTransformation()->setScale(glm::vec3(0.5f));
	knife->getTransformation()->translate(glm::vec3(130, 0, 0));
	knife->getTransformation()->setScale(glm::vec3(3.0f));
	knife->getTransformation()->rotate(glm::vec3(0.0f, 1.0f, 0.0f), 90.0f);
	FelLord->getTransformation()->setScale(glm::vec3(10.0f));
	FelLord->getTransformation()->translate(glm::vec3(110.0, 20.0, 0.0));
	FelLord->getTransformation()->rotate(glm::vec3(0, 1, 0), 270);
	renderSys->addDrawable(sponza);
	//renderSys->addDrawable(spaceShip);
	renderSys->addDrawable(seashell);
	renderSys->addDrawable(knife);
	renderSys->addDrawable(FelLord);

	renderSys->setSunLight(glm::vec3(0.1f, 1.0f, 0.3f), glm::vec3(1.0f));

	srand(time(nullptr));
	for (unsigned int x = 0; x < 128; ++x)
	{
		glm::vec3 pos;
		glm::vec3 radiance;
		pos.x = -200.0f + (((double)rand()) / RAND_MAX) * 400.0f;
		pos.y = -200.0f + (((double)rand()) / RAND_MAX) * 400.0f;
		pos.y = 2.5f;
		pos.z = -200.0f + (((double)rand()) / RAND_MAX) * 400.0f;
		radiance.x = (((double)rand()) / RAND_MAX) * 80;
		radiance.y = (((double)rand()) / RAND_MAX) * 80;
		radiance.z = (((double)rand()) / RAND_MAX) * 80;
		//radiance.x = 80.0f;
		//radiance.y = 0.0;
		//radiance.z = 0.0;

		renderSys->addPointLight(pos, radiance);
	}
}

void FelLordScene::initializeScene(Renderer::RenderSystem::ptr renderSys)
{
	// resource manager.
	MeshMgr::ptr meshMgr = renderSys->getMeshMgr();
	ShaderMgr::ptr shaderMgr = renderSys->getShaderMgr();
	TextureMgr::ptr textureMgr = renderSys->getTextureMgr();

	// shaders.
	unsigned int pbrShader = shaderMgr->loadShader("pbrShader",
		"./glsl/pbrShader.vert", "./glsl/pbrShader.frag");

	StaticModelDrawable *FelLord = new StaticModelDrawable(pbrShader,
		"./res/FelLord/Full_low.obj");
	StaticModelDrawable *lamp = new StaticModelDrawable(pbrShader,
		"./res/lamp/lamp.obj");
	StaticModelDrawable *lantern = new StaticModelDrawable(pbrShader,
		"./res/lantern/lantern_obj.obj");
	StaticModelDrawable *barrel = new StaticModelDrawable(pbrShader,
		"./res/barrel/barrels_obj.obj");
	StaticModelDrawable *seashell = new StaticModelDrawable(pbrShader,
		"./res/seashell_obj/seashell_obj.obj");
	FelLord->getTransformation()->setScale(glm::vec3(10.0f));
	FelLord->getTransformation()->translate(glm::vec3(40.0, 20.0, 0.0));
	lamp->getTransformation()->setScale(glm::vec3(4.0f));
	lantern->getTransformation()->setScale(glm::vec3(0.6f));
	lantern->getTransformation()->translate(glm::vec3(-100, 0, -50));
	barrel->getTransformation()->setScale(glm::vec3(0.5f));
	barrel->getTransformation()->translate(glm::vec3(+100, 0, -50));
	seashell->getTransformation()->translate(glm::vec3(90, 0, 90));
	seashell->getTransformation()->setScale(glm::vec3(0.5f));
	//FelLord->getTransformation()->rotate(glm::vec3(0, 1, 0), 135.0f);
	renderSys->addDrawable(FelLord);
	renderSys->addDrawable(lamp);
	renderSys->addDrawable(lantern);
	renderSys->addDrawable(barrel);
	renderSys->addDrawable(seashell);

	srand(time(nullptr));
	for (unsigned int x = 0; x < 128; ++x)
	{
		glm::vec3 pos;
		glm::vec3 radiance;
		pos.x = -200.0f + (((double)rand()) / RAND_MAX) * 400.0f;
		pos.y = -200.0f + (((double)rand()) / RAND_MAX) * 400.0f;
		pos.y = 3.0f;
		pos.z = -200.0f + (((double)rand()) / RAND_MAX) * 400.0f;
		radiance.x = (((double)rand()) / RAND_MAX) * 80;
		radiance.y = (((double)rand()) / RAND_MAX) * 80;
		radiance.z = (((double)rand()) / RAND_MAX) * 80;

		renderSys->addPointLight(pos, radiance);
	}

	renderSys->setSunLight(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f));
}

void HatkaScene::initializeScene(Renderer::RenderSystem::ptr renderSys)
{
	// resource manager.
	MeshMgr::ptr meshMgr = renderSys->getMeshMgr();
	ShaderMgr::ptr shaderMgr = renderSys->getShaderMgr();
	TextureMgr::ptr textureMgr = renderSys->getTextureMgr();

	// shaders.
	unsigned int pbrShader = shaderMgr->loadShader("pbrShader",
		"./glsl/pbrShader.vert", "./glsl/pbrShader.frag");

	StaticModelDrawable *hatka = new StaticModelDrawable(pbrShader,
		"./res/hatka_local_/hatka_local_.obj");
	//hatka->getTransformation()->setScale(glm::vec3(10.0f));
	//hatka->getTransformation()->translate(glm::vec3(40.0, 20.0, 0.0));
	renderSys->addDrawable(hatka);

	srand(time(nullptr));
	for (unsigned int x = 0; x < 128; ++x)
	{
		glm::vec3 pos;
		glm::vec3 radiance;
		pos.x = -200.0f + (((double)rand()) / RAND_MAX) * 400.0f;
		pos.y = -200.0f + (((double)rand()) / RAND_MAX) * 400.0f;
		pos.y = 3.0f;
		pos.z = -200.0f + (((double)rand()) / RAND_MAX) * 400.0f;
		radiance.x = (((double)rand()) / RAND_MAX) * 80;
		radiance.y = (((double)rand()) / RAND_MAX) * 80;
		radiance.z = (((double)rand()) / RAND_MAX) * 80;

		renderSys->addPointLight(pos, radiance);
	}

	renderSys->setSunLight(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f));
}
