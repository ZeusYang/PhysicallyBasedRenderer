#pragma once

#include "../Light.h"
#include "../Camera/Camera3D.h"
#include "../Manager/FrameBuffer.h"

namespace Renderer
{

	class SSAOProcess
	{
	private:
		std::vector<glm::vec3> m_samples;
		unsigned int m_noiseTexIndex;
		unsigned int m_screenQuadIndex;
		unsigned int m_ssaoShaderIndex;
		unsigned int m_ssaoBlurShaderIndex;
		unsigned int m_ssaoLinearizeShaderIndex;
		FrameBuffer::ptr m_ssaoFramebuffer;

	public:
		typedef std::shared_ptr<SSAOProcess> ptr;

		SSAOProcess(int width, int height, unsigned int screenQuadIndex);
		~SSAOProcess() = default;

		void SSAOProcessDepth(Camera3D::ptr camera, unsigned int posTexIndex, unsigned int normalTexIndex,
			unsigned int depthTexIndex);

	private:
		float randFloat();
		float lerp(float a, float b, float value);

		void generateSamples();
		void generateRandomNoiseTex();
	};

}

