/****************************************************************************\
**  matReflectiveShadowMap.hpp
**
**      matReflectiveShadowMap is a matTexture which represents an ordinary
**	rectangular texture with no special ability.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_REFLECTIVE_SHADOWMAP_HPP
#error matReflectiveShadowMap.hpp multiply included
#endif
#define MAT_REFLECTIVE_SHADOWMAP_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef MAT_TEXTUREDX11_HPP
#include "GraphicsDX11/mat/matTextureDX11.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/g2d/g2dRenderTarget.hpp"
#endif

class g2dDepthStencilBufferDX11;
class g2dRenderTargetTextureDX11;

class matReflectiveShadowMap : public matTextureDX11, public g2dRenderTarget
{
public:
	//--------------------------------------------------------------------
	//	This constructor will not typically be used by the mat client,
	//	since textures are loaded and managed by the matTextureMgr.
	//--------------------------------------------------------------------
	matReflectiveShadowMap();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~matReflectiveShadowMap();

	//--------------------------------------------------------------------
	//	ReloadInfo causes the matShadowMap to regenerate its
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
	void Make(int i_Width, int i_Height);

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
	// BeginScene must be called before rendering to this window 
	//------------------------------------------------------------------------
	virtual void BeginScene(int i_index);

	//------------------------------------------------------------------------
	// similar to BeginScene, call this to make this the current target of all
	// subsequent rendering calls.  Need not be followed by EndScene.
	//------------------------------------------------------------------------
	virtual void MakeCurrent();

	//------------------------------------------------------------------------
	// similar to BeginScene, call this to make this the current target of all
	// subsequent rendering calls.  Need not be followed by EndScene.
	//------------------------------------------------------------------------
	virtual void MakeCurrent(int i_index);

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
	//	Clear fills the screen with the given color.
	//------------------------------------------------------------------------
	virtual void Clear(int i_index, const g2dRGBColor& i_Color);

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
	g2dD3D11TexturePtr GetTextureSurface(int i_index = 0);

	//----------------------------------------------------------------------------
	//	GetSize returns approximate amount of memory (in kbytes) being
	// used by this texture
	//----------------------------------------------------------------------------
	float GetSize() const;

	ID3D11RenderTargetView* GetColorBuffer() const;

	//------------------------------------------------------------------------
	// call this to make this depth buffer the current depth buffer for all
	// subsequent rendering calls.  
	//------------------------------------------------------------------------
	virtual void MakeDepthCurrent();

	//------------------------------------------------------------------------
	//	Returns true if this object has allocated a depth buffer.
	//------------------------------------------------------------------------
	virtual bool GetHasDepthBuffer() const;

	//------------------------------------------------------------------------
	//	GetDepthStencilBuffer accesses the depth/stencil if the target has one.
	//------------------------------------------------------------------------
	virtual shared_ptr<g2dDepthStencilBuffer> GetDepthStencilBuffer() const;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetDepthBuffer(shared_ptr<g2dDepthStencilBuffer> i_DepthStencil);

	//--------------------------------------------------------------------
	//	SetSurface sets the DirectDraw surface pointer
	//--------------------------------------------------------------------
	//virtual void SetAllSurfaces();


	//--------------------------------------------------------------------
	//	GetD3dResource gets the rendertarget view by given index
	//--------------------------------------------------------------------
	virtual g2dD3D11RenderTargetPtr GetRenderTargetView(int i_index = 0) const;

	//--------------------------------------------------------------------
	//	GetSurface gets the shader resource view pointer by given index
	//--------------------------------------------------------------------
	virtual ID3D11ShaderResourceView* GetShaderView(int i_index = 0) const;

	//--------------------------------------------------------------------
	//	Enum for multiple color render target
	//--------------------------------------------------------------------
	enum RSMRenderTarget
	{
		e_PosBuffer = 0,
		e_NormalBuffer,
		e_FluxBuffer,
		e_NumRSMRenderTarget
	};

	//--------------------------------------------------------------------
	//	GetSurface returns a pointer usable for shaders.
	//--------------------------------------------------------------------
	virtual ID3D11ShaderResourceView* GetSurface() const;
	virtual g2dD3D11ResourcePtr GetResource() const;

protected:
	//ID3D11RenderTargetView* m_pColorBuffer[e_NumRSMRenderTarget];

	//ID3D11ShaderResourceView* m_pShaderView[e_NumRSMRenderTarget];

	shared_ptr<g2dDepthStencilBufferDX11> m_DepthStencil;
	shared_ptr<g2dRenderTargetTextureDX11> m_MainTexture[e_NumRSMRenderTarget];

	//--------------------------------------------------------------------
	// set up buffers for rendering
	//--------------------------------------------------------------------
	void configure_device(int i_index);

private:
	//--------------------------------------------------------------------
	//	This operator = is not implemented.  It is placed here in private
	//	to prevent its usage.
	//--------------------------------------------------------------------
	matReflectiveShadowMap& operator = (const matReflectiveShadowMap& i_Image);

	bool m_bFloatDepth;

	//g2dD3D11TexturePtr m_pMainTextureD3D[e_NumRSMRenderTarget];
	
};