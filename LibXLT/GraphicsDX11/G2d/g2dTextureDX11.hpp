/****************************************************************************\
**  g2dTextureDX11.hpp
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_TEXTUREDX11_HPP
#error g2dTextureDX11.hpp multiply included
#endif
#define G2D_TEXTUREDX11_HPP

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

#ifndef G2D_RESOURCECOUNTERDX11_HPP
#include "GraphicsDX11/g2d/g2dResourceCounterDX11.hpp"
#endif

class g2dTextureDX11
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	g2dTextureDX11();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~g2dTextureDX11();

	//--------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions and
	//	pixel format.  If the pixel format is not supported it will
	//	throw a matUnsupportedPixelFormatX, and leave the old surface
	//	intact.  
	//--------------------------------------------------------------------
	void Make(int i_Width, int i_Height, DXGI_FORMAT i_Format, 
		/*DXGI_SAMPLE_DESC i_Multisample,*/
		g2dResourceCounterDX11::eResourceCategory i_Category = g2dResourceCounterDX11::eTexture);

	//------------------------------------------------------------------------
	// This object will take ownership of the resource!
	//------------------------------------------------------------------------
	void Make(g2dD3D11TexturePtr i_TextureResource,
		g2dResourceCounterDX11::eResourceCategory i_Category = g2dResourceCounterDX11::eTexture);

	//------------------------------------------------------------------------
	// DANGER! Do not release these pointers! No shenanigans please. I own them.
	//------------------------------------------------------------------------
	ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView; };
	g2dD3D11TexturePtr GetResource() const { return m_pTexture; };

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	int GetWidth() const {return m_Width;}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	int GetHeight() const {return m_Height;}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	DXGI_FORMAT GetFormat() const {return m_Format;}

private:
	g2dResourceCounterDX11::eResourceCategory m_Category;
	short m_Width, m_Height;
	DXGI_FORMAT m_Format;
	//DXGI_SAMPLE_DESC m_Multisample;
	int m_MipLevels;
	g2dD3D11TexturePtr m_pTexture;
	ID3D11ShaderResourceView* m_pShaderResourceView;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	float GetSize();
};
