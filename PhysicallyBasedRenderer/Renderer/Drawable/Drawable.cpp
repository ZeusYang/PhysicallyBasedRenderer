#include "Drawable.h"

#include "../Manager/MeshMgr.h"
#include "../Manager/TextureMgr.h"
#include "../Manager/ShaderMgr.h"

#include <iostream>

namespace Renderer
{
	AABBBoundingBox & Drawable::getBoundingBox()
	{
		if (!m_boundingBox.isValid() || m_transformation.isDirty())
		{
			m_boundingBox.reset();
			MeshMgr::ptr meshMgr = MeshMgr::getSingleton();
			for (unsigned int i = 0; i < m_meshIndex.size(); ++i)
			{
				Mesh::ptr mesh = meshMgr->getMesh(m_meshIndex[i]);
				m_boundingBox.addBoundingBox(mesh->getVertices(), m_transformation.getWorldMatrix());
			}
		}
		return m_boundingBox;
	}

	void Drawable::renderImp()
	{
		// render each mesh.
		MeshMgr::ptr meshMgr = MeshMgr::getSingleton();
		TextureMgr::ptr textureMgr = TextureMgr::getSingleton();
		for (int x = 0; x < m_meshIndex.size(); ++x)
		{
			if (x < m_texIndex.size())
			{
				textureMgr->bindTexture(m_texIndex[x].m_albedoTexIndex, 0);
				textureMgr->bindTexture(m_texIndex[x].m_normalTexIndex, 1);
				textureMgr->bindTexture(m_texIndex[x].m_roughTexIndex, 2);
				textureMgr->bindTexture(m_texIndex[x].m_metallicIndex, 3);
			}
			meshMgr->drawMesh(m_meshIndex[x], m_instance, m_instanceNum);
		}
	}

	void SkyDome::render(Camera3D::ptr camera, Light::ptr sunLight, Camera3D::ptr lightCamera, Shader::ptr shader)
	{
		if (!m_visiable) return;
		if(shader == nullptr)
			shader = ShaderMgr::getSingleton()->getShader(m_shaderIndex);
		shader->bind();
		shader->setInt("image", 0);
		shader->setMat4("viewMatrix", glm::mat4(glm::mat3(camera->getViewMatrix())));
		shader->setMat4("projectMatrix", camera->getProjectMatrix());
		this->renderImp();
		ShaderMgr::getSingleton()->unBindShader();
	}

	void SimpleDrawable::render(Camera3D::ptr camera, Light::ptr sunLight, Camera3D::ptr lightCamera, Shader::ptr shader)
	{
		if (!m_visiable) return;
		if(shader == nullptr)
			shader = ShaderMgr::getSingleton()->getShader(m_shaderIndex);
		shader->bind();
		if (sunLight)
			sunLight->setLightUniform(shader, camera);
		shader->setInt("albedoMap", 0);
		// depth map.
		Texture::ptr depthMap = TextureMgr::getSingleton()->getTexture("shadowDepth");
		if (depthMap != nullptr)
		{
			shader->setInt("depthMap", 1);
			depthMap->bind(1);
		}
		// light space matrix.
		if (lightCamera != nullptr)
			shader->setMat4("lightSpaceMatrix",
				lightCamera->getProjectMatrix() * lightCamera->getViewMatrix());
		else
			shader->setMat4("lightSpaceMatrix", glm::mat4(1.0f));
		// object matrix.
		shader->setBool("instance", false);
		shader->setBool("receiveShadow", m_receiveShadow);
		shader->setMat4("modelMatrix", m_transformation.getWorldMatrix());
		shader->setMat4("viewMatrix", camera->getViewMatrix());
		shader->setMat4("projectMatrix", camera->getProjectMatrix());
		shader->setMat3("normalMatrix", m_transformation.getNormalMatrix());
		this->renderImp();
		ShaderMgr::getSingleton()->unBindShader();
	}

	void SimpleDrawable::renderDepth(Shader::ptr shader, Camera3D::ptr lightCamera)
	{
		if (!m_visiable || !m_produceShadow)
			return;
		shader->bind();
		shader->setBool("instance", false);
		shader->setMat4("lightSpaceMatrix", 
			lightCamera->getProjectMatrix() * lightCamera->getViewMatrix());
		shader->setMat4("modelMatrix", m_transformation.getWorldMatrix());
		this->renderImp();
		ShaderMgr::getSingleton()->unBindShader();
	}

	void ContainerDrawable::render(Camera3D::ptr camera, Light::ptr sunLight,
		Camera3D::ptr lightCamera, Shader::ptr shader)
	{
		if (!m_visiable) return;
		glCullFace(GL_FRONT);
		if (shader == nullptr)
			shader = ShaderMgr::getSingleton()->getShader(m_shaderIndex);
		shader->bind();
		if (sunLight)
			sunLight->setLightUniform(shader, camera);
		shader->setInt("albedoMap", 0);
		// depth map.
		Texture::ptr depthMap = TextureMgr::getSingleton()->getTexture("shadowDepth");
		if (depthMap != nullptr)
		{
			shader->setInt("depthMap", 1);
			depthMap->bind(1);
		}
		// light space matrix.
		if (lightCamera != nullptr)
			shader->setMat4("lightSpaceMatrix",
				lightCamera->getProjectMatrix() * lightCamera->getViewMatrix());
		else
			shader->setMat4("lightSpaceMatrix", glm::mat4(1.0f));
		// object matrix.
		shader->setBool("instance", false);
		shader->setBool("receiveShadow", m_receiveShadow);
		shader->setMat4("modelMatrix", m_transformation.getWorldMatrix());
		shader->setMat4("viewMatrix", camera->getViewMatrix());
		shader->setMat4("projectMatrix", camera->getProjectMatrix());
		shader->setMat3("normalMatrix", m_transformation.getNormalMatrix());
		this->renderImp();
		ShaderMgr::getSingleton()->unBindShader();
		glCullFace(GL_BACK);
	}

	void ContainerDrawable::renderDepth(Shader::ptr shader, Camera3D::ptr lightCamera)
	{
		if (!m_visiable || !m_produceShadow)
			return;
		shader->bind();
		shader->setBool("instance", false);
		shader->setMat4("lightSpaceMatrix",
			lightCamera->getProjectMatrix() * lightCamera->getViewMatrix());
		shader->setMat4("modelMatrix", m_transformation.getWorldMatrix());
		this->renderImp();
		ShaderMgr::getSingleton()->unBindShader();
	}

	AABBBoundingBox & DrawableList::getBoundingBox()
	{
		m_boundingBox.reset();
		for (unsigned int i = 0; i < m_list.size(); ++i)
		{
			Drawable::ptr drawable = m_list[i];
			m_boundingBox.mergeBoundingBox(drawable->getBoundingBox());
		}
		return m_boundingBox;
	}
}