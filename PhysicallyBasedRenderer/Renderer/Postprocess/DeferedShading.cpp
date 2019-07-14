#include "DeferedShading.h"

#include "../Manager/MeshMgr.h"
#include "../Manager/ShaderMgr.h"
#include "../Manager/TextureMgr.h"

namespace Renderer
{
	DeferedShading::DeferedShading(int width, int height)
	{
		m_screenQuadIndex = MeshMgr::getSingleton()->loadMesh(new ScreenQuad());
		m_deferedShaderIndex = ShaderMgr::getSingleton()->loadShader("pbrDeferedShading",
			"./glsl/pbrDeferedShading.vert", "./glsl/pbrDeferedShading.frag");
		m_deferedFramebuffer = std::shared_ptr<FrameBuffer>(new FrameBuffer(
			width, height, "deferedDepth", { "deferedPos", "deferedNormal", "deferedAlbedo",
			"deferedRoughness" }, true));

		// ssao.
		m_ssaoProcess = std::shared_ptr<SSAOProcess>(new SSAOProcess(
			width, height, m_screenQuadIndex));
	}

	unsigned int DeferedShading::getFrameBufferId()
	{
		return m_deferedFramebuffer->getFramebufferId();
	}

	void DeferedShading::bindDeferedFramebuffer()
	{
		m_deferedFramebuffer->bind();
	}

	void DeferedShading::ssaoFactorGeneration(Camera3D::ptr camera)
	{
		unsigned int posTexIndex = TextureMgr::getSingleton()->getTextureIndex("deferedPos");
		unsigned int normalTexIndex = TextureMgr::getSingleton()->getTextureIndex("deferedNormal");
		unsigned int depthTexIndex = TextureMgr::getSingleton()->getTextureIndex("deferedDepth");
		m_ssaoProcess->SSAOProcessDepth(camera, posTexIndex, normalTexIndex, depthTexIndex);
	}

	void DeferedShading::renderDeferedShading(Camera3D::ptr camera, Light::ptr sunLight, Camera3D::ptr lightCamera
		, const std::vector<PointLight::ptr> &pointLights)
	{
		Shader::ptr deferedShader = ShaderMgr::getSingleton()->getShader(m_deferedShaderIndex);
		deferedShader->bind();
		deferedShader->setInt("dposition", 0);
		deferedShader->setInt("dnormal", 1);
		deferedShader->setInt("dalbedo", 2);
		deferedShader->setInt("droughness", 3);
		deferedShader->setInt("ddepth", 4);
		deferedShader->setInt("shadowDepth", 5);
		deferedShader->setMat4("lightSpaceMatrix", lightCamera->getProjectMatrix() * lightCamera->getViewMatrix());
		sunLight->setLightUniform(deferedShader, camera);
		deferedShader->setInt("pointLightNum", pointLights.size());
		for (unsigned int x = 0; x < pointLights.size(); ++x)
		{
			pointLights[x]->setLightUniform(deferedShader, camera);
		}
		TextureMgr::getSingleton()->bindTexture("deferedPos", 0);
		TextureMgr::getSingleton()->bindTexture("deferedNormal", 1);
		TextureMgr::getSingleton()->bindTexture("deferedAlbedo", 2);
		TextureMgr::getSingleton()->bindTexture("deferedRoughness", 3);
		TextureMgr::getSingleton()->bindTexture("deferedDepth", 4);
		TextureMgr::getSingleton()->bindTexture("shadowDepth", 5);
		MeshMgr::getSingleton()->drawMesh(m_screenQuadIndex, false, 0);
		TextureMgr::getSingleton()->unBindTexture("deferedPos");
		TextureMgr::getSingleton()->unBindTexture("deferedNormal");
		TextureMgr::getSingleton()->unBindTexture("deferedAlbedo");
		TextureMgr::getSingleton()->unBindTexture("deferedRoughness");
		TextureMgr::getSingleton()->unBindTexture("deferedDepth");
		TextureMgr::getSingleton()->unBindTexture("shadowDepth");
		deferedShader->unBind();
	}
}