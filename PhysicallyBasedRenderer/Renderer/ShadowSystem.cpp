#include "ShadowSystem.h"

#include "Manager/ShaderMgr.h"
#include "Camera/FPSCamera.h"
#include "Drawable/AABBBoundingBox.h"

#include <iostream>

namespace Renderer
{
	ShadowSystem::ShadowSystem(unsigned int width, unsigned int height)
		: m_first(true)
	{
		m_lightCamera = nullptr;
		FrameBuffer *framebuf = new FrameBuffer(width, height, "shadowDepth", {});
		m_shadowDepthBuffer = std::shared_ptr<FrameBuffer>(framebuf);

		m_shadowDepthShaderIndex = ShaderMgr::getSingleton()->loadShader("shadow",
			"./glsl/depth.vert", "./glsl/depth.frag");
	}

	void ShadowSystem::renderShadow(int width, int height, Drawable::ptr drawableList, Light::ptr light)
	{
		if (m_first)
		{
			m_first = false;
			setShadowCamera(drawableList, light);
		}
		m_shadowDepthBuffer->bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_FRONT);
		drawableList->renderDepth(ShaderMgr::getSingleton()->getShader("shadow"), m_lightCamera);
		m_shadowDepthBuffer->unBind(width, height);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
	}

	void ShadowSystem::setShadowCamera(Drawable::ptr drawableList, Light::ptr light)
	{
		// get the whole scene's bounding box.
		AABBBoundingBox &box = drawableList->getBoundingBox();
		float radius = box.getDiagonalLength() * 0.5f;
		glm::vec3 center = box.getCenter();

		// calculate cameraPos.
		glm::vec3 cameraPos = center + radius * reinterpret_cast<DirectionalLight*>(light.get())->getDirection();
		
		FPSCamera *cam = new FPSCamera(cameraPos);
		m_lightCamera = std::shared_ptr<Camera3D>(cam);
		
		// projection.
		m_lightCamera->setOrthographicProject(-radius, +radius, -radius, +radius, 0.1f, radius * 2.0f);

		// view matrix.
		cam->lookAt(-reinterpret_cast<DirectionalLight*>(light.get())->getDirection(), Camera3D::LocalUp);
		box.debug();
	}
}