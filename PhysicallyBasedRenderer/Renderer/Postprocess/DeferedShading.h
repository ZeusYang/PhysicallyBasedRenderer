#pragma once

#include "SSAOProcess.h"
#include "../Light.h"
#include "../Camera/Camera3D.h"
#include "../Manager/Geometry.h"
#include "../Manager/FrameBuffer.h"

namespace Renderer
{
	class DeferedShading
	{
	private:
		SSAOProcess::ptr m_ssaoProcess;
		unsigned int m_screenQuadIndex;
		unsigned int m_deferedShaderIndex;
		FrameBuffer::ptr m_deferedFramebuffer;

	public:
		typedef std::shared_ptr<DeferedShading> ptr;

		DeferedShading(int width, int height);
		~DeferedShading() = default;

		void bindDeferedFramebuffer();
		unsigned int getFrameBufferId();
		void ssaoFactorGeneration(Camera3D::ptr camera);
		void renderDeferedShading(Camera3D::ptr camera, Light::ptr sunLight, Camera3D::ptr lightCamera
					, const std::vector<PointLight::ptr> &pointLights);
	};
}

