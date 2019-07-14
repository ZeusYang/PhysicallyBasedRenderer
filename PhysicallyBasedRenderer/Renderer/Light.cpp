#include "Light.h"

#include <sstream>

namespace Renderer
{
	void Light::setLightColor(glm::vec3 radiance)
	{
		m_radiance = radiance;
	}

	void DirectionalLight::setDirection(glm::vec3 dir)
	{
		m_direction = glm::normalize(dir);
	}

	void DirectionalLight::setLightUniform(Shader::ptr shader, Camera3D::ptr camera)
	{
		shader->setVec3("cameraPos", camera->getPosition());
		shader->setVec3("dirLight.direction", m_direction);
		shader->setVec3("dirLight.radiance", m_radiance);
	}

	glm::vec3 DirectionalLight::getDirection() const
	{
		return m_direction;
	}

	PointLight::PointLight()
	{
		m_atteunationCoff = 0.5f;
		m_radiance = glm::vec3(1.0f);
		m_radius = sqrt(256.0f * 1.0f / (1.0f * m_atteunationCoff));
	}

	void PointLight::setPosition(glm::vec3 pos, unsigned int order)
	{
		m_order = order;
		m_position = pos;
	}

	void PointLight::setAttenuationCoff(float coff)
	{
		m_atteunationCoff = coff;
		// calculate point light's volume.
		GLfloat lightMax = std::fmaxf(std::fmaxf(m_radiance.r, m_radiance.g), m_radiance.b);
		m_radius = sqrt(256.0f * lightMax / (1.0f * m_atteunationCoff));
	}

	void PointLight::setLightColor(glm::vec3 radiance)
	{
		Light::setLightColor(radiance);
		// calculate point light's volume.
		GLfloat lightMax = std::fmaxf(std::fmaxf(m_radiance.r, m_radiance.g), m_radiance.b);
		m_radius = sqrt(256.0f * lightMax / (1.0f * m_atteunationCoff));
	}

	void PointLight::setLightUniform(Shader::ptr shader, Camera3D::ptr camera)
	{
		std::stringstream ss;
		ss << m_order;
		shader->setVec3("cameraPos", camera->getPosition());
		std::string str = "pointLight[" + ss.str() + "].";
		shader->setVec3(str + "position", m_position);
		shader->setVec3(str + "radiance", m_radiance);
		shader->setFloat(str + "radius", m_radius);
		shader->setFloat("lightAttenuationCoff", m_atteunationCoff);
	}

	glm::vec3 PointLight::getPosition() const
	{
		return m_position;
	}

}
