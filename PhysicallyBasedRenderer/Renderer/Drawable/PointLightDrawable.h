#pragma once

#include "Drawable.h"
#include "../Manager/ShaderMgr.h"

namespace Renderer
{
	class PointLightDrawable : public Drawable
	{
	private:
		glm::vec3 m_lightColor;
		float m_particleRadius;
		unsigned int m_particleNum;
		unsigned int m_particleVAO;
		unsigned int m_particleVBO;
		ShaderMgr::ptr m_shaderMgr;

		struct LightSource
		{
			glm::vec3 pos;
			glm::vec3 color;
		};

	public:
		typedef std::shared_ptr<PointLightDrawable> ptr;

		PointLightDrawable();
		~PointLightDrawable();

		void setPointLightRadius(float raidus);
		void setPointLightPositions(const std::vector<PointLight::ptr> &lights);

		virtual void render(Camera3D::ptr camera, Light::ptr sunLight, Camera3D::ptr lightCamera,
			Shader::ptr shader = nullptr);

		virtual void renderDepth(Shader::ptr shader, Camera3D::ptr lightCamera);
	};
}