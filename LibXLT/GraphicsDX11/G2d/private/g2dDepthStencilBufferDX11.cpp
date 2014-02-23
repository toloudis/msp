/****************************************************************************\
**  g2dDepthStencilBufferDX11.cpp
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/g2d/g2dDepthStencilBufferDX11.hpp"

#include "Graphics/g2d/g2dExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/private/g2dWindowDrawUtilDX11.hpp"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dDepthStencilBufferDX11::g2dDepthStencilBufferDX11()
:	m_Category(g2dResourceCounterDX11::eRenderTarget),
	m_pDepthTexture(NULL),
	m_pDepthSurface (NULL),
	m_pDepthShaderResource(NULL),
	m_Width(0),
	m_Height(0),
	m_Format(DXGI_FORMAT_UNKNOWN)
{
	m_Multisample.Count = 1;
	m_Multisample.Quality = 0;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dDepthStencilBufferDX11::~g2dDepthStencilBufferDX11()
{
	g2dResourceCounterDX11::RemoveResource(GetSize(), m_Category);

	SAFE_RELEASE( m_pDepthSurface );
	SAFE_RELEASE( m_pDepthTexture );
	SAFE_RELEASE( m_pDepthShaderResource );
}

//--------------------------------------------------------------------
//	Make makes the surface into one with the given dimensions and
//	pixel format.  If the pixel format is not supported it will
//	throw a matUnsupportedPixelFormatX, and leave the old surface
//	intact.  
//--------------------------------------------------------------------
void g2dDepthStencilBufferDX11::Make(int i_Width, int i_Height, DXGI_FORMAT i_Format, DXGI_SAMPLE_DESC i_Multisample,
									 g2dResourceCounterDX11::eResourceCategory i_Category/*= g2dResourceCounterDX11::eRenderTarget*/)
{
	DBG_ASSERT(m_pDepthTexture == NULL, "g2dDepthStencilBufferDX11::Make called while buffer already made");

	HRESULT hr;

	//Primary texture is typeless so that we can write in one format and read in another
	DXGI_FORMAT depthFormat = DXGI_FORMAT_R24G8_TYPELESS;

	//------Create Depth/Stencil texture and view
	D3D11_TEXTURE2D_DESC DepthDesc;
	DepthDesc.Width = i_Width;
	DepthDesc.Height = i_Height;
	DepthDesc.MipLevels = 1;
	DepthDesc.ArraySize = 1;
	DepthDesc.Format = depthFormat;
	DepthDesc.SampleDesc = i_Multisample;
	DepthDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	DepthDesc.CPUAccessFlags = 0;
	DepthDesc.MiscFlags = 0;

	hr = g2dDX11Global::g_pDevice->CreateTexture2D( &DepthDesc, NULL, &m_pDepthTexture );

	if( !SUCCEEDED(hr) )
	{
		m_pDepthTexture = NULL;
		g2dDX11Global::PrintDXError(hr);
		if (hr == E_OUTOFMEMORY)
		{
			throw g2dOutOfSystemMemoryX();
		}
		else //if ( hr == D3DERR_INVALIDCALL )
		{
			throw g2dGeneralX();
		}
		DBG_ASSERT(SUCCEEDED(hr), "Error creating render target depth texture");
	}

	//create the depth stencil view with a 24 bit depth and 8 bit stencil UINT
	//Create as multisample if we have multiple sample count
	hr = g2dDX11Global::g_pDevice->CreateDepthStencilView( m_pDepthTexture,
		&CD3D11_DEPTH_STENCIL_VIEW_DESC( i_Multisample.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D24_UNORM_S8_UINT ),
		&m_pDepthSurface );
	if( !SUCCEEDED(hr) )
	{
		m_pDepthSurface = NULL;
		g2dDX11Global::PrintDXError(hr);
		if (hr == E_OUTOFMEMORY)
		{
			throw g2dOutOfSystemMemoryX();
		}
		else// if ( hr == D3DERR_INVALIDCALL )
		{
			throw g2dGeneralX();
		}
		DBG_ASSERT(SUCCEEDED(hr), "Error creating render target depth surface");
	}

	//create a shader resource view with 24bits of Red and ignore the stencil
	hr = g2dDX11Global::g_pDevice->CreateShaderResourceView( m_pDepthTexture,
		&CD3D11_SHADER_RESOURCE_VIEW_DESC( i_Multisample.Count > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R24_UNORM_X8_TYPELESS),
		&m_pDepthShaderResource );
	if( !SUCCEEDED(hr) )
	{
		m_pDepthShaderResource = NULL;
		g2dDX11Global::PrintDXError(hr);
		if (hr == E_OUTOFMEMORY)
		{
			throw g2dOutOfSystemMemoryX();
		}
		else// if ( hr == D3DERR_INVALIDCALL )
		{
			throw g2dGeneralX();
		}
		DBG_ASSERT(SUCCEEDED(hr), "Error creating render target depth shader resource");
	}

	// success... set local vars
	m_Format = i_Format;
	m_Width = i_Width;
	m_Height = i_Height;
	m_Multisample = i_Multisample;

	m_Category = i_Category;
	g2dResourceCounterDX11::AddResource(GetSize(), i_Category);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
float g2dDepthStencilBufferDX11::GetSize()
{
	// assume 32bit depth buffer is always here.
	int bpp = 32;
	return (float)m_Width*(float)m_Height*(bpp/8) / 1024.0f;
}
