/****************************************************************************\
**  matShadowMap.cpp
**
**      matShadowMap is a matTexture which represents an ordinary
**	rectangular texture with no special ability.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/mat/matShadowMap.hpp"

#include "Core/fs/fsLocator.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/mat/matExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDepthStencilBufferDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dRenderTargetTextureDX11.hpp"
#include "GraphicsDX11/g2d/private/g2dWindowDrawUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"

namespace 
{
	DXGI_FORMAT l_DepthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
};

//--------------------------------------------------------------------
//	This constructor makes an "empty" surface
//--------------------------------------------------------------------
matShadowMap::matShadowMap()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matShadowMap::~matShadowMap()
{
}

//--------------------------------------------------------------------
//	ReloadInfo causes the matShadowMap to regenerate its
//	parameters from the PAC.  Again, this function should only be
//	called by the Terawatt PAC components.
//--------------------------------------------------------------------
void matShadowMap::ReloadInfo()
{
	D3D11_TEXTURE2D_DESC desc;
	m_MainTexture->GetResource()->GetDesc(&desc);

	this->SetWidth(desc.Width);
	this->SetHeight(desc.Height);
	g2dPFD pfd;
	g2dDX11Global::PFDFromD3DFormat(desc.Format, pfd);
	this->SetPixelFormat(pfd);
}

//--------------------------------------------------------------------
//	Make makes the surface into one with the given dimensions and
//	pixel format.  If the pixel format is not supported it will
//	throw a matUnsupportedPixelFormatX, and leave the old surface
//	intact.
//--------------------------------------------------------------------
void matShadowMap::Make(int i_Width, int i_Height)
{
	if (g2dDX11Global::RestrictPow2Textures())
	{
		if (!((i_Width & (i_Width - 1)) == 0) || !((i_Height & (i_Height - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	// use floating point format for texture?
#ifdef USE_VSM_SHADOWS
	DXGI_FORMAT format = DXGI_FORMAT_R32G32_FLOAT;
#else
	DXGI_FORMAT format = DXGI_FORMAT_R32_FLOAT;
#endif

	m_MainTexture.reset(new g2dRenderTargetTextureDX11());
	m_MainTexture->Make(i_Width,i_Height,
		format,
		g2dDX11Global::DefaultSampleDesc(),
		g2dResourceCounterDX11::eShadow
	);

	// Set up depth buffer
	m_DepthStencil.reset(new g2dDepthStencilBufferDX11());
	m_DepthStencil->Make(i_Width,i_Height,
		l_DepthFormat,
		g2dDX11Global::DefaultSampleDesc(),
		g2dResourceCounterDX11::eShadow
	);

	// sync up new size and pixel format info
	ReloadInfo();

//	DBG_LOG4("render target texture created %05dx%05d %d buffers, size %d", 
//		this->GetWidth(), this->GetHeight(), (depth_buffer==NULL)?1:2, this->GetSize());
}

//------------------------------------------------------------------------
//	GetPixelFormat returns the current pixel format of the render target.
//------------------------------------------------------------------------
const g2dPFD& matShadowMap::GetPixelFormat() const
{
	return matTexture::GetPixelFormat();
}

//------------------------------------------------------------------------
//	GetDimensions returns the width and height of the render target.
//------------------------------------------------------------------------
void matShadowMap::GetDimensions(int& o_Width, int& o_Height) const
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
g2dRenderTarget* matShadowMap::GetRenderTargetAPI()
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
void matShadowMap::BeginScene()
{
	// direct device to output to our window
	configure_device();
	g2dDX11Global::BeginScene();
}

//------------------------------------------------------------------------
// similar to BeginScene, call this to make this the current target of all
// subsequent rendering calls.  Need not be followed by EndScene.
//------------------------------------------------------------------------
void matShadowMap::MakeCurrent()
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
void matShadowMap::EndScene()
{
	// nothing needed for render target textures?
	g2dDX11Global::EndScene();
}

//------------------------------------------------------------------------
//	Clear fills the screen with the given color.
//------------------------------------------------------------------------
//virtual 
void matShadowMap::Clear(const g2dRGBColor& i_Color)
{
	configure_device();
	g2dWindowDrawUtilDX11::Clear(i_Color);

}

//------------------------------------------------------------------------
//	Clear fills the screen with the given color and sets depth and stencil
//------------------------------------------------------------------------
//virtual 
void matShadowMap::Clear( const maFloatRGBA& i_Color, bool i_ClearDepth /*= false*/, 
								   float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
{
	// don't need to set render target to clear.
//	configure_device();

	float c[4];
	c[0] = i_Color.m_Red;
	c[1] = i_Color.m_Green;
	c[2] = i_Color.m_Blue;
	c[3] = i_Color.m_Alpha;
	g2dDX11Global::g_pDeviceContext->ClearRenderTargetView(this->m_MainTexture->GetRenderTargetView(), c);

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
void matShadowMap::ClearDepthStencil(float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
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
void matShadowMap::configure_device()
{
	DBG_ASSERT(m_MainTexture && m_DepthStencil, 
		"Need to call Make() before rendering to texture");
	if (m_DepthStencil)
		g2dDX11Global::SetRenderTargets(m_MainTexture->GetRenderTargetView(), m_DepthStencil->GetDepthView());
	else
		g2dDX11Global::SetColorTarget(m_MainTexture->GetRenderTargetView());

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
float matShadowMap::GetSize() const
{
	// get size of main surface (could be color or depth buffer)
	float mainSize = matTexture::GetSize();
	
	// now add in size of 2nd buffer (depth buffer or the R32 color buffer)
	float otherSize = 0;

	return mainSize + otherSize;

}

//------------------------------------------------------------------------
// call this to make this depth buffer the current depth buffer for all
// subsequent rendering calls.  
//------------------------------------------------------------------------
void matShadowMap::MakeDepthCurrent()
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
//	Returns true if this object has allocated a depth buffer.
//------------------------------------------------------------------------
bool matShadowMap::GetHasDepthBuffer() const 
{
	return m_DepthStencil != NULL;
}

//--------------------------------------------------------------------
//	GetTextureSurface returns the main D3D texture pointer.
//--------------------------------------------------------------------
g2dD3D11TexturePtr matShadowMap::GetTextureSurface() 
{
	return m_MainTexture->GetResource();
}
ID3D11RenderTargetView* matShadowMap::GetColorBuffer() const
{
	return m_MainTexture ? m_MainTexture->GetRenderTargetView() : NULL;
}

//--------------------------------------------------------------------
//	GetSurface returns a pointer usable for shaders.
//--------------------------------------------------------------------
ID3D11ShaderResourceView* matShadowMap::GetSurface() const
{
	return m_MainTexture ? m_MainTexture->GetShaderResourceView() : NULL;
}
g2dD3D11ResourcePtr matShadowMap::GetResource() const
{
	return m_MainTexture ? m_MainTexture->GetResource() : NULL;
}

shared_ptr<g2dDepthStencilBuffer> matShadowMap::GetDepthStencilBuffer() const
{
	return m_DepthStencil;
}
//--------------------------------------------------------------------
//	SetSurface sets the DirectDraw surface pointer
//--------------------------------------------------------------------
//void matShadowMap::SetSurface(g2dD3D11ResourcePtr i_Surface)
//{
//	DBG_ERROR("Don't set a shadow map surface externally. Must be constructed internally");
//}
