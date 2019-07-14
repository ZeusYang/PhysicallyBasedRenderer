#pragma once

#include <glm/glm.hpp>

#include "../Manager/Mesh.h"

namespace Renderer
{
	class AABBBoundingBox
	{
	private:
		glm::vec3 m_min, m_max;

	public:
		AABBBoundingBox(glm::vec3 min = glm::vec3(+FLT_MAX), glm::vec3 max = glm::vec3(-FLT_MAX));
		AABBBoundingBox(const AABBBoundingBox &rhs);
		~AABBBoundingBox() = default;
		
		void reset();
		bool isValid()const;
		void addBoundingBox(const std::vector<Vertex> &data);
		void addBoundingBox(const std::vector<Vertex> &data, const glm::mat4 &transform);
		void mergeBoundingBox(const AABBBoundingBox &target);

		float getLongestEdge()const;
		float getShortestEdge()const;
		float getDiagonalLength()const { return glm::length(m_max - m_min); }
		glm::vec3 getMaxPoint()const { return m_max; }
		glm::vec3 getMinPoint()const { return m_min; }
		glm::vec3 getCenter() const { return (m_max + m_min) * 0.5f; }

		void debug()const;
	};
}