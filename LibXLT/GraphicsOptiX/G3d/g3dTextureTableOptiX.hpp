/*****************************************************************************
**  g3dTextureTableOptiX.hpp
**
**      g3dTextureTableOptiX is the texture table the device programs read
**	(optxMaterialTypes.h's optxTexture records): every texture a
**	renderer's materials and projected lights use, each with a sampler
**	mode, as CUDA texture objects from the shared cuExternalImageCache. A
**	renderer keeps one and refills it before each launch:
**
**	- Begin starts a new list;
**	- Add gives a texture's index (or c_optxNoTexture), adding it the first
**	  time it is asked for with that mode;
**	- End uploads the list if it differs from the last one. The version
**	  rises when the list changes or any listed image's contents may have
**	  (vkdImage::Generation): accumulation restarts. GetWaitValue is the
**	  latest Vulkan timeline value of their writes, which CUDA must wait
**	  for before a launch samples them.
**
**	Render thread, with the OptiX context current.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DTEXTURETABLEOPTIX_HPP
#error g3dTextureTableOptiX.hpp multiply included
#endif
#define GOX_G3DTEXTURETABLEOPTIX_HPP

#ifndef GOX_CUBUFFER_HPP
#include "GraphicsOptiX/cu/cuBuffer.hpp"
#endif
#ifndef GOX_CUEXTERNALIMAGECACHE_HPP
#include "GraphicsOptiX/cu/cuExternalImageCache.hpp"
#endif
#ifndef GOX_OPTXMATERIALTYPES_H
#include "GraphicsOptiX/optx/private/Programs/optxMaterialTypes.h"
#endif

#include <cstdint>
#include <map>
#include <utility>
#include <vector>

class matTexture;

namespace gox
{
	class g3dTextureTableOptiX
	{
		public:
			explicit g3dTextureTableOptiX(cuExternalImageCache& i_Images);
			~g3dTextureTableOptiX();
			g3dTextureTableOptiX(const g3dTextureTableOptiX&) = delete;
			g3dTextureTableOptiX& operator=(const g3dTextureTableOptiX&) = delete;

			void Begin();
			unsigned Add(const matTexture* i_pTexture, cuExternalImageCache::Mode i_Mode);
			bool End();

			//--------------------------------------------------------------------
			//	Release - frees the device table; the next End uploads again
			//--------------------------------------------------------------------
			void Release();

			CUdeviceptr GetTextures() const { return m_Textures.Get(); }
			unsigned GetCount() const { return static_cast<unsigned>(m_Table.size()); }
			uint64_t GetVersion() const { return m_Version; }
			uint64_t GetWaitValue() const { return m_WaitValue; }
			const std::vector<optxTexture>& GetTable() const { return m_Table; }

		private:
			cuExternalImageCache&		m_Images;
			std::map<std::pair<const void*, int>, unsigned>	m_Index;	// this list's, by (image, mode)
			std::vector<optxTexture>	m_Building;
			std::vector<uint64_t>		m_BuildingGenerations;
			uint64_t					m_BuildingWait;
			std::vector<optxTexture>	m_Table;			// the last End's
			std::vector<uint64_t>		m_Generations;
			bool						m_bUploaded;
			cuBuffer					m_Textures;
			uint64_t					m_Version;
			uint64_t					m_WaitValue;
	};
}
