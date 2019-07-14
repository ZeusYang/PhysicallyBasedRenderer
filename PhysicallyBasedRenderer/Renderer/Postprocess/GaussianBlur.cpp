#include "GaussianBlur.h"

#include "../Manager/ShaderMgr.h"
#include "../Manager/Geometry.h"
#include "../Manager/MeshMgr.h"
#include "../Manager/TextureMgr.h"

#include <iostream>
#include <sstream>

namespace Renderer
{
	GaussianBlur::GaussianBlur(int width, int height)
		:m_readIndex(0), m_writeIndex(1), m_blurTimes(10)
	{
		m_framebuffer[0] = std::shared_ptr<FrameBuffer>(
			new FrameBuffer(width, height, "GaussianDepth0", { "Color0","BrightColor0"}, true));
		m_framebuffer[1] = std::shared_ptr<FrameBuffer>(
			new FrameBuffer(width, height, "GaussianDepth1", { "Color1","BrightColor1"}, true));
		m_screenQuadIndex = MeshMgr::getSingleton()->loadMesh(new ScreenQuad());
		//m_mergeShaderIndex
		m_mergeShaderIndex = ShaderMgr::getSingleton()->loadShader("MergeBlur",
			"./glsl/MergeBlur.vert", "./glsl/MergeBlur.frag");
		m_gaussianShaderIndex = ShaderMgr::getSingleton()->loadShader("GaussianBlur",
			"./glsl/GaussianBlur.vert", "./glsl/GaussianBlur.frag");
	}

	void GaussianBlur::bindGaussianFramebuffer()
	{
		m_framebuffer[m_readIndex]->bind();
	}

	void GaussianBlur::renderGaussianBlurEffect()
	{
		bool horizontal = true;
		Shader::ptr blurShader = ShaderMgr::getSingleton()->getShader(m_gaussianShaderIndex);
		blurShader->bind();
		std::string texStr;
		for (unsigned int i = 0; i < m_blurTimes; ++i)
		{
			m_framebuffer[m_writeIndex]->bind();
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
			glClearColor(0.0, 0.0, 1.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT);
			blurShader->setInt("Color", 0);
			blurShader->setInt("Bright", 1);
			blurShader->setInt("horizontal", horizontal);
			std::stringstream iss;
			iss << m_readIndex;
			TextureMgr::getSingleton()->bindTexture("Color" + iss.str(), 0);
			TextureMgr::getSingleton()->bindTexture("BrightColor" + iss.str(), 1);
			MeshMgr::getSingleton()->drawMesh(m_screenQuadIndex, false, 0);
			std::swap(m_readIndex, m_writeIndex);
			horizontal = !horizontal;
		}
		blurShader->unBind();

		m_framebuffer[m_readIndex]->unBind();
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glClearColor(0.0, 1.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		Shader::ptr mergeShader = ShaderMgr::getSingleton()->getShader(m_mergeShaderIndex);
		Texture::ptr depthMap = TextureMgr::getSingleton()->getTexture("shadowDepth");
		mergeShader->bind();
		mergeShader->setInt("Color", 0);
		mergeShader->setInt("BrightColor", 1);
		mergeShader->setInt("DepthMap", 2);
		TextureMgr::getSingleton()->bindTexture("Color0", 0);
		TextureMgr::getSingleton()->bindTexture("BrightColor0", 1);
		depthMap->bind(2);
		MeshMgr::getSingleton()->drawMesh(m_screenQuadIndex, false, 0);
		TextureMgr::getSingleton()->unBindTexture("Color0");
		TextureMgr::getSingleton()->unBindTexture("BrightColor0");
		depthMap->unBind();
		mergeShader->unBind();
	}
}