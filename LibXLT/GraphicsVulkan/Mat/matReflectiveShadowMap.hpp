/****************************************************************************\
**  matReflectiveShadowMap.hpp
**
**	GraphicsVulkan port of GraphicsDX11's matReflectiveShadowMap: a
**	reflective shadow map is a matTexture that is also a g2dRenderTarget,
**	three R32G32B32A32_FLOAT colour images (world position, normal and
**	flux, which the RSM renderer writes and LPV GI injects) and a D24S8
**	depth buffer, as on DX11. MakeCurrent sets all three on vkdContext; the
**	indexed calls set one; the clears go through vkdContext.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_MATREFLECTIVESHADOWMAP_HPP
#error matReflectiveShadowMap.hpp multiply included
#endif
#define GVK_MATREFLECTIVESHADOWMAP_HPP

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

	class matReflectiveShadowMap : public matTextureVk, public g2dRenderTarget
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
		//	ReloadInfo causes the map to regenerate its parameters from the
		//	images.
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
		//	on the current frame.
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
		//	GetTextureSurface returns one target's image.
		//--------------------------------------------------------------------
		vkdImage* GetTextureSurface(int i_index = 0);

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

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void SetDepthBuffer(shared_ptr<g2dDepthStencilBuffer> i_DepthStencil);

		//--------------------------------------------------------------------
		//	GetRenderTargetView gets the view rendered to, by index
		//--------------------------------------------------------------------
		virtual vkdImage* GetRenderTargetView(int i_index = 0) const;

		//--------------------------------------------------------------------
		//	GetShaderView gets the image shaders read, by index
		//--------------------------------------------------------------------
		virtual vkdImage* GetShaderView(int i_index = 0) const;

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
		//	GetImage returns the first target's image, usable for shaders.
		//--------------------------------------------------------------------
		virtual vkdImage* GetImage() const;

	protected:
		shared_ptr<g2dDepthStencilBufferVk> m_DepthStencil;
		shared_ptr<g2dRenderTargetTextureVk> m_MainTexture[e_NumRSMRenderTarget];

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
	};
}
