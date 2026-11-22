/****************************************************************************\
**  matMipTexture.hpp
**
**	GraphicsVulkan port of GraphicsDX11's matMipTexture: a matTexture
**	which has mip-map levels.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_MATMIPTEXTURE_HPP
#error matMipTexture.hpp multiply included
#endif
#define GVK_MATMIPTEXTURE_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif
#ifndef GVK_MATTEXTUREVK_HPP
#include "GraphicsVulkan/Mat/matTextureVk.hpp"
#endif

#include <memory>

namespace gvk
{
	class g2dTextureVk;

	class matMipTexture : public matTextureVk
	{
		public:

			//--------------------------------------------------------------------
			//	This constructor will not typically be used by the mat client,
			//	since textures are loaded and managed by the matTextureMgr.
			//--------------------------------------------------------------------
			matMipTexture();

			//--------------------------------------------------------------------
			//--------------------------------------------------------------------
			virtual ~matMipTexture();

			//----------------------------------------------------------------------------
			//	GetSize returns approximate amount of memory (in kbytes) being
			// used by this texture
			//----------------------------------------------------------------------------
			float GetSize() const;

			//--------------------------------------------------------------------
			//	ReloadInfo causes the matMipTexture to regenerate its
			//	parameters from the image.
			//--------------------------------------------------------------------
			void ReloadInfo();

			//--------------------------------------------------------------------
			//	Make makes the surface into one with the given dimensions
			//	and pixel format; i_NumMipLevels 0 is a full chain. With
			//	i_PixelData (mip 0) the other mips are made from it on the GPU.
			//--------------------------------------------------------------------
			void Make(int i_Width, int i_Height, const g2dPFD& i_PFD, int i_NumMipLevels,
				void* i_PixelData = NULL);

			//--------------------------------------------------------------------
			//	GetImage returns the image, usable for shaders.
			//--------------------------------------------------------------------
			virtual vkdImage* GetImage() const;

			//--------------------------------------------------------------------
			//	SetSurface replaces the image; i_DxgiFormat is the format of
			//	its data
			//--------------------------------------------------------------------
			virtual void SetSurface(std::unique_ptr<vkdImage> i_pImage, uint32_t i_DxgiFormat);

			//--------------------------------------------------------------------
			//	GetMipLevels gets the current texture mip level
			//--------------------------------------------------------------------
			int GetMipLevels();

		private:

			//--------------------------------------------------------------------
			//	This operator = is not implemented.  It is placed here in private
			//	to prevent its usage.
			//--------------------------------------------------------------------
			matMipTexture& operator = (const matMipTexture& i_Image);

			shared_ptr<g2dTextureVk> m_Texture;
	};
}
