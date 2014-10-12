/****************************************************************************\
**  g2dRenderTargetDX11.cpp
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/g2d/g2dRenderTargetDX11.hpp"

#include "Graphics/g2d/g2dExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDepthStencilBufferDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/private/g2dWindowDrawUtilDX11.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"


//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dRenderTargetDX11::g2dRenderTargetDX11()
:	m_pTargetTexture(NULL),
	m_pTargetSurface (NULL),
	m_Width(0),
	m_Height(0)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dRenderTargetDX11::~g2dRenderTargetDX11()
{
	g2dResourceCounterDX11::RemoveResource(GetSize(), g2dResourceCounterDX11::eRenderTarget);

	SAFE_RELEASE( m_pTargetSurface );
	SAFE_RELEASE( m_pTargetTexture );
}

//------------------------------------------------------------------------
// BeginScene must be called before rendering to this window
//------------------------------------------------------------------------
void g2dRenderTargetDX11::BeginScene()
{
	// direct device to output to our window
//	configure_device();
//	g2dDX11Global::BeginScene();
}

//------------------------------------------------------------------------
// similar to BeginScene, call this to make this the current target of all
// subsequent rendering calls.  Need not be followed by EndScene.
//------------------------------------------------------------------------
void g2dRenderTargetDX11::MakeCurrent()
{
	// direct device to output to our window
	configure_device();
}

//------------------------------------------------------------------------
// call this to make this depth buffer the current depth buffer for all
// subsequent rendering calls.  
//------------------------------------------------------------------------
void g2dRenderTargetDX11::MakeDepthCurrent()
{
	if (m_DepthStencil)
	{
		g2dDX11Global::SetDepthTarget(m_DepthStencil->GetDepthView());
	}
	else
	{
		DBG_TRACE("MakeDepthCurrent called with no depth buffer.");
	}
}

//------------------------------------------------------------------------
//	EndScene must be called when you are done with the drawing operations
//	on the current frame.  It will cause whatever drawing you have
//	requested to be visible on the screen.
//------------------------------------------------------------------------
void g2dRenderTargetDX11::EndScene()
{
	// nothing needed for simple render targets?
//	g2dDX11Global::EndScene();
}

//------------------------------------------------------------------------
//	Clear fills the screen with the given color.
//------------------------------------------------------------------------
void g2dRenderTargetDX11::Clear(const g2dRGBColor& i_Color)
{
	configure_device();
	g2dWindowDrawUtilDX11::Clear(i_Color);
}

//------------------------------------------------------------------------
//	Clear fills the screen with the given color and sets depth and stencil
//------------------------------------------------------------------------
void g2dRenderTargetDX11::Clear( const maFloatRGBA& i_Color, bool i_ClearDepth /*=false*/, float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
{
	configure_device();
	g2dWindowDrawUtilDX11::Clear(i_Color, i_ClearDepth && m_DepthStencil, i_Depth, i_ClearStencil, i_Stencil);
}


//------------------------------------------------------------------------
//	Clear fills the depthstencil buffer with the given values.
//	If there is no depthstencil, then this does nothing.
//------------------------------------------------------------------------
//virtual 
void g2dRenderTargetDX11::ClearDepthStencil(float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
{
	if (m_DepthStencil)
	{
		configure_device();
		g2dDX11Global::g_pDeviceContext->ClearDepthStencilView(this->m_DepthStencil->GetDepthView(), 
			i_ClearStencil ? D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL : D3D11_CLEAR_DEPTH, 
			i_Depth, i_Stencil);
	}
}

//------------------------------------------------------------------------
//	GetPixelFormat returns the current pixel format of the render target.
//------------------------------------------------------------------------
const g2dPFD& g2dRenderTargetDX11::GetPixelFormat() const
{
	return m_PFD;
}

//------------------------------------------------------------------------
//	GetDimensions returns the width and height of the render target.
//------------------------------------------------------------------------
void g2dRenderTargetDX11::GetDimensions(int& o_Width, int& o_Height) const
{
	o_Width = m_Width;
	o_Height = m_Height;
}

//--------------------------------------------------------------------
//	Make makes the surface into one with the given dimensions and
//	pixel format.  If the pixel format is not supported it will
//	throw a matUnsupportedPixelFormatX, and leave the old surface
//	intact.  
//--------------------------------------------------------------------
void g2dRenderTargetDX11::Make(int i_Width, int i_Height, const g2dPFD& i_PFD, bool i_AntiAlias /*= false*/)
{
	HRESULT hr;

	DXGI_FORMAT format = g2dDX11Global::D3DFormatFromPFD(i_PFD);
	DXGI_FORMAT depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	UINT multiSampleCount = 1;
	UINT multisampleQuality = 0;

	if (i_AntiAlias)
	{
		// find maximum multisample level.
		multiSampleCount = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;//D3DMULTISAMPLE_NONMASKABLE;//;
		do
		{
			g2dDX11Global::g_pDevice->CheckMultisampleQualityLevels(
				format,	multiSampleCount, &multisampleQuality );
			if( multisampleQuality != 0 )
			{
				g2dDX11Global::g_pDevice->CheckMultisampleQualityLevels(
					depthFormat, multiSampleCount, &multisampleQuality );
				break;	//found a match
			}
			else
			{
				multiSampleCount >>= 1;
			}
		} 
		while( multiSampleCount >= 1 );

		multisampleQuality--;

		if( !multiSampleCount )
		{
			DBG_WARNING("Multisampled rendertarget requested but multisampling not supported.");
		}
	}

	// create multisampled surfaces.

	//------Create Target texture and view
	D3D11_TEXTURE2D_DESC TargetDesc;
	TargetDesc.Width = i_Width;
	TargetDesc.Height = i_Height;
	TargetDesc.MipLevels = 1;
	TargetDesc.ArraySize = 1;
	TargetDesc.Format = format;
	TargetDesc.SampleDesc.Count = multiSampleCount;
	TargetDesc.SampleDesc.Quality = multisampleQuality;
	TargetDesc.Usage = D3D11_USAGE_DEFAULT;
	TargetDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	TargetDesc.CPUAccessFlags = 0;
	TargetDesc.MiscFlags = 0;

	hr = g2dDX11Global::g_pDevice->CreateTexture2D( &TargetDesc, NULL, &m_pTargetTexture );

	if( !SUCCEEDED(hr) )
	{
		m_pTargetTexture = NULL;
		g2dDX11Global::PrintDXError(hr);
		if (hr == E_OUTOFMEMORY)
		{
			throw g2dOutOfSystemMemoryX();
		}
		else if ( hr == D3DERR_INVALIDCALL )
		{
			throw g2dGeneralX();
		}
		DBG_ASSERT(SUCCEEDED(hr), "Error creating render target main texture");
	}

	hr = g2dDX11Global::g_pDevice->CreateRenderTargetView( m_pTargetTexture, NULL, &m_pTargetSurface );
	if( !SUCCEEDED(hr) )
	{
		m_pTargetSurface = NULL;
		g2dDX11Global::PrintDXError(hr);
		if (hr == E_OUTOFMEMORY)
		{
			throw g2dOutOfSystemMemoryX();
		}
		else if ( hr == D3DERR_INVALIDCALL )
		{
			throw g2dGeneralX();
		}
		DBG_ASSERT(SUCCEEDED(hr), "Error creating render target main surface");
	}

	//------Create Depth/Stencil texture and view
	m_DepthStencil.reset(new g2dDepthStencilBufferDX11());
	m_DepthStencil->Make(i_Width, i_Height, depthFormat, TargetDesc.SampleDesc);

	// success... set local vars
	m_PFD = i_PFD;
	m_Width = i_Width;
	m_Height = i_Height;

	g2dResourceCounterDX11::AddResource(GetSize(), g2dResourceCounterDX11::eRenderTarget);

}

//------------------------------------------------------------------------
//	Resolve will copy the contents to the given texture, resolving 
//	the multisample surface to a single sampled one.
//------------------------------------------------------------------------
void g2dRenderTargetDX11::Resolve(matRenderTargetTexture* o_pTexture)
{
	D3D11_TEXTURE2D_DESC SrcDesc;
	m_pTargetTexture->GetDesc( &SrcDesc );

	g2dD3D11TexturePtr destTex = o_pTexture->GetTextureSurface();

#ifdef _DEBUG
	D3D11_TEXTURE2D_DESC DestDesc;
	destTex->GetDesc( &DestDesc );
	DBG_ASSERT( (SrcDesc.Width == DestDesc.Width) &&
		        (SrcDesc.Height == DestDesc.Height) &&
				(SrcDesc.Format == DestDesc.Format), "Texture parameters don't match for Resolve!" );
#endif

	g2dDX11Global::g_pDeviceContext->ResolveSubresource( destTex, 0, m_pTargetTexture, 0, SrcDesc.Format );
}

//------------------------------------------------------------------------
// set up device to render to our window
//------------------------------------------------------------------------
void g2dRenderTargetDX11::configure_device()
{
	DBG_ASSERT(m_pTargetSurface && m_DepthStencil, 
		"Need to call Make() before rendering to target");
	g2dDX11Global::SetRenderTargets(m_pTargetSurface, m_DepthStencil->GetDepthView());

	D3D11_VIEWPORT vprt;
	vprt.TopLeftX = vprt.TopLeftY = 0;
	vprt.Width = (float)this->m_Width;
	vprt.Height = (float)this->m_Height;
	vprt.MinDepth = 0;
	vprt.MaxDepth = 1;
	g2dDX11Global::g_pDeviceContext->RSSetViewports(1, &vprt);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
float g2dRenderTargetDX11::GetSize()
{
	// assume 32bit depth buffer is always here.
	return m_Width*m_Height*(m_PFD.BitsPerPixel()/8 + 32/8) / 1024.0f;
}

//------------------------------------------------------------------------
//	Returns true if this object has allocated a depth buffer.
//------------------------------------------------------------------------
bool g2dRenderTargetDX11::GetHasDepthBuffer() const
{
	return m_DepthStencil != NULL;
}

