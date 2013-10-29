/****************************************************************************\
**  matMipTexture.hpp
**
**      matMipTexture is a matTexture which has mip-map levels.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_MIPTEXTURE_HPP
#error matMipTexture.hpp multiply included
#endif
#define MAT_MIPTEXTURE_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include "Area18/mat/matTextureGL.hpp"
#include "Area18/ogl/oglTexture2d.h"

class matMipTexture : public matTextureGL
{
	public:

		//--------------------------------------------------------------------
		//	This constructor will not typically be used by the mat client,
		//	since textures are loaded and managed by the matTextureMgr.
		//--------------------------------------------------------------------
		matMipTexture();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~matMipTexture();

		//----------------------------------------------------------------------------
		//	GetSize returns approximate amount of memory (in kbytes) being
		// used by this texture
		//----------------------------------------------------------------------------
		float GetSize() const;

		//--------------------------------------------------------------------
		//	ReloadInfo causes the matMipTexture to regenerate its
		//	parameters from the PAC.  Again, this function should only be
		//	called by the Terawatt PAC components.
		//--------------------------------------------------------------------
		void ReloadInfo();

		//--------------------------------------------------------------------
		//	Make makes the surface into one with the given dimensions
		//	and pixel format.  If the pixel format is not supported
		//	it will throw a matUnsupportedPixelFormatX, and leave the old
		//	surface intact.
		//--------------------------------------------------------------------
		void Make(int i_Width, int i_Height, const g2dPFD& i_PFD, int i_NumMipLevels, 
			void* i_PixelData = NULL);

		//--------------------------------------------------------------------
		//	GetSurface returns a pointer usable for shaders.
		//--------------------------------------------------------------------
		virtual GLint GetResource() const;

		void SetSurface(oglTexture2d* t);
		virtual oglTexture2d* GetSurface() const  {return m_Texture.get();}
	private:

		//--------------------------------------------------------------------
		//	This operator = is not implemented.  It is placed here in private
		//	to prevent its usage.
		//--------------------------------------------------------------------
		matMipTexture& operator = (const matMipTexture& i_Image);

		shared_ptr<oglTexture2d> m_Texture;

};


