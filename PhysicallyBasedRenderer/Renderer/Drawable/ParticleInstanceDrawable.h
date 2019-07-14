#pragma once

#include "Drawable.h"

namespace Renderer
{

	class ParticleInstanceDrawable : public Drawable
	{
	private:
		unsigned int m_depthShaderIndex;
		unsigned int m_instanceVBO;

	public:
		ParticleInstanceDrawable(unsigned int shaderIndex);
		~ParticleInstanceDrawable();

		void setInstanceVBO(unsigned int vbo, unsigned int nums);
		void setParticleRadius(float radius);

		virtual void render(Camera3D::ptr camera, Light::ptr sunLight, Camera3D::ptr lightCamera, Shader::ptr shader = nullptr);
		virtual void renderDepth(Shader::ptr shader, Camera3D::ptr lightCamera);
	};

}
