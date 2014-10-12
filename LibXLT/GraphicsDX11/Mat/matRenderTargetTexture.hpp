/****************************************************************************\
**  matRenderTargetTexture.hpp
**
**      matRenderTargetTexture is a matTexture which represents an ordinary
**	rectangular texture with no special ability.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_RENDERTARGETTEXTURE_HPP
#error matRenderTargetTexture.hpp multiply included
#endif
#define MAT_RENDERTARGETTEXTURE_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef MAT_TEXTUREDX11_HPP
#include "GraphicsDX11/mat/matTextureDX11.hpp"
#endif
#ifndef G2D_RESOURCECOUNTERDX11_HPP
#include "GraphicsDX11/g2d/g2dResourceCounterDX11.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/g2d/g2dRenderTarget.hpp"
#endif

class g2dDepthStencilBuffer;
class g2dDepthStencilBufferDX11;
class g2dRenderTargetTextureDX11;

class matRenderTargetTexture : public matTextureDX11, public g2dRenderTarget
{
public:
	//--------------------------------------------------------------------
	//	This constructor will not typically be used by the mat client,
	//	since textures are loaded and managed by the matTextureMgr.
	//--------------------------------------------------------------------
	matRenderTargetTexture(bool i_bStoreDepths, bool i_bFloatDepth);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~matRenderTargetTexture();

	//--------------------------------------------------------------------
	//	ReloadInfo causes the matRenderTargetTexture to regenerate its
	//	parameters from the PAC.  Again, this function should only be
	//	called by the Terawatt PAC components.
	//--------------------------------------------------------------------
	void ReloadInfo();

	//--------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions and
	//	pixel format.  If the pixel format is not supported it will
	//	throw a matUnsupportedPixelFormatX, and leave the old surface
	//	intact.  i_bAllocDepthBuf is ignored if m_bStoreDepths is true.
	//--------------------------------------------------------------------
	void Make(int i_Width, int i_Height, const g2dPFD& i_PFD, 
		bool i_bAllocDepthBuf=true, bool i_bAutoGenMipmap = false,
		g2dResourceCounterDX11::eResourceCategory i_Category = g2dResourceCounterDX11::eRenderTarget,
		bool i_bAntiAlias = false);

	//--------------------------------------------------------------------
	// Return an object pointer to use for rendering to this texture.
	//	Only certain texture types can return this object, most will
	//	return NULL.  The object pointed to will be owned by this 
	//	texture, it should not be deleted by the user.
	//--------------------------------------------------------------------
	virtual g2dRenderTarget* GetRenderTargetAPI();

	//------------------------------------------------------------------------
	// BeginScene must be called before rendering to this window
	//------------------------------------------------------------------------
	virtual void BeginScene();

	//------------------------------------------------------------------------
	// similar to BeginScene, call this to make this the current target of all
	// subsequent rendering calls.  Need not be followed by EndScene.
	//------------------------------------------------------------------------
	virtual void MakeCurrent();

	//------------------------------------------------------------------------
	//	EndScene must be called when you are done with the drawing operations
	//	on the current frame.  It will cause whatever drawing you have
	//	requested to be visible on the screen.
	//------------------------------------------------------------------------
	virtual void EndScene();

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color.
	//------------------------------------------------------------------------
	virtual void Clear(const g2dRGBColor& i_Color);

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color and sets depth and stencil
	//------------------------------------------------------------------------
	virtual void Clear( const maFloatRGBA& i_Color, bool i_ClearDepth = false, float i_Depth = 1, bool i_ClearStencil = true, unsigned int i_Stencil = 0);

	//------------------------------------------------------------------------
	//	Clear fills the depthstencil buffer with the given values.
	//	If there is no depthstencil, then this does nothing.
	//------------------------------------------------------------------------
	virtual void ClearDepthStencil(float i_Depth = 1, bool i_ClearStencil = true, unsigned int i_Stencil = 0);

	//------------------------------------------------------------------------
	//	Resolve will copy the contents to the given texture, resolving 
	//	the multisample surface to a single sampled one.
	//------------------------------------------------------------------------
	void Resolve(matRenderTargetTexture* o_pTexture);
	//replace with a smart copy instead? JCS

	//------------------------------------------------------------------------
	//	CopyWithResolve will copy this texture into the output texture,
	//  if the source is MSAA and the output is non MSAA then it will be resolved
	//  except if the bNoResolve flag is true.
	//  Optional flag will copy/resolve the depth buffer
	//  Returns false if the source cannot be copied/resolved to the output.
	//  As with all copies the source and dest must match dimensions and format.
	//------------------------------------------------------------------------
	bool CopyWithResolve( matRenderTargetTexture* o_pTexture, bool bNoResolve = false, bool bResolveDepth = false );

	//------------------------------------------------------------------------
	//	GetPixelFormat returns the current pixel format of the render target.
	//------------------------------------------------------------------------
	virtual const g2dPFD& GetPixelFormat() const;

	//------------------------------------------------------------------------
	//	GetDimensions returns the width and height of the render target.
	//------------------------------------------------------------------------
	virtual void GetDimensions(int& o_Width, int& o_Height) const;

	//--------------------------------------------------------------------
	//	GetTextureSurface returns the main D3D texture pointer.
	//--------------------------------------------------------------------
	g2dD3D11TexturePtr GetTextureSurface();

	//----------------------------------------------------------------------------
	//	GetSize returns approximate amount of memory (in kbytes) being
	// used by this texture
	//----------------------------------------------------------------------------
	float GetSize() const;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	ID3D11RenderTargetView* GetColorBuffer() const;
	ID3D11DepthStencilView* GetDepthBuffer() const;


	//------------------------------------------------------------------------
	// call this to make this depth buffer the current depth buffer for all
	// subsequent rendering calls.  
	//------------------------------------------------------------------------
	virtual void MakeDepthCurrent();

	//------------------------------------------------------------------------
	//	Returns true if this object has allocated a depth buffer.
	//------------------------------------------------------------------------
	virtual bool GetHasDepthBuffer() const {return m_DepthStencil != NULL;}

	//------------------------------------------------------------------------
	//	GetDepthStencilBuffer accesses the depth/stencil if the target has one.
	//------------------------------------------------------------------------
	virtual shared_ptr<g2dDepthStencilBuffer> GetDepthStencilBuffer() const;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetDepthBuffer(shared_ptr<g2dDepthStencilBuffer> i_DepthStencil);

	//--------------------------------------------------------------------
	//	GetSurface returns a pointer usable for shaders.
	//--------------------------------------------------------------------
	virtual ID3D11ShaderResourceView* GetSurface() const;
	virtual g2dD3D11ResourcePtr GetResource() const;

	bool IsAA(){ return m_bAA; }	//true if have an MSAA target

protected:

	shared_ptr<g2dRenderTargetTextureDX11> m_RenderTarget;
	shared_ptr<g2dDepthStencilBufferDX11> m_DepthStencil;

	bool m_bHasMipMaps;

	bool m_bNoDepthRequested;

	bool m_bAA;

	//--------------------------------------------------------------------
	// set up buffers for rendering
	//--------------------------------------------------------------------
	virtual void configure_device();

private:
	//--------------------------------------------------------------------
	//	This operator = is not implemented.  It is placed here in private
	//	to prevent its usage.
	//--------------------------------------------------------------------
	matRenderTargetTexture& operator = (const matRenderTargetTexture& i_Image);

	bool m_bStoreDepths;

	bool m_bFloatDepth;
};


