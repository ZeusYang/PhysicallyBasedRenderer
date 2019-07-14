#include "FrameBuffer.h"

#include "TextureMgr.h"
#include <iostream>

namespace Renderer
{
	FrameBuffer::FrameBuffer(int width, int height, const std::string &depthName,
		const std::vector<std::string> &colorName, bool hdr)
		:m_width(width), m_height(height), m_hdr(hdr)
	{
		glGenFramebuffers(1, &m_id);

		// color attachment.
		m_colorTexIndex.resize(colorName.size());
		for (int x = 0; x < colorName.size(); ++x)
			setupColorFramebuffer(colorName[x], x);
		// depth attachment.
		if (!depthName.empty())
			setupDepthFramebuffer(depthName);

		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		if (colorName.size() > 0)
		{
			glDrawBuffers(colorName.size(), ColorAttachments);
		}
		else
		{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::bind()
	{
		glViewport(0, 0, m_width, m_height);
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
	}

	void FrameBuffer::unBind(int width, int height)
	{
		if (width == -1)width = m_width;
		if (height == -1)height = m_height;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
	}

	void FrameBuffer::setupDepthFramebuffer(const std::string &name)
	{
		TextureMgr::ptr textureMgr = TextureMgr::getSingleton();
		m_depthTexIndex = textureMgr->loadTextureDepth(name, m_width, m_height);
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, textureMgr->getTexture(m_depthTexIndex)->getTextureId(), 0);
		//glDrawBuffer(GL_NONE);
		//glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::setupColorFramebuffer(const std::string & name, unsigned int attachIdx)
	{
		TextureMgr::ptr textureMgr = TextureMgr::getSingleton();
		m_colorTexIndex[attachIdx] = textureMgr->loadTextureColor(name, m_width, m_height, m_hdr);
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachIdx,
			GL_TEXTURE_2D, textureMgr->getTexture(m_colorTexIndex[attachIdx])->getTextureId(), 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::clearFramebuffer()
	{
		glDeleteFramebuffers(1, &m_id);
	}

}