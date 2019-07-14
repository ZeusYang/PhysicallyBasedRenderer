#include "ParticlePointSpriteDrawable.h"

#include "../RenderDevice.h"

#include <iostream>

namespace Renderer
{
	ParticlePointSpriteDrawable::ParticlePointSpriteDrawable(unsigned int posChannel)
		:m_posChannel(posChannel)
	{
		// initialize
		m_particleVBO = 0;
		m_numParticles = 0;
		m_particleRadius = 1.0f;
		m_vboCreateBySelf = false;
		m_baseColor = glm::vec3(1.0f, 0.6f, 0.3f);
		glGenVertexArrays(1, &m_particleVAO);

		// load shader and texture.
		m_shaderMgr = ShaderMgr::getSingleton();
		m_textureMgr = TextureMgr::getSingleton();
		m_shaderIndex = m_shaderMgr->loadShader("particle",
			"./glsl/particle.vert", "./glsl/particle.frag");
		generateGaussianMap(32);
	}

	ParticlePointSpriteDrawable::~ParticlePointSpriteDrawable()
	{
		if(m_vboCreateBySelf && m_particleVBO != 0)
			glDeleteBuffers(1, &m_particleVBO);
		glDeleteVertexArrays(1, &m_particleVAO);
	}

	void ParticlePointSpriteDrawable::setParticleRadius(float radius)
	{
		m_particleRadius = radius;
	}

	void ParticlePointSpriteDrawable::setPositions(std::vector<glm::vec4>& position)
	{
		m_posChannel = 4;
		m_vboCreateBySelf = true;
		m_numParticles = position.size();
		if (m_particleVBO == 0)
			glGenBuffers(1, &m_particleVBO);
		glBindVertexArray(m_particleVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_particleVBO);
		glBufferData(GL_ARRAY_BUFFER, m_numParticles * m_posChannel * sizeof(float),
			&position[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, m_posChannel, GL_FLOAT, GL_FALSE, m_posChannel * sizeof(float),
			static_cast<void*>(0));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void ParticlePointSpriteDrawable::setPositions(std::vector<glm::vec3> &position)
	{
		m_posChannel = 3;
		m_vboCreateBySelf = true;
		m_numParticles = position.size();
		if (m_particleVBO == 0)
			glGenBuffers(1, &m_particleVBO);
		glBindVertexArray(m_particleVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_particleVBO);
		glBufferData(GL_ARRAY_BUFFER, m_numParticles * m_posChannel * sizeof(float),
			&position[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, m_posChannel, GL_FLOAT, GL_FALSE, m_posChannel * sizeof(float),
			static_cast<void*>(0));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void ParticlePointSpriteDrawable::setParticleVBO(unsigned int vbo, int numParticles)
	{
		m_vboCreateBySelf = false;
		m_particleVBO = vbo;
		m_numParticles = numParticles;

		glBindVertexArray(m_particleVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_particleVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, m_posChannel, GL_FLOAT, GL_FALSE, m_posChannel * sizeof(float),
			static_cast<void*>(0));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void ParticlePointSpriteDrawable::render(Camera3D::ptr camera, Light::ptr sunLight,
		Camera3D::ptr lightCamera, Shader::ptr shader)
	{
		if (!m_visiable) return;

		// calculate particle size scale factor.
		float aspect = camera->getAspect();
		float fovy = camera->getFovy();
		int width = RenderDevice::getSingleton()->getWindowWidth();
		float pointScale = 1.0f * width / aspect * (1.0f / tanf(glm::radians(fovy) * 0.5f));

		// render state.
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

		// shader
		shader = m_shaderMgr->getShader(m_shaderIndex);
		shader->bind();
		if (sunLight)
			sunLight->setLightUniform(shader, camera);
		shader->setInt("image", 0);
		shader->setVec3("baseColor", m_baseColor);
		shader->setFloat("pointScale", pointScale);
		shader->setFloat("pointSize", m_particleRadius);
		shader->setMat4("modelMatrix", m_transformation.getWorldMatrix());
		shader->setMat4("viewMatrix", camera->getViewMatrix());
		shader->setMat4("projectMatrix", camera->getProjectMatrix());

		// texture
		m_textureMgr->bindTexture(m_particleTex, 0);

		// draw
		glBindVertexArray(m_particleVAO);
		glDrawArrays(GL_POINTS, 0, m_numParticles);
		glBindVertexArray(0);

		// restore.
		m_shaderMgr->unBindShader();
		m_textureMgr->unBindTexture(m_particleTex);
		glDisable(GL_PROGRAM_POINT_SIZE);
	}

	void ParticlePointSpriteDrawable::renderDepth(Shader::ptr shader, Camera3D::ptr lightCamera)
	{
		if (!m_visiable || !m_produceShadow)
			return;
		static ShaderMgr::ptr shaderMgr = ShaderMgr::getSingleton();
		static unsigned int index = shaderMgr->loadShader("spriteDepth", "./glsl/spriteDepth.vert",
			"./glsl/spriteDepth.frag");
		shader = shaderMgr->getShader(index);
		shader->bind();
		shader->setInt("image", 0);
		shader->setMat4("lightSpaceMatrix",
			lightCamera->getProjectMatrix() * lightCamera->getViewMatrix());
		shader->setMat4("modelMatrix", m_transformation.getWorldMatrix());
		shader->setFloat("pointSize", m_particleRadius);
		m_textureMgr->bindTexture(m_particleTex, 0);

		// draw
		glEnable(GL_PROGRAM_POINT_SIZE);
		glBindVertexArray(m_particleVAO);
		glDrawArrays(GL_POINTS, 0, m_numParticles);
		glBindVertexArray(0);
		glDisable(GL_PROGRAM_POINT_SIZE);

		ShaderMgr::getSingleton()->unBindShader();
		return;
	}

	void ParticlePointSpriteDrawable::generateGaussianMap(int resolution)
	{
		// generate gaussian map manually.
		unsigned char *data = new unsigned char[4 * resolution * resolution];
		int index = 0;
		float step = 2.0f / resolution;
		float X, Y, value;
		float distance;
		Y = -1.0f;
		for (int y = 0; y < resolution; ++y, Y += step)
		{
			float Y2 = Y * Y;
			X = -1.0f;
			for (int x = 0; x < resolution; ++x, X += step)
			{
				distance = sqrtf(X * X + Y2);
				if (distance > 1.0f)distance = 1.0f;
				value = 2 * pow(distance, 3.0f) - 3 * pow(distance, 2.0f) + 1.0f;
				data[index + 0] = static_cast<char>(value * 255);
				data[index + 1] = static_cast<char>(value * 255);
				data[index + 2] = static_cast<char>(value * 255);
				data[index + 3] = static_cast<char>(value * 255);
				index += 4;
			}
		}

		m_particleTex = m_textureMgr->loadTexture2D("GaussianMap", data, resolution, resolution, 4);
		delete []data;
	}
}
