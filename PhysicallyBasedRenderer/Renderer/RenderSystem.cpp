#include "RenderSystem.h"

#include "Camera/FPSCamera.h"
#include "Camera/TPSCamera.h"

#include <iostream>
#include <time.h>

using namespace std;

namespace Renderer
{
	void RenderSystem::resize(int width, int height)
	{
		m_width = width;
		m_height = height;
		// resize event.
		glViewport(0, 0, width, height);
		if(m_camera != nullptr)
			m_camera->changeAspect(static_cast<float>(width) / height);
	}

	void RenderSystem::initialize(int width, int height, int shadowWidth, int shadowHeight)
	{
		m_width = width;
		m_height = height;
		m_pointLightDrawable = nullptr;
		// shadow system.
		setSunLight(glm::vec3(0.1f, 1.0f, 0.3f), glm::vec3(0.6f));
		m_shadowSys = std::shared_ptr<ShadowSystem>(new ShadowSystem(shadowWidth, shadowHeight));

		// bloom effect.
		m_glowBlur = std::shared_ptr<GaussianBlur>(new GaussianBlur(m_width, m_height));

		// initialization.
		resize(width, height);
		m_meshMgr = MeshMgr::getSingleton();
		m_shaderMgr = ShaderMgr::getSingleton();
		m_textureMgr = TextureMgr::getSingleton();
		m_drawableList = make_shared<DrawableList>();

		// load built-in shaders.
		m_shaderMgr->loadShader("shadow", "./glsl/depth.vert", "./glsl/depth.frag");

		// defered shading.
		m_deferedShading = std::shared_ptr<DeferedShading>(new DeferedShading(m_width, m_height));
	}

	void RenderSystem::setSkyDome(const std::string & path, const std::string & pFix)
	{
		// set skydome background.
		if (m_skyDome != nullptr)
			return;
		unsigned int skyboxShader = m_shaderMgr->loadShader("skybox",
			"./glsl/skybox.vert", "./glsl/skybox.frag");
		unsigned int cubeTex = m_textureMgr->loadTextureCube("skybox", path, pFix);
		unsigned int mesh = m_meshMgr->loadMesh(new Sphere(1.0f, 10, 10));
		m_skyDome = make_shared<SkyDome>(skyboxShader);
		m_skyDome->addMesh(mesh);
		PBRMaterial mat;
		mat.m_albedoTexIndex = cubeTex;
		m_skyDome->addPbrTexture(mat);
	}

	Camera3D::ptr RenderSystem::createFPSCamera(glm::vec3 pos, glm::vec3 target)
	{
		// create a first person camera.
		FPSCamera *_cam = new FPSCamera(pos);
		_cam->lookAt(glm::normalize(target - pos), Camera3D::LocalUp);
		m_camera = shared_ptr<Camera3D>(_cam);
		return m_camera;
	}

	Camera3D::ptr RenderSystem::createTPSCamera(glm::vec3 pos, glm::vec3 target)
	{
		// create a third person camera.
		TPSCamera *_cam = new TPSCamera(target, 0.0f, 30.0f, 3.0f);
		m_camera = shared_ptr<Camera3D>(_cam);
		return m_camera;
	}

	void RenderSystem::setClearMask(GLbitfield mask)
	{
		m_renderState.m_clearMask = mask;
	}

	void RenderSystem::setClearColor(glm::vec4 clearColor)
	{
		m_renderState.m_clearColor = clearColor;
	}

	void RenderSystem::setCullFace(bool enable, GLenum face)
	{
		m_renderState.m_cullFace = enable;
		m_renderState.m_cullFaceMode = face;
	}

	void RenderSystem::setDepthTest(bool enable, GLenum func)
	{
		m_renderState.m_depthTest = enable;
		m_renderState.m_depthFunc = func;
	}

	void RenderSystem::addPointLight(glm::vec3 pos, glm::vec3 radiance)
	{
		if (m_pointLights.size() >= 128)
			return;
		PointLight::ptr pointLight = std::shared_ptr<PointLight>(new PointLight());
		pointLight->setPosition(pos, m_pointLights.size());
		pointLight->setLightColor(radiance);
		m_pointLights.push_back(pointLight);
	}

	void RenderSystem::setSunLight(glm::vec3 dir, glm::vec3 radiance)
	{
		DirectionalLight *light = new DirectionalLight();
		light->setDirection(dir);
		light->setLightColor(radiance);
		m_sunLight = shared_ptr<DirectionalLight>(light);
	}

	void RenderSystem::setPolygonMode(GLenum mode)
	{
		m_renderState.m_polygonMode = mode;
	}

	void RenderSystem::render()
	{
		if (m_drawableList == nullptr)
			return;
		// render the shadow.
		m_shadowSys->renderShadow(m_width, m_height, m_drawableList, m_sunLight);

		// point light objects genration.
		if (m_pointLightDrawable == nullptr && m_pointLights.size() > 0)
		{
			m_pointLightDrawable = std::shared_ptr<PointLightDrawable>
				(new PointLightDrawable());
			m_pointLightDrawable->setPointLightRadius(0.7f);
			m_pointLightDrawable->setPointLightPositions(m_pointLights);
		}
		updatePointLightPosition();
		
		// render to g-buffers.
		{
			m_deferedShading->bindDeferedFramebuffer();

			glClearColor(0.0, 0.0, 0.0, 1.0f);
			glClear(m_renderState.m_clearMask);

			// polygon mode.
			glPolygonMode(GL_FRONT_AND_BACK, m_renderState.m_polygonMode);

			// cullface setting.
			if (m_renderState.m_cullFace)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
			glCullFace(m_renderState.m_cullFaceMode);

			// depth testing setting.
			if (m_renderState.m_depthTest)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
			glDepthFunc(m_renderState.m_depthFunc);

			// render the drawable list.
			m_drawableList->render(m_camera, m_sunLight, m_shadowSys->getLightCamera());

			// render the light source.
			if (m_pointLightDrawable != nullptr)
				m_pointLightDrawable->render(m_camera, m_sunLight, nullptr, nullptr);
		}

		// ssao.
		m_deferedShading->ssaoFactorGeneration(m_camera);

		// defered shading and forward shading.
		m_glowBlur->bindGaussianFramebuffer();
		{
			glDisable(GL_BLEND);
			glDisable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_TRUE);
			glClearColor(1.0, 1.0, 1.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_deferedShading->renderDeferedShading(m_camera, m_sunLight,
				m_shadowSys->getLightCamera(), m_pointLights);

			// render the skydome.
			if (m_skyDome != nullptr)
			{
				glEnable(GL_DEPTH_TEST);
				glCullFace(GL_FRONT);
				m_skyDome->render(m_camera, m_sunLight, nullptr);
			}
		}

		// bloom effect.
		m_glowBlur->renderGaussianBlurEffect();
	}

	// just move light source for demo.
	void RenderSystem::updatePointLightPosition()
	{
		// generate random velocity.
		if (m_rndVelForPointLights.empty())
		{
			m_rndVelForPointLights.resize(m_pointLights.size());
			srand(time(nullptr));
			for (unsigned int x = 0; x < m_pointLights.size(); ++x)
			{
				glm::vec3 vel;
				vel.x = (((double)rand()) / RAND_MAX) * 2.0f - 1.0f;
				//vel.y = (((double)rand()) / RAND_MAX) * 2.0f - 1.0f;
				vel.y = 0.0f;
				vel.z = (((double)rand()) / RAND_MAX) * 2.0f - 1.0f;
				m_rndVelForPointLights[x] = vel;
			}
		}

		const float speed = 0.2f;
		for (unsigned int x = 0; x < m_pointLights.size(); ++x)
		{
			glm::vec3 pos = m_pointLights[x]->getPosition();
			pos += m_rndVelForPointLights[x] * speed;
			bool update = false;
			if (pos.x > 200.0f)
			{
				pos.x = 200.0f;
				update = true;
			}
			if (pos.x < -200.0f)
			{
				pos.x = -200.0f;
				update = true;
			}
			if (pos.z > 200.0f)
			{
				pos.z = 200.0f;
				update = true;
			}
			if (pos.z < -200.0f)
			{
				pos.z = -200.0f;
				update = true;
			}
			if (update)
				m_rndVelForPointLights[x] *= (-1);
			m_pointLights[x]->setPosition(pos, x);
		}
		m_pointLightDrawable->setPointLightPositions(m_pointLights);
	}

}