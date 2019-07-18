#pragma once

#include <map>
#include <vector>
#include <iostream>

#include "Texture.h"
#include "Singleton.h"

namespace Renderer
{
	class TextureMgr : public Singleton<TextureMgr>
	{
	private:
		std::vector<Texture::ptr> m_units;
		std::map<std::string, unsigned int> m_unitMap;

	public:
		typedef std::shared_ptr<TextureMgr> ptr;

		TextureMgr() = default;
		~TextureMgr() = default;

		static TextureMgr::ptr getSingleton();

		unsigned int loadTexture2D(const std::string &name, const std::string &path,
			glm::vec4 bColor = glm::vec4(1.0f))
		{
			if (m_unitMap.find(name) != m_unitMap.end())
				return m_unitMap[name];
			Texture::ptr tex(new Texture2D(path, bColor));
			m_units.push_back(tex);
			m_unitMap[name] = m_units.size() - 1;
			return m_units.size() - 1;
		}

		unsigned int loadTexture2D(const std::string &name, unsigned char *data, int width,
			int height, int channel)
		{
			if (m_unitMap.find(name) != m_unitMap.end())
				return m_unitMap[name];
			Texture::ptr tex(new Texture2D(data, width, height, channel));
			m_units.push_back(tex);
			m_unitMap[name] = m_units.size() - 1;
			return m_units.size() - 1;
		}

		unsigned int loadTexture2DHdr(const std::string &name, const std::string &path)
		{
			if (m_unitMap.find(name) != m_unitMap.end())
				return m_unitMap[name];
			Texture::ptr tex(new Texture2DHdr(path));
			m_units.push_back(tex);
			m_unitMap[name] = m_units.size() - 1;
			return m_units.size() - 1;
		}

		unsigned int loadTexture2DHdrRaw(const std::string &name, const char *data,
			int width, int height)
		{
			if (m_unitMap.find(name) != m_unitMap.end())
				return m_unitMap[name];
			Texture::ptr tex(new Texture2DHdr(data, width, height));
			m_units.push_back(tex);
			m_unitMap[name] = m_units.size() - 1;
			return m_units.size() - 1;
		}

		unsigned int loadTextureCube(const std::string &name, const std::string &path,
			const std::string &pFix)
		{
			if (m_unitMap.find(name) != m_unitMap.end())
				return m_unitMap[name];
			Texture::ptr tex(new TextureCube(path, pFix));
			m_units.push_back(tex);
			m_unitMap[name] = m_units.size() - 1;
			return m_units.size() - 1;
		}

		unsigned int loadTextureCubeHdrRaw(const std::string &name, const char *data,
			int width, int height, bool mipmap = false)
		{
			if (m_unitMap.find(name) != m_unitMap.end())
				return m_unitMap[name];
			Texture::ptr tex(new TextureCubeHdrRaw(data, width, height, mipmap));
			m_units.push_back(tex);
			m_unitMap[name] = m_units.size() - 1;
			return m_units.size() - 1;
		}

		unsigned int loadTextureDepth(const std::string &name, int width, int height)
		{
			if (m_unitMap.find(name) != m_unitMap.end())
				return m_unitMap[name];
			Texture::ptr tex(new TextureDepth(width, height));
			m_units.push_back(tex);
			m_unitMap[name] = m_units.size() - 1;
			return m_units.size() - 1;
		}

		unsigned int loadTextureColor(const std::string &name, int width, int height, bool hdr = false)
		{
			if (m_unitMap.find(name) != m_unitMap.end())
				return m_unitMap[name];
			Texture::ptr tex(new TextureColor(width, height, hdr));
			m_units.push_back(tex);
			m_unitMap[name] = m_units.size() - 1;
			return m_units.size() - 1;
		}

		unsigned int loadTextureRaw(const std::string &name, int width, int height, const char *data)
		{
			if (m_unitMap.find(name) != m_unitMap.end())
				return m_unitMap[name];
			Texture::ptr tex(new TextureRaw(width, height, data));
			m_units.push_back(tex);
			m_unitMap[name] = m_units.size() - 1;
			return m_units.size() - 1;
		}

		Texture::ptr getTexture(const std::string &name)
		{
			if (m_unitMap.find(name) == m_unitMap.end())
				return nullptr;
			return m_units[m_unitMap[name]];
		}

		Texture::ptr getTexture(unsigned int unit)
		{
			if (unit >= m_units.size())
				return nullptr;
			return m_units[unit];
		}

		unsigned int getTextureIndex(const std::string &name)
		{
			if (m_unitMap.find(name) == m_unitMap.end())
				return -1;
			return m_unitMap[name];
		}

		bool bindTexture(unsigned int index, unsigned int unit)
		{
			if (index >= m_units.size())
				return false;
			m_units[index]->bind(unit);
			return true;
		}

		bool bindTexture(const std::string &name, unsigned int unit)
		{
			if (m_unitMap.find(name) == m_unitMap.end())
			{
				return false;
			}
			m_units[m_unitMap[name]]->bind(unit);
			return true;
		}

		bool unBindTexture(unsigned int index)
		{
			if (index >= m_units.size())
				return false;
			m_units[index]->unBind();
			return true;
		}

		bool unBindTexture(const std::string &name)
		{
			if (m_unitMap.find(name) == m_unitMap.end())
				return false;
			m_units[m_unitMap[name]]->unBind();
			return true;
		}

		bool deallocateTexture(unsigned int index)
		{
			if (index >= m_units.size())
				return false;
			m_units[index] = nullptr;
			return true;
		}

		bool deallocateTexture(const std::string &name)
		{
			if (m_unitMap.find(name) == m_unitMap.end())
				return false;
			m_units[m_unitMap[name]] = nullptr;
			return true;
		}
	};
}

