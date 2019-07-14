#pragma once

#include "../Manager/TextureMgr.h"
#include "../Manager/FrameBuffer.h"

namespace Renderer
{
	class GaussianBlur
	{
	private:
		unsigned int m_blurTimes;
		unsigned int m_readIndex;
		unsigned int m_writeIndex;
		unsigned int m_screenQuadIndex;
		unsigned int m_mergeShaderIndex;
		unsigned int m_gaussianShaderIndex;
		FrameBuffer::ptr m_framebuffer[2];

	public:
		typedef std::shared_ptr<GaussianBlur> ptr;

		GaussianBlur(int width, int height);
		~GaussianBlur() = default;

		void bindGaussianFramebuffer();
		void renderGaussianBlurEffect();

		unsigned int getFrameBufferId()const { return m_framebuffer[0]->getFramebufferId(); }
		unsigned int getSceneTexIndex()const { return TextureMgr::getSingleton()->getTextureIndex("Color0"); }
		unsigned int getSceneDepthTexIndex()const { return TextureMgr::getSingleton()->getTextureIndex("GaussianDepth0"); }
		unsigned int &getBlurTimes() { return m_blurTimes; }
		void setBlurTimes(unsigned int t) { m_blurTimes = t; }

	};

}
