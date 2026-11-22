/*****************************************************************************
**  g3dTextureTableOptiX.cpp
**
**      See g3dTextureTableOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DTEXTURETABLEOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dTextureTableOptiX.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif

#include <algorithm>
#include <cstring>

namespace gox
{
	g3dTextureTableOptiX::g3dTextureTableOptiX(cuExternalImageCache& i_Images)
		: m_Images(i_Images)
		, m_BuildingWait(0)
		, m_bUploaded(false)
		, m_Version(0)
		, m_WaitValue(0)
	{
	}

	g3dTextureTableOptiX::~g3dTextureTableOptiX()
	{
	}

	void g3dTextureTableOptiX::Begin()
	{
		m_Images.Collect();
		m_Index.clear();
		m_Building.clear();
		m_BuildingGenerations.clear();
		m_BuildingWait = 0;
	}

	unsigned g3dTextureTableOptiX::Add(const matTexture* i_pTexture, cuExternalImageCache::Mode i_Mode)
	{
		gvk::vkdImage* image = i_pTexture ? gvk::g3dVkTextureUtil::GetImage(i_pTexture) : NULL;
		if (!image)
		{
			return c_optxNoTexture;
		}
		const std::pair<const void*, int> key(image, int(i_Mode));
		auto found = m_Index.find(key);
		if (found != m_Index.end())
		{
			return found->second;
		}
		unsigned index = c_optxNoTexture;
		const CUtexObject object = m_Images.Texture(*image, i_Mode);
		const cuExternalImageCache::Image* mapped = object ? m_Images.Map(*image) : NULL;
		if (mapped)
		{
			optxTexture record = {};
			record.m_Object = object;
			record.m_Width = mapped->m_Width;
			record.m_Height = mapped->m_Height;
			record.m_Swizzle = mapped->m_Swizzle;
			index = static_cast<unsigned>(m_Building.size());
			m_Building.push_back(record);
			m_BuildingGenerations.push_back(image->Generation());
			m_BuildingWait = std::max(m_BuildingWait, mapped->m_WriteValue);
		}
		m_Index.emplace(key, index);
		return index;
	}

	bool g3dTextureTableOptiX::End()
	{
		m_WaitValue = m_BuildingWait;
		const bool same_table = m_bUploaded && m_Building.size() == m_Table.size() &&
								(m_Table.empty() || std::memcmp(m_Building.data(), m_Table.data(), m_Table.size() * sizeof(optxTexture)) == 0);
		if (same_table && m_BuildingGenerations == m_Generations)
		{
			return false;
		}
		if (!same_table)
		{
			//	Stream-ordered: a launch queued before reads the old table
			m_bUploaded = m_Textures.Upload(m_Building.data(), m_Building.size() * sizeof(optxTexture));
			m_Table = m_Building;
			if (!m_bUploaded)
			{
				m_Table.clear();
			}
		}
		m_Generations = m_BuildingGenerations;
		++m_Version;
		return true;
	}

	void g3dTextureTableOptiX::Release()
	{
		m_Textures.Free();
		m_Table.clear();
		m_Generations.clear();
		m_bUploaded = false;
	}
}
