/****************************************************************************\
**  matMipTexture.hpp
**
**      matMipTexture is a matTexture which has mip-map levels.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Area18/mat/matMipTexture.hpp"

#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/mat/matExceptionX.hpp"
//#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
//#include "GraphicsDX11/g2d/g2dTextureDX11.hpp"


//--------------------------------------------------------------------
//	This constructor makes an "empty" surface
//--------------------------------------------------------------------
matMipTexture::matMipTexture()
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matMipTexture::~matMipTexture()
{
}

//----------------------------------------------------------------------------
//	GetSize returns approximate amount of memory (in kbytes) being
// used by this texture
//----------------------------------------------------------------------------
float matMipTexture::GetSize() const
{
	float base_size = matTexture::GetSize();

	// mip-map texture size is about 1/3 bigger than that of normal texture
	return (base_size * 4) / 3;
}

//--------------------------------------------------------------------
//	ReloadInfo causes the matMipTexture to regenerate its
//	parameters from the PAC.  Again, this function should only be
//	called by the Terawatt PAC components.
//--------------------------------------------------------------------
void matMipTexture::ReloadInfo()
{
	if (m_Texture == NULL) {
		this->SetHeight(0);
		this->SetWidth(0);
		g2dPFD pfd;
		pfd.SetPixelFormat(g2dPFD::e_Unknown);
		pfd.SetBitsPerPixel(0);
		this->SetPixelFormat(pfd);
		return;
	}

	GLint id = m_Texture->GetResource();
	glBindTexture(GL_TEXTURE_2D, id);
	// get tex params from gl object.
	GLint w;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,
	 	GL_TEXTURE_WIDTH,
 		&w);
	this->SetWidth(w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,
	 	GL_TEXTURE_HEIGHT,
 		&w);
	this->SetHeight(w); 

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,
	 	GL_TEXTURE_INTERNAL_FORMAT,
 		&w);
	g2dPFD pfd;
	ogl::PFDFromGLFormat(w, pfd);
	this->SetPixelFormat(pfd);
}

//--------------------------------------------------------------------
//	Make makes the surface into one with the given dimensions and
//	pixel format.  If the pixel format is not supported it will
//	throw a matUnsupportedPixelFormatX, and leave the old surface
//	intact.
//--------------------------------------------------------------------
void matMipTexture::Make(int i_Width, int i_Height, const g2dPFD& i_PFD, int i_NumMipLevels, 
						 void* i_PixelData /*= NULL*/)
{
	// big assumption: initial data is packed tightly and pitch
	// is related directly to pixel size for this format
	int pitch = i_PFD.BitsPerPixel()*i_Width/8;
	oglTexture2d* new_texture = new oglTexture2d(NULL, i_Width, i_Height, ogl::GLFormatFromPFD(i_PFD), i_PixelData, GL_RGBA, GL_BYTE);

	// if we get to this point, the allocation succeeded and we can get rid of our old surface
	this->SetSurface(new_texture);
	this->SetWidth(i_Width);
	this->SetHeight(i_Height);
	this->SetPixelFormat(i_PFD);
}

//--------------------------------------------------------------------
//	SetSurface sets the DirectDraw surface pointer
//--------------------------------------------------------------------
void matMipTexture::SetSurface(oglTexture2d* i_Surface)
{
	// out with the old, in with the new
	m_Texture.reset(i_Surface);
}
GLint matMipTexture::GetResource() const
{
	return m_Texture->GetResource();
}

