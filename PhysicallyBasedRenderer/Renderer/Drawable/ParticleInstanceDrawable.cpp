#include "ParticleInstanceDrawable.h"

#include "../Manager/MeshMgr.h"
#include "../Manager/ShaderMgr.h"
#include "../Manager/TextureMgr.h"

namespace Renderer
{
	ParticleInstanceDrawable::ParticleInstanceDrawable(unsigned int shaderIndex)
	{
		m_shaderIndex = ShaderMgr::getSingleton()->loadShader("particleInstance",
			"./glsl/particleInstance.vert", "./glsl/particleInstance.frag");
		m_depthShaderIndex = ShaderMgr::getSingleton()->loadShader("particleInstanceDepth",
			"./glsl/particleInstanceDepth.vert", "./glsl/particleInstanceDepth.frag");
	}

	ParticleInstanceDrawable::~ParticleInstanceDrawable()
	{
	}

	void ParticleInstanceDrawable::setInstanceVBO(unsigned int vbo, unsigned int nums)
	{
		m_instanceVBO = vbo;
		MeshMgr::ptr meshMgr = MeshMgr::getSingleton();
		for (int x = 0; x < m_meshIndex.size(); ++x)
		{
			unsigned int vao = meshMgr->getMesh(m_meshIndex[x])->getVertexArrayObject();
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
			// vertex property.
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glVertexAttribDivisor(6, 1);
		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_instance = true;
		m_instanceNum = nums;
	}

	void ParticleInstanceDrawable::setParticleRadius(float radius)
	{
		m_transformation.setScale(glm::vec3(radius));
	}

	void ParticleInstanceDrawable::render(Camera3D::ptr camera, Light::ptr sunLight,
		Camera3D::ptr lightCamera, Shader::ptr shader)
	{
		if (!m_visiable) return;
		// render the model.
		shader = ShaderMgr::getSingleton()->getShader(m_shaderIndex);
		shader->bind();
		if (sunLight)
			sunLight->setLightUniform(shader, camera);
		shader->setInt("image", 0);
		// depth map.
		Texture::ptr depthMap = TextureMgr::getSingleton()->getTexture("shadowDepth");
		if (depthMap != nullptr)
		{
			shader->setInt("depthMap", 1);
			depthMap->bind(1);
		}
		if (lightCamera != nullptr)
			shader->setMat4("lightSpaceMatrix",
				lightCamera->getProjectMatrix() * lightCamera->getViewMatrix());
		else
			shader->setMat4("lightSpaceMatrix", glm::mat4(1.0f));
		// object matrix.
		shader->setBool("receiveShadow", m_receiveShadow);
		shader->setMat4("modelMatrix", m_transformation.getWorldMatrix());
		shader->setMat4("viewMatrix", camera->getViewMatrix());
		shader->setMat4("projectMatrix", camera->getProjectMatrix());
		this->renderImp();
		ShaderMgr::getSingleton()->unBindShader();
	}

	void ParticleInstanceDrawable::renderDepth(Shader::ptr shader, Camera3D::ptr lightCamera)
	{
		if (!m_visiable || !m_produceShadow)
			return;
		shader = ShaderMgr::getSingleton()->getShader(m_depthShaderIndex);
		shader->bind();
		shader->setMat4("lightSpaceMatrix",
			lightCamera->getProjectMatrix() * lightCamera->getViewMatrix());
		shader->setMat4("modelMatrix", m_transformation.getWorldMatrix());
		this->renderImp();
		ShaderMgr::getSingleton()->unBindShader();
	}
}