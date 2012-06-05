/****************************************************************************\
**  matExceptionX.hpp
**
**      matExceptionX.hpp defines the exceptions that can be thrown from the
**	mat package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_EXCEPTIONX_HPP
#error matExceptionX.hpp multiply included
#endif
#define MAT_EXCEPTIONX_HPP

#ifndef FS_FILEX_HPP
#include "Core/Fs/fsFileX.hpp"
#endif 
#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif

//----------------------------------------------------------------------------
//	matUnsupportedPixelFormatX is thrown when someone tries to do something
//	with a pixel format which is not supported by the hardware.
//----------------------------------------------------------------------------
class matUnsupportedPixelFormatX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		matUnsupportedPixelFormatX(const g2dPFD& i_PixelFormat);

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;

		//------------------------------------------------------------------------
		//	GetPixelFormat returns the pixel format which couldn't be handled
		//------------------------------------------------------------------------
		const g2dPFD& GetPixelFormat() const;

	private:
		g2dPFD m_PixelFormat;
};

//----------------------------------------------------------------------------
//	matUnknownImageFileTypeX is thrown when a function encounters a image
//	type that is not supported.
//----------------------------------------------------------------------------
class matUnknownImageFileTypeX : public fsFileExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		matUnknownImageFileTypeX(const fsLocator& i_Locator);

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};

//----------------------------------------------------------------------------
//	matInvalidTextureSizeX is thrown when an attempt to load a texture w/ invalid
//	dimensions or other such problems occurs
//----------------------------------------------------------------------------
class matInvalidTextureSizeX : public fsFileExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		matInvalidTextureSizeX(const fsLocator& i_Locator);

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};

//----------------------------------------------------------------------------
//	matDDSGeneralX is a unspecified DDS/DXT compressed texture error.
//----------------------------------------------------------------------------
class matDDSGeneralX : public fsFileExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		matDDSGeneralX(const fsLocator& i_Locator);

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};

//----------------------------------------------------------------------------
//	matIncorrectShaderVersionX means that the shader trying to be loaded
//		is from a different, unsupported version.
//----------------------------------------------------------------------------
class matIncorrectShaderVersionX : public fsFileExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		matIncorrectShaderVersionX(const fsLocator& i_Locator);

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};
