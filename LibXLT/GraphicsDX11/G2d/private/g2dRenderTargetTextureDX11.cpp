/****************************************************************************\
**  g2dRenderTargetTextureDX11.cpp
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/g2d/g2dRenderTargetTextureDX11.hpp"

#include "Graphics/g2d/g2dExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/private/g2dWindowDrawUtilDX11.hpp"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dRenderTargetTextureDX11::g2dRenderTargetTextureDX11()
:	m_Category(g2dResourceCounterDX11::eRenderTarget),
	m_pTexture(NULL),
	m_pRenderTargetView(NULL),
	m_pShaderResourceView(NULL),
	m_Width(0),
	m_Height(0),
	m_Mips(0),
	m_Format(DXGI_FORMAT_UNKNOWN)
{
	m_Multisample.Count = 1;
	m_Multisample.Quality = 0;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dRenderTargetTextureDX11::~g2dRenderTargetTextureDX11()
{
	g2dResourceCounterDX11::RemoveResource(GetSize(), m_Category);

	SAFE_RELEASE( m_pShaderResourceView );
	SAFE_RELEASE( m_pRenderTargetView );
	SAFE_RELEASE( m_pTexture );
}

//--------------------------------------------------------------------
//	Make makes the surface into one with the given dimensions and
//	pixel format.  If the pixel format is not supported it will
//	throw a matUnsupportedPixelFormatX, and leave the old surface
//	intact.  
//--------------------------------------------------------------------
void g2dRenderTargetTextureDX11::Make(int i_Width, int i_Height, DXGI_FORMAT i_Format, DXGI_SAMPLE_DESC i_Multisample,
									  g2dResourceCounterDX11::eResourceCategory i_Category/*= g2dResourceCounterDX11::eRenderTarget*/,
									  bool i_bGenerateMips /*= false*/)
{
	DBG_ASSERT(m_pTexture == NULL, "g2dRenderTargetTextureDX11::Make called while buffer already made");

	HRESULT hr;

	//------Create texture and view
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = i_Width;
	desc.Height = i_Height;
	desc.MipLevels = i_bGenerateMips ? 0 : 1;
	desc.ArraySize = 1;
	desc.Format = i_Format;
	desc.SampleDesc = i_Multisample;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = i_bGenerateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	hr = g2dDX11Global::g_pDevice->CreateTexture2D( &desc, NULL, &m_pTexture );

	if( !SUCCEEDED(hr) )
	{
		m_pTexture = NULL;
		g2dDX11Global::PrintDXError(hr);
		if (hr == E_OUTOFMEMORY)
		{
			throw g2dOutOfSystemMemoryX();
		}
		else if ( hr == D3DERR_INVALIDCALL )
		{
			throw g2dGeneralX();
		}
		DBG_ASSERT(SUCCEEDED(hr), "Error creating render target depth texture");
	}

	hr = g2dDX11Global::g_pDevice->CreateRenderTargetView( m_pTexture, NULL, &m_pRenderTargetView );
	if( !SUCCEEDED(hr) )
	{
		m_pRenderTargetView = NULL;
		g2dDX11Global::PrintDXError(hr);
		if (hr == E_OUTOFMEMORY)
		{
			throw g2dOutOfSystemMemoryX();
		}
		else if ( hr == D3DERR_INVALIDCALL )
		{
			throw g2dGeneralX();
		}
		DBG_ASSERT(SUCCEEDED(hr), "Error creating render target depth surface");
	}
	hr = g2dDX11Global::g_pDevice->CreateShaderResourceView( m_pTexture, NULL, &m_pShaderResourceView );
	if( !SUCCEEDED(hr) )
	{
		m_pShaderResourceView = NULL;
		g2dDX11Global::PrintDXError(hr);
		if (hr == E_OUTOFMEMORY)
		{
			throw g2dOutOfSystemMemoryX();
		}
		else if ( hr == D3DERR_INVALIDCALL )
		{
			throw g2dGeneralX();
		}
		DBG_ASSERT(SUCCEEDED(hr), "Error creating render target depth surface");
	}

	// success... set local vars

	m_Format = i_Format;
	m_Width = i_Width;
	m_Height = i_Height;
	m_Multisample = i_Multisample;
	m_Mips = i_bGenerateMips ? 0 : 1;

	m_Category = i_Category;
	g2dResourceCounterDX11::AddResource(GetSize(), i_Category);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
float g2dRenderTargetTextureDX11::GetSize()
{
	int bpp = g2dDX11Global::BitsPerPixel(m_Format);
	float size = (float)m_Width*(float)m_Height*(bpp/8) / 1024.0f;
	return m_Mips ? size : (size * 3.0f/2.0f);	//0 is full mip chain
}
