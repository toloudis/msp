/****************************************************************************\
**  matRenderTargetTexture.cpp
**
**      matRenderTargetTexture is a matTexture which represents an ordinary
**	rectangular texture with no special ability.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"

#include "Core/fs/fsLocator.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/mat/matExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDepthStencilBufferDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dRenderTargetTextureDX11.hpp"
#include "GraphicsDX11/g2d/private/g2dWindowDrawUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/G3d/g3dDX11Util.hpp"

namespace 
{
	DXGI_FORMAT l_DepthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
};

//--------------------------------------------------------------------
//	This constructor makes an "empty" surface
//--------------------------------------------------------------------
matRenderTargetTexture::matRenderTargetTexture(bool i_bStoreDepths, bool i_bFloatDepth)
:	m_bStoreDepths(i_bStoreDepths), m_bFloatDepth( i_bFloatDepth ), 
	//m_pMainTextureD3D(NULL), m_pColorBuffer(NULL), 
	m_bHasMipMaps(false)
{
	m_bNoDepthRequested = false;
	m_bAA = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matRenderTargetTexture::~matRenderTargetTexture()
{
//	if( m_pColorBuffer )
//		m_pColorBuffer->Release();
}

//--------------------------------------------------------------------
//	ReloadInfo causes the matRenderTargetTexture to regenerate its
//	parameters from the PAC.  Again, this function should only be
//	called by the Terawatt PAC components.
//--------------------------------------------------------------------
void matRenderTargetTexture::ReloadInfo()
{
	this->SetWidth(m_RenderTarget->GetWidth());
	this->SetHeight(m_RenderTarget->GetHeight());
	g2dPFD pfd;
	g2dDX11Global::PFDFromD3DFormat(m_RenderTarget->GetFormat(), pfd);
	this->SetPixelFormat(pfd);
}

//--------------------------------------------------------------------
//	Make makes the surface into one with the given dimensions and
//	pixel format.  If the pixel format is not supported it will
//	throw a matUnsupportedPixelFormatX, and leave the old surface
//	intact.
//--------------------------------------------------------------------
void matRenderTargetTexture::Make(int i_Width, int i_Height, const g2dPFD& i_PFD,
								  bool i_bAllocDepthBuf /*=true*/, 
								  bool i_bAutoGenMipmap /*= false*/,
								  g2dResourceCounterDX11::eResourceCategory i_Category/*= g2dResourceCounterDX11::eRenderTarget*/,
								  bool i_bAntiAlias /*= false*/)
{
	if (g2dDX11Global::RestrictPow2Textures())
	{
		if (!((i_Width & (i_Width - 1)) == 0) || !((i_Height & (i_Height - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}


	// use floating point format for texture?
//#ifdef USE_VSM_SHADOWS
//	DXGI_FORMAT format = (m_bStoreDepths) ? DXGI_FORMAT_R32G32_FLOAT : g2dDX11Global::D3DFormatFromPFD(i_PFD);//D3DFMT_A8R8G8B8;
//#else
	DXGI_FORMAT format = (m_bStoreDepths) ? DXGI_FORMAT_R32_FLOAT : g2dDX11Global::D3DFormatFromPFD(i_PFD);//D3DFMT_A8R8G8B8;
//#endif

//	desc.MipLevels = (i_bAutoGenMipmap) ? 0 : 1;
//	desc.MiscFlags = (i_bAutoGenMipmap) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;


	DXGI_SAMPLE_DESC SDesc = g2dDX11Global::DefaultSampleDesc();
	if( i_bAntiAlias )
	{
		g2dDX11Global::ChooseMultisampleQuality( format, SDesc.Count, SDesc.Quality );
	}

	m_RenderTarget.reset(new g2dRenderTargetTextureDX11());
	m_RenderTarget->Make(i_Width,i_Height,
		format,
		SDesc,
		i_Category,
		i_bAutoGenMipmap
	);

	m_bHasMipMaps = i_bAutoGenMipmap;
	m_bAA = i_bAntiAlias;

	// Set up depth buffer
	if (i_bAllocDepthBuf /*|| m_bStoreDepths*/)
	{
		m_DepthStencil.reset(new g2dDepthStencilBufferDX11());
		m_DepthStencil->Make(i_Width,i_Height,
			m_bFloatDepth ? DXGI_FORMAT_D32_FLOAT : l_DepthFormat,
			SDesc,
			i_Category
		);
	}

//	this->SetSurface(m_pMainTextureD3D);

	if (i_bAllocDepthBuf == false)
		m_bNoDepthRequested = true;

	// sync up new size and pixel format info
	ReloadInfo();

//	DBG_LOG4("render target texture created %05dx%05d %d buffers, size %d", 
//		this->GetWidth(), this->GetHeight(), (depth_buffer==NULL)?1:2, this->GetSize());
}

//------------------------------------------------------------------------
//	Resolve will copy the contents to the given texture, resolving 
//	the multisample surface to a single sampled one.
//------------------------------------------------------------------------
void matRenderTargetTexture::Resolve(matRenderTargetTexture* o_pTexture)
{
	g2dD3D11TexturePtr srcTex = m_RenderTarget->GetResource();
	D3D11_TEXTURE2D_DESC SrcDesc;
	srcTex->GetDesc( &SrcDesc );

	g2dD3D11TexturePtr destTex = o_pTexture->GetTextureSurface();

#ifdef _DEBUG
	D3D11_TEXTURE2D_DESC DestDesc;
	destTex->GetDesc( &DestDesc );
	DBG_ASSERT( m_bAA && (SrcDesc.Width == DestDesc.Width) &&
		(SrcDesc.Height == DestDesc.Height) &&
		(SrcDesc.Format == DestDesc.Format), "Texture parameters don't match for Resolve!" );
#endif

	g2dDX11Global::g_pDeviceContext->ResolveSubresource( destTex, 0, srcTex, 0, SrcDesc.Format );
}


//------------------------------------------------------------------------
//	GetPixelFormat returns the current pixel format of the render target.
//------------------------------------------------------------------------
const g2dPFD& matRenderTargetTexture::GetPixelFormat() const
{
	return matTexture::GetPixelFormat();
}

//------------------------------------------------------------------------
//	GetDimensions returns the width and height of the render target.
//------------------------------------------------------------------------
void matRenderTargetTexture::GetDimensions(int& o_Width, int& o_Height) const
{
	o_Width = GetWidth();
	o_Height = GetHeight();
}


//--------------------------------------------------------------------
// Return an object pointer to use for rendering to this texture.
//	Only certain texture types can return this object, most will
//	return NULL.  The object pointed to will be owned by this 
//	texture, it should not be deleted by the user.
//--------------------------------------------------------------------
//virtual 
g2dRenderTarget* matRenderTargetTexture::GetRenderTargetAPI()
{
	// In this case we are using multiple inheritance, but we could 
	// also have used containment and made the render target a member
	// of this class.
	return this;
}


//------------------------------------------------------------------------
// BeginScene must be called before rendering to this window
//------------------------------------------------------------------------
//virtual 
void matRenderTargetTexture::BeginScene()
{
	// direct device to output to our window
	configure_device();
	g2dDX11Global::BeginScene();
}

//------------------------------------------------------------------------
// similar to BeginScene, call this to make this the current target of all
// subsequent rendering calls.  Need not be followed by EndScene.
//------------------------------------------------------------------------
void matRenderTargetTexture::MakeCurrent()
{
	// direct device to output to our window
	configure_device();
}

//------------------------------------------------------------------------
//	EndScene must be called when you are done with the drawing operations
//	on the current frame.  It will cause whatever drawing you have
//	requested to be visible on the screen.
//------------------------------------------------------------------------
//virtual 
void matRenderTargetTexture::EndScene()
{
	// nothing needed for render target textures?
	g2dDX11Global::EndScene();
}

//------------------------------------------------------------------------
//	Clear fills the screen with the given color.
//------------------------------------------------------------------------
//virtual 
void matRenderTargetTexture::Clear(const g2dRGBColor& i_Color)
{
	configure_device();
	g2dWindowDrawUtilDX11::Clear(i_Color);

}

//------------------------------------------------------------------------
//	Clear fills the screen with the given color and sets depth and stencil
//------------------------------------------------------------------------
//virtual 
void matRenderTargetTexture::Clear( const maFloatRGBA& i_Color, bool i_ClearDepth /*= false*/, 
								   float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
{
	// don't need to set render target to clear.
//	configure_device();

	float c[4];
	c[0] = i_Color.m_Red;
	c[1] = i_Color.m_Green;
	c[2] = i_Color.m_Blue;
	c[3] = i_Color.m_Alpha;
	g2dDX11Global::g_pDeviceContext->ClearRenderTargetView(this->m_RenderTarget->GetRenderTargetView(), c);

	// temp: does it work to clear here?
	if (i_ClearDepth && m_DepthStencil)
	{
		g2dDX11Global::g_pDeviceContext->ClearDepthStencilView(this->m_DepthStencil->GetDepthView(), 
			D3D11_CLEAR_DEPTH | (i_ClearStencil ? D3D11_CLEAR_STENCIL : 0), i_Depth, i_Stencil);
	}
}

//------------------------------------------------------------------------
//	Clear fills the depthstencil buffer with the given values.
//	If there is no depthstencil, then this does nothing.
//------------------------------------------------------------------------
//virtual 
void matRenderTargetTexture::ClearDepthStencil(float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
{
	if (this->m_DepthStencil)
	{
		configure_device();
		g2dDX11Global::g_pDeviceContext->ClearDepthStencilView(m_DepthStencil->GetDepthView(), 
			i_ClearStencil ? D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL : D3D11_CLEAR_DEPTH,
			i_Depth, i_Stencil);
	}
}

//------------------------------------------------------------------------
// set up device to render to our window
//------------------------------------------------------------------------
void matRenderTargetTexture::configure_device()
{
	DBG_ASSERT(m_RenderTarget && (m_DepthStencil || m_bNoDepthRequested), 
		"Need to call Make() before rendering to texture");
	if (m_DepthStencil)
		g2dDX11Global::SetRenderTargets(m_RenderTarget->GetRenderTargetView(), m_DepthStencil->GetDepthView());
	else
		g2dDX11Global::SetColorTarget(m_RenderTarget->GetRenderTargetView());

	D3D11_VIEWPORT vprt;
	vprt.TopLeftX = vprt.TopLeftY = 0;
	vprt.Width = (float)this->GetWidth();
	vprt.Height = (float)this->GetHeight();
	vprt.MinDepth = 0;
	vprt.MaxDepth = 1;
	g2dDX11Global::g_pDeviceContext->RSSetViewports(1, &vprt);
}

//----------------------------------------------------------------------------
//	GetSize returns approximate amount of memory (in kbytes) being
// used by this texture
//----------------------------------------------------------------------------
float matRenderTargetTexture::GetSize() const
{
	// both resource objects are memory managed at a lower level.
	return 0;

	// get size of main surface (could be color or depth buffer)
	float mainSize = matTexture::GetSize();
	// now figure out which of the 2 surfaces might be mipmapped.
	if (m_bHasMipMaps)
	{
		mainSize = (mainSize * 4) / 3;
	}

	// Don't add the size of a depthbuffer because it automatically adds itself
	// when created.

	// For the internal memory counter, this is correct, but if we really want to
	// know the total burden of this object, then we might want to add it in,
	// even if it is shared.

	return mainSize;
}

//------------------------------------------------------------------------
// call this to make this depth buffer the current depth buffer for all
// subsequent rendering calls.  
//------------------------------------------------------------------------
void matRenderTargetTexture::MakeDepthCurrent()
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
//------------------------------------------------------------------------
ID3D11DepthStencilView* matRenderTargetTexture::GetDepthBuffer() const 
{
	return m_DepthStencil ? m_DepthStencil->GetDepthView() : NULL; 
};
ID3D11RenderTargetView* matRenderTargetTexture::GetColorBuffer() const
{
	return m_RenderTarget ? m_RenderTarget->GetRenderTargetView() : NULL; 
}
//--------------------------------------------------------------------
//	GetTextureSurface returns the main D3D texture pointer.
//--------------------------------------------------------------------
g2dD3D11TexturePtr matRenderTargetTexture::GetTextureSurface()
{
	return m_RenderTarget ? m_RenderTarget->GetResource() : NULL; 
}

//--------------------------------------------------------------------
//	GetSurface returns a pointer usable for shaders.
//--------------------------------------------------------------------
ID3D11ShaderResourceView* matRenderTargetTexture::GetSurface() const
{
	return m_RenderTarget ? m_RenderTarget->GetShaderResourceView() : NULL; 
}
g2dD3D11ResourcePtr matRenderTargetTexture::GetResource() const
{
	return m_RenderTarget ? m_RenderTarget->GetResource() : NULL; 
}

bool matRenderTargetTexture::CopyWithResolve( matRenderTargetTexture* o_pTexture, bool bNoResolve /*= false */, bool bResolveDepth /*= false*/ )
{
	bool rVal = true;

	if( this == o_pTexture ) DBG_TRACE( "CopyWithResolve cannot copy same target!" );

	//direct copy if matching dimensions and format
	int iW, iH;
	int oW, oH;

	GetDimensions( iW, iH );
	o_pTexture->GetDimensions( oW, oH );

	//resolve if downsampling is required
	if( !bNoResolve && IsAA() && !o_pTexture->IsAA() )
	{
		Resolve( o_pTexture );
	}
	else if( (iH == oH) && GetPixelFormat().GetPixelFormat() == o_pTexture->GetPixelFormat().GetPixelFormat() )
	{
		g2dDX11Global::g_pDeviceContext->CopySubresourceRegion( o_pTexture->GetTextureSurface(), 0,
			0, 0, 0, GetTextureSurface(), 0, NULL);
	}
	else if( !IsAA() )	//resort to a scaled copy if source is not MSAA
	{
		g3dDX11Util::CopyTexToTarget( this, o_pTexture );
	}
	else
	{
		rVal = false;
	}

	//Resolve/Copy only if different
	if( bResolveDepth && GetDepthStencilBuffer() != o_pTexture->GetDepthStencilBuffer() )
	{
		//Make sure both have depths and that target is non MSAA
		if( GetHasDepthBuffer() && o_pTexture->GetHasDepthBuffer() && !o_pTexture->IsAA() )
		{
			g3dDX11Util::CopyDepth( this, o_pTexture );
		}
		else rVal = false;	//can't resolve/copy if requested
	}

	return rVal;
}

shared_ptr<g2dDepthStencilBuffer> matRenderTargetTexture::GetDepthStencilBuffer() const
{
	return m_DepthStencil;
}

void matRenderTargetTexture::SetDepthBuffer( shared_ptr<g2dDepthStencilBuffer> i_DepthStencil )
{
	m_DepthStencil = boost::dynamic_pointer_cast<g2dDepthStencilBufferDX11>(i_DepthStencil);
}
//--------------------------------------------------------------------
//	SetSurface sets the DirectDraw surface pointer
//--------------------------------------------------------------------
//void matRenderTargetTexture::SetSurface(g2dD3D11ResourcePtr i_Surface)
//{
//	DBG_ERROR("SetSurface shoud not be called on matRenderTargetTexture");
//}
