/****************************************************************************\
**  g2dTextureDX11.cpp
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/g2d/g2dTextureDX11.hpp"

#include "Graphics/g2d/g2dExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/private/g2dWindowDrawUtilDX11.hpp"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dTextureDX11::g2dTextureDX11()
:	m_Category(g2dResourceCounterDX11::eTexture),
	m_pTexture(NULL),
	m_pShaderResourceView(NULL),
	m_Width(0),
	m_Height(0),
	m_MipLevels(0),
	m_Format(DXGI_FORMAT_UNKNOWN)
{
//	m_Multisample.Count = 1;
//	m_Multisample.Quality = 0;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dTextureDX11::~g2dTextureDX11()
{
	g2dResourceCounterDX11::RemoveResource(GetSize(), m_Category);

	SAFE_RELEASE( m_pShaderResourceView );
//	SAFE_RELEASE( m_pTexture );
	ULONG ref_count = m_pTexture->Release();
	if( ref_count > 0 )
	{
		//g2dDX11Global::PrintDXError(op_result);
		DBG_ASSERT(ref_count > 0, "Error releasing surface, ref_count " << ref_count);
	}
}

//--------------------------------------------------------------------
//	Make makes the surface into one with the given dimensions and
//	pixel format.  If the pixel format is not supported it will
//	throw a matUnsupportedPixelFormatX, and leave the old surface
//	intact.  
//--------------------------------------------------------------------
void g2dTextureDX11::Make(int i_Width, int i_Height, DXGI_FORMAT i_Format, /*DXGI_SAMPLE_DESC i_Multisample,*/
									  g2dResourceCounterDX11::eResourceCategory i_Category/*= g2dResourceCounterDX11::eTexture*/)
{
	DBG_ASSERT(m_pTexture == NULL, "g2dTextureDX11::Make called while buffer already made");

	HRESULT hr;

	//------Create texture and view
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = i_Width;
	desc.Height = i_Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = i_Format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	//desc.SampleDesc = i_Multisample;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

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
	//m_Multisample = i_Multisample;
	m_MipLevels = 1;

	m_Category = i_Category;
	g2dResourceCounterDX11::AddResource(GetSize(), i_Category);
}

//------------------------------------------------------------------------
// This object will take ownership of the resource!
//------------------------------------------------------------------------
void g2dTextureDX11::Make(g2dD3D11TexturePtr i_TextureResource,
	g2dResourceCounterDX11::eResourceCategory i_Category /*= g2dResourceCounterDX11::eTexture*/)
{
	DBG_ASSERT(m_pTexture == NULL, "g2dTextureDX11::Make called while buffer already made");
	DBG_ASSERT(i_TextureResource != NULL, "g2dTextureDX11::Make called to wrap a NULL resource");

	HRESULT hr;

	m_pTexture = i_TextureResource;

	D3D11_TEXTURE2D_DESC desc;
	m_pTexture->GetDesc(&desc);

	//------Create view
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

	m_Format = desc.Format;
	m_Width = desc.Width;
	m_Height = desc.Height;
	//m_Multisample = desc.Multisample;
	m_MipLevels = desc.MipLevels;

	m_Category = i_Category;
	g2dResourceCounterDX11::AddResource(GetSize(), i_Category);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
float g2dTextureDX11::GetSize()
{
	int bpp = g2dDX11Global::BitsPerPixel(m_Format);
	float baseSize = (float)m_Width*(float)m_Height*(bpp/8) / 1024.0f;

	// approximation assuming all mip levels are present.
	if (m_MipLevels > 1)
		baseSize *= 1.333333f;
	
	return baseSize;
}
