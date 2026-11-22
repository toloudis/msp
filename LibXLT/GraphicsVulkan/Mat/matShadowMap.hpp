/****************************************************************************\
**  matShadowMap.hpp
**
**	GraphicsVulkan port of GraphicsDX11's matShadowMap: a shadow map is a
**	matTexture that is also a g2dRenderTarget, an R32_FLOAT colour image
**	(the depth map renderer writes the light-space depth into it; shaders
**	read it) and a D24S8 depth buffer, as on DX11. MakeCurrent sets them on
**	vkdContext; the clears go through it.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_MATSHADOWMAP_HPP
#error matShadowMap.hpp multiply included
#endif
#define GVK_MATSHADOWMAP_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif
#ifndef GVK_MATTEXTUREVK_HPP
#include "GraphicsVulkan/Mat/matTextureVk.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/G2d/g2dRenderTarget.hpp"
#endif

class g2dDepthStencilBuffer;

namespace gvk
{
	class g2dDepthStencilBufferVk;
	class g2dRenderTargetTextureVk;

	class matShadowMap : public matTextureVk, public g2dRenderTarget
	{
	public:
		//--------------------------------------------------------------------
		//	This constructor will not typically be used by the mat client,
		//	since textures are loaded and managed by the matTextureMgr.
		//--------------------------------------------------------------------
		matShadowMap();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~matShadowMap();

		//--------------------------------------------------------------------
		//	ReloadInfo causes the matShadowMap to regenerate its
		//	parameters from the image.
		//--------------------------------------------------------------------
		void ReloadInfo();

		//--------------------------------------------------------------------
		//	Make makes the surface into one with the given dimensions.
		//--------------------------------------------------------------------
		void Make(int i_Width, int i_Height);

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

		vkdImage* GetColorBuffer() const;

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

		//--------------------------------------------------------------------
		//	GetImage returns the image, usable for shaders.
		//--------------------------------------------------------------------
		virtual vkdImage* GetImage() const;

	protected:
		shared_ptr<g2dDepthStencilBufferVk> m_DepthStencil;
		shared_ptr<g2dRenderTargetTextureVk> m_MainTexture;

		//--------------------------------------------------------------------
		// set up buffers for rendering
		//--------------------------------------------------------------------
		void configure_device();

	private:
		//--------------------------------------------------------------------
		//	This operator = is not implemented.  It is placed here in private
		//	to prevent its usage.
		//--------------------------------------------------------------------
		matShadowMap& operator = (const matShadowMap& i_Image);
	};
}
