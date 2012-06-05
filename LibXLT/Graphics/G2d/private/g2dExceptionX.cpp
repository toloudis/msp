/****************************************************************************\
**  g2dExceptionX.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g2d/g2dExceptionX.hpp"

#include "Core/it/itStringUtil.hpp"


//----------------------------------------------------------------------------
//	g2dScreenInitX is thrown when there is some sort of general
//	initialization failure when setting up the drawing system or the screen
//	mode.
//----------------------------------------------------------------------------
g2dScreenInitX::g2dScreenInitX()
{
}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string g2dScreenInitX::GetErrorMessage() const
{
	return "There was a problem initializing the graphics system";
}	

//----------------------------------------------------------------------------
//	g2dHardwareCapabilityX is thrown when the video card is determined not 
//	to meet our minimum requirements.
//----------------------------------------------------------------------------
g2dHardwareCapabilityX::g2dHardwareCapabilityX()
{
}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string g2dHardwareCapabilityX::GetErrorMessage() const
{
	return "Insufficient hardware to run. Your video card must support Shader Model 3 and have at least 512 MB of video memory.";
}	

//----------------------------------------------------------------------------
//	g2dUnknownFontX is thrown when someone requests a font which can't be
//	found.
//----------------------------------------------------------------------------
g2dUnknownFontX::g2dUnknownFontX(const itString& i_Name)
:	m_Name(i_Name)
{
}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string g2dUnknownFontX::GetErrorMessage() const
{
	return "Unknown font: " + itStringUtil::GetStdString(m_Name);
}	

//------------------------------------------------------------------------
//	GetName returns the name of the font which couldn't be found
//------------------------------------------------------------------------
const itString& g2dUnknownFontX::GetName() const
{
	return m_Name;
}

//----------------------------------------------------------------------------
//	g2dGeneralX is a unspecified g2d error.
//----------------------------------------------------------------------------
g2dGeneralX::g2dGeneralX()
{
}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string g2dGeneralX::GetErrorMessage() const
{
	return "Graphics error";
}	

//------------------------------------------------------------------------
//	g2dUnknownImageFileTypeX is thrown when a function encounters a image
//	type that is not supported.
//------------------------------------------------------------------------
g2dUnknownImageFileTypeX::g2dUnknownImageFileTypeX()
{
}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string g2dUnknownImageFileTypeX::GetErrorMessage() const
{
	return "Unknown image file type";
}	

//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dOutOfVideoMemoryX::g2dOutOfVideoMemoryX()
{
}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string g2dOutOfVideoMemoryX::GetErrorMessage() const
{
	return "Out of video memory";
}	

//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dOutOfSystemMemoryX::g2dOutOfSystemMemoryX()
{
}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string g2dOutOfSystemMemoryX::GetErrorMessage() const
{
	return "Out of system memory";
}	

//----------------------------------------------------------------------------
//	g2dWriteBufferOverrunX is thrown when the buffer is *about* to be 
//	overrun. (copying pixels from D3D surface with RLE compression in TGA data format)
//----------------------------------------------------------------------------
g2dWriteBufferOverrunX::g2dWriteBufferOverrunX()
{
}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string g2dWriteBufferOverrunX::GetErrorMessage() const
{
	return "Write buffer overflow";
}	

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g2dCaptureX::g2dCaptureX()
{

}

//--------------------------------------------------------------------
// Returns error message string
//--------------------------------------------------------------------
std::string g2dCaptureX::GetErrorMessage() const
{
	return "Cannot copy surface";
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g2dImageSaveX::g2dImageSaveX()
{

}

//--------------------------------------------------------------------
// Returns error message string
//--------------------------------------------------------------------
std::string g2dImageSaveX::GetErrorMessage() const
{
	return "Cannot save image";
}
