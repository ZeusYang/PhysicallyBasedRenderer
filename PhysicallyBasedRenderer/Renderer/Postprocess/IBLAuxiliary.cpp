#include "IBLAuxiliary.h"

#include "../Manager/Geometry.h"
#include "../Manager/Mesh.h"
#include "../Manager/TextureMgr.h"
#include "../Manager/ShaderMgr.h"
#include "../Manager/FrameBuffer.h"

#include <sstream>

#include "../Debug.h"

namespace Renderer
{
	void IBLAuxiliary::convertToCubemap(int width, int height,
		unsigned int hdrTexIndex, unsigned int cuebmapTexIndex)
	{
		// manager.
		TextureMgr::ptr texMgr = TextureMgr::getSingleton();
		ShaderMgr::ptr shaderMgr = ShaderMgr::getSingleton();
		// load shader.
		unsigned int shaderIndex = shaderMgr->loadShader("convertToCubemap",
			"./glsl/convertToCubemap.vert", "./glsl/convertToCubemap.frag");
		// load cube mesh.
		Mesh::ptr cubeMesh = std::shared_ptr<Mesh>(new Cube(1.0f, 1.0f, 1.0f));
		// load framebuffer.
		FrameBuffer::ptr framebuffer = std::shared_ptr<FrameBuffer>(new FrameBuffer(width, height,
			"convertDepth", {}, true));
		// projection matrix and view matrix.
		glm::mat4 captureProjectMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViewMatrix[] =
		{
			glm::lookAt(glm::vec3(0.0f), glm::vec3(+1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,+1.0f, 0.0f), glm::vec3(0.0f,  0.0f,+1.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,-1.0f, 0.0f), glm::vec3(0.0f,  0.0f,-1.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, 0.0f,+1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, 0.0f,-1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		};

		// convert.
		framebuffer->bind();
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLuint cubemapId = texMgr->getTexture(cuebmapTexIndex)->getTextureId();
		Shader::ptr shader = shaderMgr->getShader(shaderIndex);
		shader->bind();
		shader->setInt("hdrMap", 0);
		shader->setMat4("projectMatrix", captureProjectMatrix);
		texMgr->bindTexture(hdrTexIndex, 0);
		for (unsigned int i = 0; i < 6; ++i)
		{
			shader->setMat4("viewMatrix", captureViewMatrix[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemapId, 0);
			cubeMesh->draw(false, 0);
		}
		shader->unBind();
		texMgr->unBindTexture(hdrTexIndex);
		framebuffer->unBind();
	}

	void IBLAuxiliary::convoluteDiffuseIntegral(int width, int height, unsigned int cubemapTexIndex,
		unsigned int irradianceTexIndex)
	{
		// manager.
		TextureMgr::ptr texMgr = TextureMgr::getSingleton();
		ShaderMgr::ptr shaderMgr = ShaderMgr::getSingleton();
		// load shader.
		unsigned int shaderIndex = shaderMgr->loadShader("diffuseIntegral",
			"./glsl/diffuseIntegral.vert", "./glsl/diffuseIntegral.frag");
		// load cube mesh.
		Mesh::ptr cubeMesh = std::shared_ptr<Mesh>(new Cube(1.0f, 1.0f, 1.0f));
		// load framebuffer.
		FrameBuffer::ptr framebuffer = std::shared_ptr<FrameBuffer>(
			new FrameBuffer(width, height, "irradianceDepth", {}, true));
		// projection matrix and view matrix.
		glm::mat4 captureProjectMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViewMatrix[] =
		{
			glm::lookAt(glm::vec3(0.0f), glm::vec3(+1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,+1.0f, 0.0f), glm::vec3(0.0f,  0.0f,+1.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,-1.0f, 0.0f), glm::vec3(0.0f,  0.0f,-1.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, 0.0f,+1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3 (0.0f, 0.0f,-1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		};

		// begin to convolute.
		framebuffer->bind();
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLuint irradianceTexId = texMgr->getTexture(irradianceTexIndex)->getTextureId();
		Shader::ptr shader = shaderMgr->getShader(shaderIndex);
		shader->bind();
		shader->setInt("environmentMap", 0);
		shader->setMat4("projectMatrix", captureProjectMatrix);
		texMgr->bindTexture(cubemapTexIndex, 0);
		for (unsigned int i = 0; i < 6; ++i)
		{
			shader->setMat4("viewMatrix", captureViewMatrix[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceTexId, 0);
			cubeMesh->draw(false, 0);
		}
		shader->unBind();
		texMgr->unBindTexture(cubemapTexIndex);
		framebuffer->unBind();
	}

	void IBLAuxiliary::convoluteSpecularIntegral(int width, int height, unsigned int cubemapTexIndex,
		unsigned int prefilteredTexIndex)
	{
		// manager.
		TextureMgr::ptr texMgr = TextureMgr::getSingleton();
		ShaderMgr::ptr shaderMgr = ShaderMgr::getSingleton();
		// load shader.
		unsigned int shaderIndex = shaderMgr->loadShader("prefilterEnvMap",
			"./glsl/prefilterEnvMap.vert", "./glsl/prefilterEnvMap.frag");
		// load cube mesh.
		Mesh::ptr cubeMesh = std::shared_ptr<Mesh>(new Cube(1.0f, 1.0f, 1.0f));
		// projection matrix and view matrix.
		glm::mat4 captureProjectMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViewMatrix[] =
		{
			glm::lookAt(glm::vec3(0.0f), glm::vec3(+1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,+1.0f, 0.0f), glm::vec3(0.0f,  0.0f,+1.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,-1.0f, 0.0f), glm::vec3(0.0f,  0.0f,-1.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f,+1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f,-1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		};

		// begin to filter.
		GLuint prefilteredTexId = texMgr->getTexture(prefilteredTexIndex)->getTextureId();
		Shader::ptr shader = shaderMgr->getShader(shaderIndex);
		shader->bind();
		shader->setInt("environmentMap", 0);
		shader->setMat4("projectMatrix", captureProjectMatrix);
		texMgr->bindTexture(cubemapTexIndex, 0);
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			unsigned int mipWidth = width * std::pow(0.5, mip);
			unsigned int mipHeight = height * std::pow(0.5, mip);
			std::stringstream ss;
			ss << mip;
			FrameBuffer::ptr framebuffer = std::shared_ptr<FrameBuffer>(
				new FrameBuffer(mipWidth, mipHeight, "prefilteredDepth" + ss.str(), {}, true));
			framebuffer->bind();
			glDisable(GL_BLEND);
			glDisable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
			glDepthFunc(GL_LEQUAL);
			glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			shader->setFloat("roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				shader->setMat4("viewMatrix", captureViewMatrix[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilteredTexId, mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				cubeMesh->draw(false, 0);
			}
			framebuffer->unBind();
		}
		shader->unBind();
		texMgr->unBindTexture(cubemapTexIndex);
	}

	void IBLAuxiliary::convoluteSpecularBRDFIntegral(int width, int height, unsigned int brdfLutTexIndex)
	{
		// manager.
		TextureMgr::ptr texMgr = TextureMgr::getSingleton();
		ShaderMgr::ptr shaderMgr = ShaderMgr::getSingleton();
		// load shader.
		unsigned int shaderIndex = shaderMgr->loadShader("genBrdfLUT",
			"./glsl/genBrdfLUT.vert", "./glsl/genBrdfLUT.frag");
		// load quad mesh.
		Mesh::ptr quadMesh = std::shared_ptr<Mesh>(new ScreenQuad());
		FrameBuffer::ptr framebuffer = std::shared_ptr<FrameBuffer>(
			new FrameBuffer(width, height, "brdfDepth", {}, true)) ;
		Shader::ptr shader = shaderMgr->getShader(shaderIndex);

		framebuffer->bind();
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		GLuint brdfLutTexId = texMgr->getTexture(brdfLutTexIndex)->getTextureId();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLutTexId, 0);

		shader->bind();
		quadMesh->draw(false, 0);
		shader->unBind();
		framebuffer->unBind();
	}
}