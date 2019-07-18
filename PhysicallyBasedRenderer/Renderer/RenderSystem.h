#pragma once

#include "Light.h"
#include "Manager/MeshMgr.h"
#include "Manager/ShaderMgr.h"
#include "Manager/TextureMgr.h"
#include "Drawable/Drawable.h"
#include "Camera/Camera3D.h"
#include "Manager/FrameBuffer.h"
#include "Postprocess/GaussianBlur.h"
#include "Postprocess/DeferedShading.h"
#include "Drawable/PointLightDrawable.h"
#include "ShadowSystem.h"

namespace Renderer
{

	struct RenderState
	{
	public:
		GLenum m_depthFunc;
		GLenum m_polygonMode;
		GLenum m_cullFaceMode;
		glm::vec4 m_clearColor;
		GLbitfield m_clearMask;
		bool m_depthTest, m_cullFace;

		RenderState() : 
			m_depthFunc(GL_LESS),
			m_polygonMode(GL_FILL),
			m_cullFaceMode(GL_BACK),
			m_clearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
			m_clearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT),
			m_depthTest(true), m_cullFace(true)
		{}
	};

	class RenderSystem
	{
	private:
		// gaussian blur.
		GaussianBlur::ptr m_glowBlur;

		// skydome.
		SkyDome::ptr m_skyDome;
		DirectionalLight::ptr m_sunLight;
		std::vector<PointLight::ptr> m_pointLights;

		// render state.
		int m_width, m_height;
		RenderState m_renderState;
		ShadowSystem::ptr m_shadowSys;

		// Defered shading.
		DeferedShading::ptr m_deferedShading;
		PointLightDrawable::ptr m_pointLightDrawable;

		// manager.
		MeshMgr::ptr m_meshMgr;
		Camera3D::ptr m_camera;
		ShaderMgr::ptr m_shaderMgr;
		TextureMgr::ptr m_textureMgr;
		DrawableList::ptr m_drawableList;

	public:
		typedef std::shared_ptr<RenderSystem> ptr;

		// ctor/dtor
		RenderSystem() = default;
		~RenderSystem() = default;

		// Initialization.
		void resize(int width, int height);
		void initialize(int width, int height, int shadowWidth, int shadowHeight);
		void setSkyDomeHdr(const std::string &path);
		void setSkyDome(const std::string &path, const std::string &pFix);
		Camera3D::ptr createFPSCamera(glm::vec3 pos, glm::vec3 target);
		Camera3D::ptr createTPSCamera(glm::vec3 pos, glm::vec3 target);

		// Getter.
		Camera3D::ptr getCamera() { return m_camera; }
		MeshMgr::ptr getMeshMgr() { return m_meshMgr; }
		ShaderMgr::ptr getShaderMgr() { return m_shaderMgr; }
		TextureMgr::ptr getTextureMgr() { return m_textureMgr; }
		GaussianBlur::ptr getGaussianBlur() { return m_glowBlur; }

		// Setter.
		void setPolygonMode(GLenum mode);
		void setClearMask(GLbitfield mask);
		void setClearColor(glm::vec4 clearColor);
		void setCullFace(bool enable, GLenum face);
		void setDepthTest(bool enable, GLenum func);
		void addPointLight(glm::vec3 pos, glm::vec3 radiance);
		void addDrawable(Drawable *target) { m_drawableList->addDrawable(target); }
		void setSunLight(glm::vec3 dir, glm::vec3 radiance);

		void render();

	private:
		std::vector<glm::vec3> m_rndVelForPointLights;
		void updatePointLightPosition();
	};
}

