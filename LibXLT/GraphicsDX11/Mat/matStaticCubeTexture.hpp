/****************************************************************************\
**  matStaticCubeTexture.hpp
**
**      matStaticCubeTexture is a matTexture which represents a cubic
**	environment map which is preset at creation (not rendered to).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_STATICCUBETEXTURE_HPP
#error matStaticCubeTexture.hpp multiply included
#endif
#define MAT_STATICCUBETEXTURE_HPP

#ifndef MAT_TEXTUREDX11_HPP
#include "GraphicsDX11/mat/matTextureDX11.hpp"
#endif


class matStaticCubeTexture : public matTextureDX11
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
		void Make(int i_Width, int i_Height, const g2dPFD& i_PFD);

		//----------------------------------------------------------------------------
		//	GetSize returns approximate amount of memory (in kbytes) being
		// used by this texture
		//----------------------------------------------------------------------------
		float GetSize() const;

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
		matStaticCubeTexture& operator = (const matStaticCubeTexture& i_Image);

		ID3D11Texture2D* m_Texture2D;
		ID3D11ShaderResourceView* m_Texture2DSRV;
};


