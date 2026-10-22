/****************************************************************************\
**  matRenderTargetTexture.hpp
**
**	GraphicsVulkan port of GraphicsDX11's matRenderTargetTexture: a
**	matTexture that is also a g2dRenderTarget (a colour image and,
**	if asked, a depth-stencil buffer). MakeCurrent sets them on
**	vkdContext; the clears go through it.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_MATRENDERTARGETTEXTURE_HPP
#error matRenderTargetTexture.hpp multiply included
#endif
#define GVK_MATRENDERTARGETTEXTURE_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif
#ifndef GVK_MATTEXTUREVK_HPP
#include "GraphicsVulkan/Mat/matTextureVk.hpp"
#endif
#ifndef GVK_G2DRESOURCECOUNTERVK_HPP
#include "GraphicsVulkan/G2d/g2dResourceCounterVk.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/G2d/g2dRenderTarget.hpp"
#endif

class g2dDepthStencilBuffer;

namespace gvk
{
	class g2dDepthStencilBufferVk;
	class g2dRenderTargetTextureVk;

	class matRenderTargetTexture : public matTextureVk, public g2dRenderTarget
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
		//	parameters from the image.
		//--------------------------------------------------------------------
		void ReloadInfo();

		//--------------------------------------------------------------------
		//	Make makes the surface into one with the given dimensions and
		//	pixel format.
		//--------------------------------------------------------------------
		void Make(int i_Width, int i_Height, const g2dPFD& i_PFD,
			bool i_bAllocDepthBuf=true, bool i_bAutoGenMipmap = false,
			g2dResourceCounterVk::eResourceCategory i_Category = g2dResourceCounterVk::eRenderTarget,
			bool i_bAntiAlias = false);

		//--------------------------------------------------------------------
		// Return an object pointer to use for rendering to this texture.
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
		//	on the current frame.
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

		//------------------------------------------------------------------------
		//	ResolveImage resolves multisampled i_pSource into i_pDest (same
		//	size and format), as D3D11's ResolveSubresource
		//------------------------------------------------------------------------
		static void ResolveImage(vkdImage* i_pSource, vkdImage* i_pDest);

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
		//	GetTextureSurface returns the main image.
		//--------------------------------------------------------------------
		vkdImage* GetTextureSurface();

		//----------------------------------------------------------------------------
		//	GetSize returns approximate amount of memory (in kbytes) being
		// used by this texture
		//----------------------------------------------------------------------------
		float GetSize() const;

		//------------------------------------------------------------------------
		//	The views rendered to
		//------------------------------------------------------------------------
		vkdImage* GetColorBuffer() const;
		vkdImage* GetDepthBuffer() const;

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
		//	GetImage returns the image, usable for shaders.
		//--------------------------------------------------------------------
		virtual vkdImage* GetImage() const;

		bool IsAA(){ return m_bAA; }	//true if have an MSAA target

	protected:

		shared_ptr<g2dRenderTargetTextureVk> m_RenderTarget;
		shared_ptr<g2dDepthStencilBufferVk> m_DepthStencil;

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
}
