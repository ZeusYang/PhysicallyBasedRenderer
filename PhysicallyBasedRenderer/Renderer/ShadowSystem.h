#pragma once

#include "Drawable/Drawable.h"
#include "Manager/FrameBuffer.h"

namespace Renderer
{
	class ShadowSystem
	{
	private:
		bool m_first;
		Camera3D::ptr m_lightCamera;
		FrameBuffer::ptr m_shadowDepthBuffer;
		unsigned int m_shadowDepthShaderIndex;

	public:
		typedef std::shared_ptr<ShadowSystem> ptr;

		ShadowSystem(unsigned int width = 2048, unsigned int height = 2048);
		~ShadowSystem() = default;

		Camera3D::ptr getLightCamera() const { return m_lightCamera; }
		FrameBuffer::ptr getShadowDepthBuffer()const { return m_shadowDepthBuffer; }

		void renderShadow(int width, int height, Drawable::ptr drawableList, Light::ptr light);

	private:
		void setShadowCamera(Drawable::ptr drawableList, Light::ptr light);
	};

}
