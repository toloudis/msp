/****************************************************************************\
**  g2dDepthStencilBufferDX11.hpp
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_DEPTHSTENCILBUFFERDX11_HPP
#error g2dDepthStencilBufferDX11.hpp multiply included
#endif
#define G2D_DEPTHSTENCILBUFFERDX11_HPP

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

#ifndef G2D_RESOURCECOUNTERDX11_HPP
#include "GraphicsDX11/g2d/g2dResourceCounterDX11.hpp"
#endif

#ifndef G2D_DEPTHSTENCILBUFFER_HPP
#include "Graphics/G2d/g2dDepthStencilBuffer.hpp"
#endif

class g2dDepthStencilBufferDX11 : public g2dDepthStencilBuffer
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	g2dDepthStencilBufferDX11();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~g2dDepthStencilBufferDX11();

	//--------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions and
	//	pixel format.  If the pixel format is not supported it will
	//	throw a matUnsupportedPixelFormatX, and leave the old surface
	//	intact.  
	//--------------------------------------------------------------------
	void Make(int i_Width, int i_Height, DXGI_FORMAT i_Format, DXGI_SAMPLE_DESC i_Multisample,
		g2dResourceCounterDX11::eResourceCategory i_Category = g2dResourceCounterDX11::eRenderTarget);

	//------------------------------------------------------------------------
	// Access to the primary texture and it's views
	// DANGER! Do not release these pointers! No shenanigans please. I own them.
	//------------------------------------------------------------------------
	g2dD3D11TexturePtr GetDepthTexture() const { return m_pDepthTexture; };
	g2dD3D11DepthStencilPtr GetDepthView() const { return m_pDepthSurface; };
	g2dD3D11ShaderResourcePtr GetDepthShaderResource() const { return m_pDepthShaderResource; };

	//------------------------------------------------------------------------
	//will be 1 for non MSAA targets otherwise > 1
	//------------------------------------------------------------------------
	int GetMultisampleCount() const { return m_Multisample.Count; };

private:
	g2dResourceCounterDX11::eResourceCategory m_Category;
	short m_Width, m_Height;
	DXGI_FORMAT m_Format;
	DXGI_SAMPLE_DESC m_Multisample;
	g2dD3D11TexturePtr m_pDepthTexture;
	g2dD3D11DepthStencilPtr m_pDepthSurface;
	g2dD3D11ShaderResourcePtr m_pDepthShaderResource;	//to read from the depth in a shader

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	float GetSize();
};
