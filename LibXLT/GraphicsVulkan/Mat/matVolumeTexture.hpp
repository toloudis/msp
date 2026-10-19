/****************************************************************************\
**  matVolumeTexture.hpp
**
**	GraphicsVulkan port of GraphicsDX11's matVolumeTexture: a matTexture
**	which represents a 3d volume or 1D array of 2d Texture layers.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_MATVOLUMETEXTURE_HPP
#error matVolumeTexture.hpp multiply included
#endif
#define GVK_MATVOLUMETEXTURE_HPP

#ifndef GVK_MATTEXTUREVK_HPP
#include "GraphicsVulkan/Mat/matTextureVk.hpp"
#endif
#ifndef MAT_TEXTURETYPE_HPP
#include "Graphics/Mat/matTextureType.hpp"
#endif

#include <memory>

namespace gvk
{
	class matVolumeTexture : public matTextureVk
	{
		public:

			//--------------------------------------------------------------------
			//	This constructor will not typically be used by the mat client,
			//	since textures are loaded and managed by the matTextureMgr.
			//--------------------------------------------------------------------
			matVolumeTexture();

			//--------------------------------------------------------------------
			//--------------------------------------------------------------------
			virtual ~matVolumeTexture();

			//--------------------------------------------------------------------
			//	ReloadInfo causes the matVolumeTexture to regenerate its
			//	parameters from the image.
			//--------------------------------------------------------------------
			void ReloadInfo();

			//--------------------------------------------------------------------
			//	Make makes the surface into one with the given dimensions and
			//	pixel format (one mip); i_pInitialData slices tightly packed.
			//--------------------------------------------------------------------
			void Make(int i_Width, int i_Height, int i_Depth, const g2dPFD& i_PFD, BIND_TYPE i_Bindings, void* i_pInitialData );

			//----------------------------------------------------------------------------
			//	GetSize returns approximate amount of memory (in kbytes) being
			// used by this texture
			//----------------------------------------------------------------------------
			float GetSize() const;

			//	The image as an unordered access view (u register); NULL unless
			//	made with BIND_UNORDERED_ACCESS
			vkdImage* GetUnorderedAccessView() { return m_bUnorderedAccess ? m_pImage.get() : NULL; }

			//--------------------------------------------------------------------
			//	GetImage returns the image, usable for shaders.
			//--------------------------------------------------------------------
			virtual vkdImage* GetImage() const;

			//--------------------------------------------------------------------
			//	SetSurface replaces the image; i_DxgiFormat is the format of
			//	its data
			//--------------------------------------------------------------------
			virtual void SetSurface(std::unique_ptr<vkdImage> i_pImage, uint32_t i_DxgiFormat);

		private:
			bool m_bUnorderedAccess;

			//--------------------------------------------------------------------
			// number of layers in the volume
			//--------------------------------------------------------------------
			int m_NumLayers;

			std::unique_ptr<vkdImage> m_pImage;
			uint32_t m_Format;
	};
}
