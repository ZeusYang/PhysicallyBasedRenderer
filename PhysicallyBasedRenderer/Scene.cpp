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
	StaticModelDrawable *seashell = new StaticModelDrawable(pbrShader,
		"./res/seashell_obj/seashell_obj.obj");
	StaticModelDrawable *knife = new StaticModelDrawable(pbrShader,
		"./res/knife/KnifeLow.obj");
	StaticModelDrawable *FelLord = new StaticModelDrawable(pbrShader,
		"./res/FelLord/Full_low.obj");
	sponza->getTransformation()->setScale(glm::vec3(0.2, 0.2, 0.2));
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

	//StaticModelDrawable *FelLord = new StaticModelDrawable(pbrShader,
	//	"./res/FelLord/Full_low.obj");
	StaticModelDrawable *lamp = new StaticModelDrawable(pbrShader,
		"./res/lamp/lamp.obj");
	StaticModelDrawable *barrel = new StaticModelDrawable(pbrShader,
		"./res/barrel/barrels_obj.obj");
	StaticModelDrawable *seashell = new StaticModelDrawable(pbrShader,
		"./res/seashell_obj/seashell_obj.obj");

	// 
	unsigned int planeMeshIndex = meshMgr->loadMesh(new Plane(900, 900));
	unsigned int sphereMeshIndex = meshMgr->loadMesh(new Sphere(10, 25, 25));

	PBRMaterial mat;
	mat.m_albedoTexIndex = textureMgr->loadTexture2D("scuffed_basecolor",
		"./res/copper_scuffed/Copper-scuffed_basecolor.png");
	mat.m_metallicIndex = textureMgr->loadTexture2D("scuffed_metallic",
		"./res/copper_scuffed/Copper-scuffed_metallic.png");
	mat.m_normalTexIndex = textureMgr->loadTexture2D("scuffed_normal",
		"./res/copper_scuffed/Copper-scuffed_normal.png");
	mat.m_roughTexIndex = textureMgr->loadTexture2D("scuffed_roughness",
		"./res/copper_scuffed/Copper-scuffed_roughness.png");
	// material sphere.
	PBRMaterial spMat;
	spMat.m_albedoTexIndex = textureMgr->loadTexture2D("greasy-pan-2-albedo",
		"./res/greasy-pan-2/greasy-pan-2-albedo.png");
	spMat.m_metallicIndex = textureMgr->loadTexture2D("greasy-pan-2-metal",
		"./res/greasy-pan-2/greasy-pan-2-metal.png");
	spMat.m_normalTexIndex = textureMgr->loadTexture2D("greasy-pan-2-normal",
		"./res/greasy-pan-2/greasy-pan-2-normal.png");
	spMat.m_roughTexIndex = textureMgr->loadTexture2D("greasy-pan-2-roughness",
		"./res/greasy-pan-2/greasy-pan-2-roughness.png");
	PBRMaterial spMat1;
	spMat1.m_albedoTexIndex = textureMgr->loadTexture2D("streaked-metal1-rough",
		"./res/streaked-metal1/streaked-metal1-albedo.png");
	spMat1.m_metallicIndex = textureMgr->loadTexture2D("streaked-metal1-metalness",
		"./res/streaked-metal1/streaked-metal1-metalness.png");
	spMat1.m_normalTexIndex = textureMgr->loadTexture2D("streaked-metal1-normal-dx",
		"./res/streaked-metal1/streaked-metal1-normal-dx.png");
	spMat1.m_roughTexIndex = textureMgr->loadTexture2D("streaked-metal1-rough",
		"./res/streaked-metal1/streaked-metal1-rough.png");
	PBRMaterial militaryMat;
	militaryMat.m_albedoTexIndex = textureMgr->loadTexture2D("military-panel1-albedo",
		"./res/military-panel/military-panel1-albedo.png");
	militaryMat.m_metallicIndex = textureMgr->loadTexture2D("military-panel1-metalness",
		"./res/military-panel/military-panel1-metalness.png");
	militaryMat.m_normalTexIndex = textureMgr->loadTexture2D("military-panel1-nmap-dx",
		"./res/military-panel/military-panel1-nmap-dx.png");
	militaryMat.m_roughTexIndex = textureMgr->loadTexture2D("military-panel1-rough",
		"./res/military-panel/military-panel1-rough.png");
	PBRMaterial brickMat;
	brickMat.m_albedoTexIndex = textureMgr->loadTexture2D("harshbricks-albedo",
		"./res/harshbricks/harshbricks-albedo.png");
	brickMat.m_metallicIndex = textureMgr->loadTexture2D("harshbricks-metalness",
		"./res/harshbricks/harshbricks-metalness.png");
	brickMat.m_normalTexIndex = textureMgr->loadTexture2D("harshbricks-normal",
		"./res/harshbricks/harshbricks-normal.png");
	brickMat.m_roughTexIndex = textureMgr->loadTexture2D("harshbricks-roughness",
		"./res/harshbricks/harshbricks-roughness.png");
	// floor material.
	PBRMaterial floorMat;
	floorMat.m_albedoTexIndex = textureMgr->loadTexture2D("bathroomtile2-basecolor",
		"./res/floorMat/bathroomtile2-basecolor.png");
	floorMat.m_metallicIndex = textureMgr->loadTexture2D("bathroomtile2-metalness",
		"./res/floorMat/bathroomtile2-metalness.png");
	floorMat.m_normalTexIndex = textureMgr->loadTexture2D("bathroomtile2-normal-dx",
		"./res/floorMat/bathroomtile2-normal-dx.png");
	floorMat.m_roughTexIndex = textureMgr->loadTexture2D("bathroomtile2-roughness",
		"./res/floorMat/bathroomtile2-roughness.png");

	SimpleDrawable *sphere1 = new SimpleDrawable(pbrShader);
	SimpleDrawable *sphere2 = new SimpleDrawable(pbrShader);
	SimpleDrawable *sphere3 = new SimpleDrawable(pbrShader);
	SimpleDrawable *sphere4 = new SimpleDrawable(pbrShader);
	SimpleDrawable *sphere5 = new SimpleDrawable(pbrShader);
	sphere1->addMesh(sphereMeshIndex);
	sphere2->addMesh(sphereMeshIndex);
	sphere3->addMesh(sphereMeshIndex);
	sphere4->addMesh(sphereMeshIndex);
	sphere5->addMesh(sphereMeshIndex);
	sphere1->addPbrTexture(mat);
	sphere2->addPbrTexture(spMat);
	sphere3->addPbrTexture(spMat1);
	sphere4->addPbrTexture(militaryMat);
	sphere5->addPbrTexture(brickMat);

	SimpleDrawable *floor = new SimpleDrawable(pbrShader);
	floor->addMesh(planeMeshIndex);
	floor->addPbrTexture(floorMat);

	sphere1->getTransformation()->translate(glm::vec3(0, 10, 50));
	sphere2->getTransformation()->translate(glm::vec3(20, 10, 50));
	sphere3->getTransformation()->translate(glm::vec3(-20, 10, 50));
	sphere4->getTransformation()->translate(glm::vec3(-40, 10, 50));
	sphere5->getTransformation()->translate(glm::vec3(+40, 10, 50));

	//FelLord->getTransformation()->setScale(glm::vec3(10.0f));
	//FelLord->getTransformation()->translate(glm::vec3(40.0, 20.0, 0.0));
	lamp->getTransformation()->setScale(glm::vec3(4.0f));
	barrel->getTransformation()->setScale(glm::vec3(0.5f));
	barrel->getTransformation()->translate(glm::vec3(+100, 0, -50));
	seashell->getTransformation()->translate(glm::vec3(90, 0, 90));
	seashell->getTransformation()->setScale(glm::vec3(0.5f));
	//renderSys->addDrawable(FelLord);
	renderSys->addDrawable(lamp);
	renderSys->addDrawable(barrel);
	renderSys->addDrawable(seashell);
	renderSys->addDrawable(floor);
	renderSys->addDrawable(sphere1);
	renderSys->addDrawable(sphere2);
	renderSys->addDrawable(sphere3);
	renderSys->addDrawable(sphere4);
	renderSys->addDrawable(sphere5);

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