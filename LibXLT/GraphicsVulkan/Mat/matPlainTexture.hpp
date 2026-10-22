/****************************************************************************\
**  matPlainTexture.hpp
**
**	GraphicsVulkan port of GraphicsDX11's matPlainTexture: a matTexture
**	which represents an ordinary rectangular texture with no special
**	ability.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_MATPLAINTEXTURE_HPP
#error matPlainTexture.hpp multiply included
#endif
#define GVK_MATPLAINTEXTURE_HPP

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

	class matPlainTexture : public matTextureVk
	{
		public:

			//--------------------------------------------------------------------
			//	This constructor will not typically be used by the mat client,
			//	since textures are loaded and managed by the matTextureMgr.
			//--------------------------------------------------------------------
			matPlainTexture();

			//--------------------------------------------------------------------
			//--------------------------------------------------------------------
			virtual ~matPlainTexture();

			//--------------------------------------------------------------------
			//	ReloadInfo causes the matPlainTexture to regenerate its
			//	parameters from the image.
			//--------------------------------------------------------------------
			void ReloadInfo();

			//--------------------------------------------------------------------
			//	Make makes the surface into one with the given dimensions and
			//	pixel format.
			//--------------------------------------------------------------------
			void Make(int i_Width, int i_Height, const g2dPFD& i_PFD,
				void* i_PixelData = NULL);

			//--------------------------------------------------------------------
			//	GetImage returns the image, usable for shaders.
			//--------------------------------------------------------------------
			virtual vkdImage* GetImage() const;

			//--------------------------------------------------------------------
			//	SetSurface replaces the image (the old one is destroyed once the
			//	GPU is done with it); i_DxgiFormat is the format of its data
			//--------------------------------------------------------------------
			virtual void SetSurface(std::unique_ptr<vkdImage> i_pImage, uint32_t i_DxgiFormat);

		private:

			//--------------------------------------------------------------------
			//	This operator = is not implemented.  It is placed here in private
			//	to prevent its usage.
			//--------------------------------------------------------------------
			matPlainTexture& operator = (const matPlainTexture& i_Image);

			shared_ptr<g2dTextureVk> m_Texture;
	};
}
