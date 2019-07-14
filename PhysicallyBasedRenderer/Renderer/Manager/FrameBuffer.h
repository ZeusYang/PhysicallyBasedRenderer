#pragma once

#include <vector>
#include <GL/glew.h>
#include <memory>

namespace Renderer
{
	const GLenum ColorAttachments[] =
	{
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5,
		GL_COLOR_ATTACHMENT6,
		GL_COLOR_ATTACHMENT7
	};

	class FrameBuffer
	{
	private:
		bool m_hdr;
		unsigned int m_id;
		int m_width, m_height;
		unsigned int m_depthTexIndex;
		std::vector<unsigned int> m_colorTexIndex;

	public:
		typedef std::shared_ptr<FrameBuffer> ptr;

		FrameBuffer(int width, int height, const std::string &depthName,
			const std::vector<std::string> &colorName, bool hdr = false);
		virtual ~FrameBuffer() { clearFramebuffer(); }

		void bind();
		void unBind(int width = -1, int height = -1);

		int getWidth() const { return m_width; }
		int getHeight() const { return m_height; }
		unsigned int getFramebufferId()const { return m_id; }
		unsigned int getDepthTexIndex()const { return m_depthTexIndex; }
		unsigned int getColorTexIndex(int i)const { return m_colorTexIndex[i]; }

	private:
		void setupDepthFramebuffer(const std::string &name);
		void setupColorFramebuffer(const std::string &name, unsigned int attachIdx);
		void clearFramebuffer();
	};

}
