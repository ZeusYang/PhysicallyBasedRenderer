#include "PointLightDrawable.h"

#include "../RenderDevice.h"

namespace Renderer
{
	PointLightDrawable::PointLightDrawable()
	{
		// initialize
		m_particleVBO = 0;
		m_particleRadius = 1.0f;
		m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glGenVertexArrays(1, &m_particleVAO);
		glGenBuffers(1, &m_particleVBO);

		// load shader and texture.
		m_shaderMgr = ShaderMgr::getSingleton();
		m_shaderIndex = m_shaderMgr->loadShader("lightSource",
			"./glsl/lightSource.vert", "./glsl/lightSource.frag");
	}

	PointLightDrawable::~PointLightDrawable()
	{
		glDeleteBuffers(1, &m_particleVBO);
		glDeleteVertexArrays(1, &m_particleVAO);
	}

	void PointLightDrawable::setPointLightRadius(float radius)
	{
		m_particleRadius = radius;
	}

	void PointLightDrawable::setPointLightPositions(const std::vector<PointLight::ptr>& lights)
	{
		m_particleNum = lights.size();
		std::vector<LightSource> pos;
		for (unsigned int x = 0; x < lights.size(); ++x)
		{
			LightSource target;
			target.pos = lights[x]->getPosition();
			target.color = glm::normalize(lights[x]->getRadiance());
			pos.push_back(target);
		}
		glBindVertexArray(m_particleVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_particleVBO);
		glBufferData(GL_ARRAY_BUFFER, m_particleNum * sizeof(LightSource), &pos[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LightSource), 
			reinterpret_cast<void*>(offsetof(LightSource, pos)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LightSource),
			reinterpret_cast<void*>(offsetof(LightSource, color)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void PointLightDrawable::render(Camera3D::ptr camera, Light::ptr sunLight, 
		Camera3D::ptr lightCamera, Shader::ptr shader)
	{
		//if (!isVisiable())
		//	return;

		// calculate particle size scale factor.
		int scrWidth = RenderDevice::getSingleton()->getWindowWidth();
		float aspect = camera->getAspect();
		float fovy = camera->getFovy();
		float pointScale = 1.0f * scrWidth / aspect * (1.0f / tanf(glm::radians(fovy) * 0.5f));

		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

		shader = m_shaderMgr->getShader("lightSource");
		shader->bind();
		shader->setVec3("lightColor", m_lightColor);
		shader->setFloat("pointScale", pointScale);
		shader->setFloat("pointSize", m_particleRadius);
		shader->setMat4("modelMatrix", m_transformation.getWorldMatrix());
		shader->setMat4("viewMatrix", camera->getViewMatrix());
		shader->setMat4("projectMatrix", camera->getProjectMatrix());

		// draw
		glBindVertexArray(m_particleVAO);
		glDrawArrays(GL_POINTS, 0, m_particleNum);
		glBindVertexArray(0);

		shader->unBind();
		glDisable(GL_PROGRAM_POINT_SIZE);
	}

	void PointLightDrawable::renderDepth(Shader::ptr shader, Camera3D::ptr lightCamera)
	{
		return;
	}
}