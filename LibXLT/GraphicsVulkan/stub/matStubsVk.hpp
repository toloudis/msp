/*****************************************************************************
**  matStubsVk.hpp
**
**      Vulkan implementations of the material hooks that aren't ported yet
**	(see g2dStubsVk.hpp for the rules): the textures matTextureMgrVk doesn't
**	make yet keep their sizes and names but have no GPU image.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_MATSTUBSVK_HPP
#error matStubsVk.hpp multiply included
#endif
#define GVK_MATSTUBSVK_HPP

#ifndef MAT_TEXTURE_HPP
#include "Graphics/Mat/matTexture.hpp"
#endif
#ifndef GVK_MATTEXTUREVK_HPP
#include "GraphicsVulkan/Mat/matTextureVk.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <string>
#include <vector>

namespace gvk
{
	//	The texture shadow maps, reflective shadow maps and cube render
	//	targets get until Plan 5 ports them (matTextureMgrVk makes them): a
	//	size and a pixel format, no image
	class matStubTextureVk : public matTextureVk
	{
		public:
			matStubTextureVk(int i_Width, int i_Height, const g2dPFD* i_pPFD, const std::string& i_Name);
			virtual ~matStubTextureVk();
			virtual vkdImage* GetImage() const { return NULL; }
			const std::string& Name() const { return m_Name; }

		private:
			std::string m_Name;
	};
}
