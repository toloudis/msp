/****************************************************************************\
**  matCubeRenderTargetTexture.hpp
**
**      matCubeRenderTargetTexture is a matTexture which represents an cube 
**	map set up as a render target.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_CUBERENDERTARGETTEXTURE_HPP
#error matCubeRenderTargetTexture.hpp multiply included
#endif
#define MAT_CUBERENDERTARGETTEXTURE_HPP

#ifndef MAT_RENDERTARGETTEXTURE_HPP
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#endif

class matCubeRenderTargetTexture : public matRenderTargetTexture
{
	public:
		//--------------------------------------------------------------------
		//	This constructor will not typically be used by the mat client,
		//	since textures are loaded and managed by the matTextureMgr.
		//--------------------------------------------------------------------
		matCubeRenderTargetTexture();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~matCubeRenderTargetTexture();

		//--------------------------------------------------------------------
		//	ReloadInfo causes the matRenderTargetTexture to regenerate its
		//	parameters from the PAC.  Again, this function should only be
		//	called by the Terawatt PAC components.
		//--------------------------------------------------------------------
		virtual void ReloadInfo();

		//--------------------------------------------------------------------
		//	Make makes the surface into one with the given dimensions and
		//	pixel format.  If the pixel format is not supported it will
		//	throw a matUnsupportedPixelFormatX, and leave the old surface
		//	intact.  i_bAllocDepthBuf is ignored if m_bStoreDepths is true.
		//--------------------------------------------------------------------
		void Make(int i_Width, int i_Height, const g2dPFD& i_PFD, bool i_bAllocDepthBuf=true);

		//------------------------------------------------------------------------
		//	Clear fills the screen with the given color.
		//------------------------------------------------------------------------
		virtual void Clear(const g2dRGBColor& i_Color);

		//------------------------------------------------------------------------
		//	Clear fills the screen with the given color and sets depth and stencil
		//------------------------------------------------------------------------
		virtual void Clear( const maFloatRGBA& i_Color, bool i_ClearDepth = false, float i_Depth = 1, 
			bool i_ClearStencil = true, unsigned int i_Stencil = 0);

		//------------------------------------------------------------------------
		// set one of the 6 faces as the current render target.
		//------------------------------------------------------------------------
		void SetFaceTarget(int i_CubeFace);

		void SaveFaces();

		//--------------------------------------------------------------------
		//	GetCubeTextureSurface returns the main D3D texture pointer.
		//--------------------------------------------------------------------
		g2dD3D11TexturePtr GetCubeTextureSurface() {return m_pCubeTextureD3D;}

		//----------------------------------------------------------------------------
		//	GetSize returns approximate amount of memory (in kbytes) being
		// used by this texture
		//----------------------------------------------------------------------------
		virtual float GetSize() const;

		//--------------------------------------------------------------------
		//	GetSurface returns a pointer usable for shaders.
		//--------------------------------------------------------------------
		virtual ID3D11ShaderResourceView* GetSurface() const;
		virtual g2dD3D11ResourcePtr GetResource() const;
protected:
	//--------------------------------------------------------------------
	// set up buffers for rendering
	//--------------------------------------------------------------------
	virtual void configure_device();

private:

		//--------------------------------------------------------------------
		//	This operator = is not implemented.  It is placed here in private
		//	to prevent its usage.
		//--------------------------------------------------------------------
		matCubeRenderTargetTexture& operator = (const matCubeRenderTargetTexture& i_Image);

		int m_CurrentFaceTarget;
		ID3D11RenderTargetView* m_pCubeFaces[6];
		ID3D11Texture2D* m_pCubeTextureD3D;
		ID3D11ShaderResourceView* m_pCubeTextureView;
};



