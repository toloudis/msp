/****************************************************************************\
**  matPlainTexture.hpp
**
**      matPlainTexture is a matTexture which represents an ordinary
**	rectangular texture with no special ability.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_PLAINTEXTURE_HPP
#error matPlainTexture.hpp multiply included
#endif
#define MAT_PLAINTEXTURE_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef MAT_TEXTUREDX11_HPP
#include "GraphicsDX11/mat/matTextureDX11.hpp"
#endif

class g2dTextureDX11;

class matPlainTexture : public matTextureDX11
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
		//	parameters from the PAC.  Again, this function should only be
		//	called by the Terawatt PAC components.
		//--------------------------------------------------------------------
		void ReloadInfo();

		//--------------------------------------------------------------------
		//	Make makes the surface into one with the given dimensions and
		//	pixel format.  If the pixel format is not supported it will
		//	throw a matUnsupportedPixelFormatX, and leave the old surface
		//	intact.
		//--------------------------------------------------------------------
		void Make(int i_Width, int i_Height, const g2dPFD& i_PFD, 
			void* i_PixelData = NULL);

		//--------------------------------------------------------------------
		//	GetSurface returns a pointer usable for shaders.
		//--------------------------------------------------------------------
		virtual ID3D11ShaderResourceView* GetSurface() const;
		virtual g2dD3D11ResourcePtr GetResource() const;

		//--------------------------------------------------------------------
		//	SetSurface sets the DirectDraw surface pointer
		//--------------------------------------------------------------------
		virtual void SetSurface(g2dD3D11ResourcePtr i_Surface);
	private:

		//--------------------------------------------------------------------
		//	This operator = is not implemented.  It is placed here in private
		//	to prevent its usage.
		//--------------------------------------------------------------------
		matPlainTexture& operator = (const matPlainTexture& i_Image);

		shared_ptr<g2dTextureDX11> m_Texture;
};


