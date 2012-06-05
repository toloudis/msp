/****************************************************************************\
**  matExceptionX.cpp
**
**      matExceptionX.hpp defines the exceptions that can be thrown from the
**	mat package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Graphics/mat/matExceptionX.hpp"


//----------------------------------------------------------------------------
//	matUnsupportedPixelFormatX is thrown when someone tries to do something
//	with a pixel format which is not supported by the hardware.
//----------------------------------------------------------------------------
matUnsupportedPixelFormatX::matUnsupportedPixelFormatX(const g2dPFD& i_PixelFormat)
:	m_PixelFormat(i_PixelFormat)
{
}

//------------------------------------------------------------------------
// Returns error message string
//------------------------------------------------------------------------
std::string matUnsupportedPixelFormatX::GetErrorMessage() const
{
	return "Unsupported pixel format";
}	

//------------------------------------------------------------------------
//	GetPixelFormat returns the pixel format which couldn't be handled
//------------------------------------------------------------------------
const g2dPFD& matUnsupportedPixelFormatX::GetPixelFormat() const
{
	return m_PixelFormat;
}

//------------------------------------------------------------------------
//	matUnknownImageFileTypeX is thrown when a function encounters a image
//	type that is not supported.
//------------------------------------------------------------------------
matUnknownImageFileTypeX::matUnknownImageFileTypeX(const fsLocator& i_Locator)
:	fsFileExceptionX(i_Locator)
{
}

//------------------------------------------------------------------------
// Returns error message string
//------------------------------------------------------------------------
std::string matUnknownImageFileTypeX::GetErrorMessage() const
{
	return "Unknown image file type or bad image file associated with: " + GetLocatorString();
}	

//----------------------------------------------------------------------------
//	matInvalidTextureSizeX is thrown when an attempt to load a texture w/ invalid
//	dimensions or other such problems occurs
//----------------------------------------------------------------------------
matInvalidTextureSizeX::matInvalidTextureSizeX(const fsLocator& i_Locator)
:	fsFileExceptionX(i_Locator)
{
}

//------------------------------------------------------------------------
// Returns error message string
//------------------------------------------------------------------------
std::string matInvalidTextureSizeX::GetErrorMessage() const
{
	return "Invalid texture size: " + GetLocatorString();
}	

//----------------------------------------------------------------------------
//	matDDSGeneralX is a unspecified DDS/DXT compressed texture error.
//----------------------------------------------------------------------------
matDDSGeneralX::matDDSGeneralX(const fsLocator& i_Locator)
:	fsFileExceptionX(i_Locator)
{
}

//------------------------------------------------------------------------
// Returns error message string
//------------------------------------------------------------------------
std::string matDDSGeneralX::GetErrorMessage() const
{
	return "Error loading DDS file: " + GetLocatorString();
}	


//----------------------------------------------------------------------------
//	matIncorrectShaderVersionX means that the shader trying to be loaded
//		is from a different, unsupported version.
//----------------------------------------------------------------------------
matIncorrectShaderVersionX::matIncorrectShaderVersionX(const fsLocator& i_Locator)
:	fsFileExceptionX(i_Locator)
{
}

//------------------------------------------------------------------------
// Returns error message string
//------------------------------------------------------------------------
std::string matIncorrectShaderVersionX::GetErrorMessage() const
{
	return "Shader file: " + GetLocatorString() + " is from an unsupported version.";
}	
