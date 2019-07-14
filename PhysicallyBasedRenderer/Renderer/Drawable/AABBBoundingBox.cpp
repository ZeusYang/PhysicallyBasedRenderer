#include "AABBBoundingBox.h"

#include <iostream>
#include <algorithm>

namespace Renderer
{
	AABBBoundingBox::AABBBoundingBox(glm::vec3 min, glm::vec3 max)
	{
		m_min = min;
		m_max = max;
	}

	AABBBoundingBox::AABBBoundingBox(const AABBBoundingBox & rhs)
	{
		m_min = rhs.m_min;
		m_max = rhs.m_max;
	}

	void AABBBoundingBox::reset()
	{
		m_min = glm::vec3(+FLT_MAX);
		m_max = glm::vec3(-FLT_MAX);
	}

	bool AABBBoundingBox::isValid() const
	{
		if(m_min.x > m_max.x || m_min.y > m_max.y || m_min.z > m_max.z)
			return false;
		return true;
	}

	void AABBBoundingBox::addBoundingBox(const std::vector<Vertex>& data)
	{
		for (unsigned int i = 0; i < data.size(); ++i)
		{
			m_min.x = std::min(data[i].position.x, m_min.x);
			m_min.y = std::min(data[i].position.y, m_min.y);
			m_min.z = std::min(data[i].position.z, m_min.z);

			m_max.x = std::max(data[i].position.x, m_max.x);
			m_max.y = std::max(data[i].position.y, m_max.y);
			m_max.z = std::max(data[i].position.z, m_max.z);
		}
	}

	void AABBBoundingBox::addBoundingBox(const std::vector<Vertex>& data, const glm::mat4 & transform)
	{
		glm::vec3 tmpPos;
		for (unsigned int i = 0; i < data.size(); ++i)
		{
			tmpPos = glm::vec3(transform * glm::vec4(data[i].position.x,
				data[i].position.y, data[i].position.z, 1.0f));
			m_min.x = std::min(tmpPos.x, m_min.x);
			m_min.y = std::min(tmpPos.y, m_min.y);
			m_min.z = std::min(tmpPos.z, m_min.z);

			m_max.x = std::max(tmpPos.x, m_max.x);
			m_max.y = std::max(tmpPos.y, m_max.y);
			m_max.z = std::max(tmpPos.z, m_max.z);
		}
	}

	void AABBBoundingBox::mergeBoundingBox(const AABBBoundingBox & target)
	{
		m_min.x = std::min(this->m_min.x, target.m_min.x);
		m_min.y = std::min(this->m_min.y, target.m_min.y);
		m_min.z = std::min(this->m_min.z, target.m_min.z);

		m_max.x = std::max(this->m_max.x, target.m_max.x);
		m_max.y = std::max(this->m_max.y, target.m_max.y);
		m_max.z = std::max(this->m_max.z, target.m_max.z);
	}

	float AABBBoundingBox::getLongestEdge() const
	{
		float edge1 = m_max.x - m_min.x;
		float edge2 = m_max.y - m_min.y;
		float edge3 = m_max.z - m_min.z;
		return std::max(edge1, std::max(edge2, edge3));
	}

	float AABBBoundingBox::getShortestEdge() const
	{
		float edge1 = m_max.x - m_min.x;
		float edge2 = m_max.y - m_min.y;
		float edge3 = m_max.z - m_min.z;
		return std::min(edge1, std::min(edge2, edge3));
	}

	void AABBBoundingBox::debug() const
	{
		std::cout << "min:" << m_min.x << "," << m_min.y << "," << m_min.z << std::endl;
		std::cout << "max:" << m_max.x << "," << m_max.y << "," << m_max.z << std::endl;
		std::cout << "longest edge:" << getLongestEdge() << std::endl;
		std::cout << "shortest edge:" << getShortestEdge() << std::endl;
		std::cout << "diagonal length:" << getDiagonalLength() << std::endl;
	}

}