#include "SSAOProcess.h"

#include "../Manager/MeshMgr.h"
#include "../Manager/ShaderMgr.h"
#include "../Manager/TextureMgr.h"

#include <time.h>
#include <sstream>

namespace Renderer
{
	SSAOProcess::SSAOProcess(int width, int height, unsigned int screenQuadIndex)
		: m_screenQuadIndex(screenQuadIndex)
	{
		m_ssaoFramebuffer = std::shared_ptr<FrameBuffer>(new FrameBuffer(
			width, height, "ssaoDepth", {}, true));
		m_ssaoShaderIndex = ShaderMgr::getSingleton()->loadShader("pbrSSAOProcess",
			"./glsl/pbrSSAOProcess.vert", "./glsl/pbrSSAOProcess.frag");
		m_ssaoBlurShaderIndex = ShaderMgr::getSingleton()->loadShader("pbrSSAOBlur",
			"./glsl/pbrSSAOBlur.vert", "./glsl/pbrSSAOBlur.frag");
		m_ssaoLinearizeShaderIndex = ShaderMgr::getSingleton()->loadShader("pbrSSAOLinearize",
			"./glsl/pbrSSAOLinearize.vert", "./glsl/pbrSSAOLinearize.frag");
		generateSamples();
		generateRandomNoiseTex();
	}

	void SSAOProcess::SSAOProcessDepth(Camera3D::ptr camera, unsigned int posTexIndex,
		unsigned int normalTexIndex, unsigned int depthTexIndex)
	{
		// first of all, linearize the depth.
		m_ssaoFramebuffer->bind();

		{
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glClear(GL_DEPTH_BUFFER_BIT);
			Shader::ptr linearizeShader = ShaderMgr::getSingleton()->getShader(m_ssaoLinearizeShaderIndex);
			linearizeShader->bind();
			linearizeShader->setInt("depthMap", 0);
			linearizeShader->setFloat("nearPlane", camera->getNear());
			linearizeShader->setFloat("farPlane", camera->getFar());
			TextureMgr::getSingleton()->bindTexture(depthTexIndex, 0);
			MeshMgr::getSingleton()->drawMesh(m_screenQuadIndex, false, 0);
			// copy to depth map.
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_ssaoFramebuffer->getWidth(),
				m_ssaoFramebuffer->getHeight());
			TextureMgr::getSingleton()->unBindTexture(depthTexIndex);
			linearizeShader->unBind();
		}

		// calculate ao factor.
		{
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glClear(GL_DEPTH_BUFFER_BIT);
			Shader::ptr ssaoShader = ShaderMgr::getSingleton()->getShader(m_ssaoShaderIndex);
			ssaoShader->bind();
			ssaoShader->setInt("dposition", 0);
			ssaoShader->setInt("dnormal", 1);
			ssaoShader->setInt("ddepth", 2);
			ssaoShader->setInt("randomNoise", 3);
			ssaoShader->setFloat("nearPlane", camera->getNear());
			ssaoShader->setFloat("farPlane", camera->getFar());
			ssaoShader->setMat4("viewMatrix", camera->getViewMatrix());
			ssaoShader->setMat4("projectMatrix", camera->getProjectMatrix());
			for (unsigned int i = 0; i < 64; ++i)
			{
				std::stringstream ss;
				ss << i;
				ssaoShader->setVec3("samples[" + ss.str() + "]", m_samples[i]);
			}
			TextureMgr::getSingleton()->bindTexture(posTexIndex, 0);
			TextureMgr::getSingleton()->bindTexture(normalTexIndex, 1);
			TextureMgr::getSingleton()->bindTexture(depthTexIndex, 2);
			TextureMgr::getSingleton()->bindTexture(m_noiseTexIndex, 3);
			MeshMgr::getSingleton()->drawMesh(m_screenQuadIndex, false, 0);
			TextureMgr::getSingleton()->unBindTexture(posTexIndex);
			TextureMgr::getSingleton()->unBindTexture(normalTexIndex);
			TextureMgr::getSingleton()->unBindTexture(depthTexIndex);
			TextureMgr::getSingleton()->unBindTexture(m_noiseTexIndex);
			// copy to depth map.
			TextureMgr::getSingleton()->bindTexture(depthTexIndex, 0);
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_ssaoFramebuffer->getWidth(),
				m_ssaoFramebuffer->getHeight());
			TextureMgr::getSingleton()->unBindTexture(depthTexIndex);
			ssaoShader->unBind();
		}

		// blur the ao factor.
		{
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glClear(GL_DEPTH_BUFFER_BIT);
			Shader::ptr ssaoBlurShader = ShaderMgr::getSingleton()->getShader(m_ssaoBlurShaderIndex);
			ssaoBlurShader->bind();
			ssaoBlurShader->setInt("ddepth", 0);
			TextureMgr::getSingleton()->bindTexture(depthTexIndex, 0);
			MeshMgr::getSingleton()->drawMesh(m_screenQuadIndex, false, 0);
			// copy to depth map.
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_ssaoFramebuffer->getWidth(),
				m_ssaoFramebuffer->getHeight());
			TextureMgr::getSingleton()->unBindTexture(depthTexIndex);
			ssaoBlurShader->unBind();
		}

		m_ssaoFramebuffer->unBind();
	}

	float SSAOProcess::randFloat()
	{
		// randome float number in [0,1]
		return (((double)rand()) / RAND_MAX);
	}

	float SSAOProcess::lerp(float a, float b, float value)
	{
		return a + value * (b - a);
	}

	void SSAOProcess::generateSamples()
	{
		// generate sample points.
		m_samples.clear();
		srand(time(nullptr));
		for (unsigned int i = 0; i < 64; ++i)
		{
			glm::vec3 sample;
			sample.x = 2.0f * randFloat() - 1.0f;
			sample.y = 2.0f * randFloat() - 1.0f;
			sample.z = randFloat();

			sample = glm::normalize(sample);
			sample *= randFloat();
			float scale = static_cast<float>(i) / 64.0f;
			scale = lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;
			m_samples.push_back(sample);
		}
	}

	void SSAOProcess::generateRandomNoiseTex()
	{
		// generate random vectors in xy plane.
		srand(time(nullptr));
		std::vector<glm::vec3> ssaoNoise;
		for (unsigned int i = 0; i < 16; ++i)
		{
			glm::vec3 noise;
			noise.x = 2.0f * randFloat() - 1.0f;
			noise.y = 2.0f * randFloat() - 1.0f;
			noise.z = 0.0f;
			ssaoNoise.push_back(noise);
		}
		m_noiseTexIndex = TextureMgr::getSingleton()->loadTextureRaw("ssaoNoise",
			4, 4, reinterpret_cast<char*>(&ssaoNoise[0]));
	}
}