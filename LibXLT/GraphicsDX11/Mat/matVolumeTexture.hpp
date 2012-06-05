/****************************************************************************\
**  matVolumeTexture.hpp
**
**      matVolumeTexture is a matTexture which represents a 3d volume
**	or 1D array of 2d Texture layers.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_VOLUMETEXTURE_HPP
#error matVolumeTexture.hpp multiply included
#endif
#define MAT_VOLUMETEXTURE_HPP

#ifndef MAT_TEXTUREDX11_HPP
#include "GraphicsDX11/mat/matTextureDX11.hpp"
#endif


#ifndef MAT_TEXTURETYPE_HPP
#include "Graphics/mat/matTextureType.hpp"
#endif

class matVolumeTexture : public matTextureDX11
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
		void Make(int i_Width, int i_Height, int i_Depth, const g2dPFD& i_PFD, BIND_TYPE i_Bindings, void* i_pInitialData );

		//----------------------------------------------------------------------------
		//	GetSize returns approximate amount of memory (in bytes) being
		// used by this texture
		//----------------------------------------------------------------------------
		float GetSize() const;

		ID3D11UnorderedAccessView* GetUnorderedAccessView(){ return m_pUnorderedAccessView; }

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
//		ID3D11DepthStencilView* m_pDepthView;
		ID3D11UnorderedAccessView* m_pUnorderedAccessView;

		//--------------------------------------------------------------------
		//	This operator = is not implemented.  It is placed here in private
		//	to prevent its usage.
		//--------------------------------------------------------------------
		matVolumeTexture& operator = (const matVolumeTexture& i_Image);

		// the volume texture depth
		int m_NumLayers;

		ID3D11Texture3D* m_Texture3D;
		ID3D11ShaderResourceView* m_Texture3DSRV;

		//----------------------------------------------------------------------------
		//	CreateSRV
		//----------------------------------------------------------------------------
		void CreateSRV(ID3D11Texture3D* i_pNewTexture);
};


