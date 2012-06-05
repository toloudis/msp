/****************************************************************************\
**  g2dExceptionX.hpp
**
**      g2dExceptionX.hpp defines the exceptions that can be thrown from the
**	g2d package.  (The g2d package's package error index is 8).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_EXCEPTIONX_HPP
#error g2dExceptionX.hpp multiply included
#endif
#define G2D_EXCEPTIONX_HPP

#ifndef ENV_EXCEPTIONX_HPP
#include "Core/env/envExceptionX.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


//============================================================================
//	g2dScreenInitX is thrown when there is some sort of general
//	initialization failure when setting up the drawing system or the screen
//	mode.
//============================================================================
class g2dScreenInitX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		g2dScreenInitX();

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};


//============================================================================
//	g2dHardwareCapabilityX is thrown when the video card is determined not 
//	to meet our minimum requirements.
//============================================================================
class g2dHardwareCapabilityX : public envExceptionX
{
	public:

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		g2dHardwareCapabilityX();

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};

//============================================================================
//	g2dUnknownFontX is thrown when someone requests a font which can't be
//	found.
//============================================================================
class g2dUnknownFontX : public envExceptionX
{
	public:

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		g2dUnknownFontX(const itString& i_Name);

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;

		//------------------------------------------------------------------------
		//	GetName returns the name of the font which couldn't be found
		//------------------------------------------------------------------------
		const itString& GetName() const;

	private:
		itString m_Name;
};


//============================================================================
//	g2dGeneralX is a unspecified g2d error.
//============================================================================
class g2dGeneralX : public envExceptionX
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		g2dGeneralX();

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};

//============================================================================
//	g2dUnknownImageFileTypeX is thrown when a function encounters a image
//	type that is not supported.
//============================================================================
class g2dUnknownImageFileTypeX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		g2dUnknownImageFileTypeX();

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};

//============================================================================
//	g2dOutOfVideoMemoryX reports that the video card has run out of video memory
//  and cannot complete the current operation.  Usually a texture creation.
//============================================================================
class g2dOutOfVideoMemoryX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		g2dOutOfVideoMemoryX();

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};

//============================================================================
//	g2dOutOfSystemMemoryX reports that the system has run out of main memory
//  and cannot complete the current operation.  Usually a texture creation.
//============================================================================
class g2dOutOfSystemMemoryX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		g2dOutOfSystemMemoryX();

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};


//============================================================================
//	g2dWriteBufferOverrunX is thrown when the buffer is *about* to be 
//	overrun. (copying pixels from D3D surface with RLE compression in TGA data format)
//============================================================================
class g2dWriteBufferOverrunX : public envExceptionX
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		g2dWriteBufferOverrunX();

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};

//============================================================================
//	g2dCaptureX is thrown when a buffer cannot be captured 
//============================================================================
class g2dCaptureX : public envExceptionX
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		g2dCaptureX();

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};

//============================================================================
//	g2dImageSaveX is thrown when an image can not be saved to disk
//============================================================================
class g2dImageSaveX : public envExceptionX
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		g2dImageSaveX();

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};
