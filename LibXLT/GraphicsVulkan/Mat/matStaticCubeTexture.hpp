/****************************************************************************\
**  matStaticCubeTexture.hpp
**
**	GraphicsVulkan port of GraphicsDX11's matStaticCubeTexture: a
**	matTexture which represents a cubic environment map which is preset
**	at creation (not rendered to).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_MATSTATICCUBETEXTURE_HPP
#error matStaticCubeTexture.hpp multiply included
#endif
#define GVK_MATSTATICCUBETEXTURE_HPP

#ifndef GVK_MATTEXTUREVK_HPP
#include "GraphicsVulkan/Mat/matTextureVk.hpp"
#endif

#include <memory>

namespace gvk
{
	class matStaticCubeTexture : public matTextureVk
	{
		public:

			//--------------------------------------------------------------------
			//	This constructor will not typically be used by the mat client,
			//	since textures are loaded and managed by the matTextureMgr.
			//--------------------------------------------------------------------
			matStaticCubeTexture();

			//--------------------------------------------------------------------
			//--------------------------------------------------------------------
			virtual ~matStaticCubeTexture();

			//--------------------------------------------------------------------
			//	ReloadInfo causes the matStaticCubeTexture to regenerate its
			//	parameters from the image.
			//--------------------------------------------------------------------
			void ReloadInfo();

			//--------------------------------------------------------------------
			//	Make makes the surface into one with the given dimensions and
			//	pixel format (a full mip chain, not filled).
			//--------------------------------------------------------------------
			void Make(int i_Width, int i_Height, const g2dPFD& i_PFD);

			//----------------------------------------------------------------------------
			//	GetSize returns approximate amount of memory (in kbytes) being
			// used by this texture
			//----------------------------------------------------------------------------
			float GetSize() const;

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
			std::unique_ptr<vkdImage> m_pImage;
			uint32_t m_Format;
	};
}
