/*****************************************************************************
**  g2dFontUtil.cpp
**
**      g2dFontUtil contains functions related to font support in the
**	g2d package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_FONTUTIL_HPP
#error g2dFontUtil.hpp multiply included
#endif
#define G2D_FONTUTIL_HPP

#ifndef G2D_FONTHANDLE_HPP
#include "Graphics/g2d/g2dFontHandle.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class fsLocator;
class itString;


//============================================================================
//============================================================================
class g2dFontUtilImpl
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~g2dFontUtilImpl() {};

	//------------------------------------------------------------------------
	//	LoadFont make the given font name available for use and returns
	//	a g2dFontHandle which can be used later.  When you are done with the
	//	font, call ReleaseFont.
	//------------------------------------------------------------------------
	virtual g2dFontHandle LoadFont(const itString& i_Name, int i_Size) = 0;

	//------------------------------------------------------------------------
	//	ReleaseFont informs the system that you are done with the given
	//	g2dFontHandle.
	//------------------------------------------------------------------------
	virtual void ReleaseFont(g2dFontHandle i_Handle) = 0;

	//------------------------------------------------------------------------
	// GetLoadedFontName returns the name of the font being used for the given
	// g2dFontHandle.  It looks this up in the system to learn exactly what
	// the system loaded.
	//------------------------------------------------------------------------
	virtual void GetLoadedFontName( g2dFontHandle i_Font, itString& o_Name ) = 0;
	virtual void GetLoadedFontName( g2dFontHandle i_Font, std::string& o_Name ) = 0;

	//------------------------------------------------------------------------
	//	GetTextSizeInPixels returns the width and height in pixels of the given text and font
	//------------------------------------------------------------------------
	virtual void GetTextSizeInPixels(g2dFontHandle i_Font,
									 const itString& i_Text,
									 int& o_Width,
									 int& o_Height) = 0;

	//------------------------------------------------------------------------
	//	Get and set the global font path
	//------------------------------------------------------------------------
	virtual void SetGlobalFontBitmap(const fsLocator& i_FontPath) = 0;
	
	virtual void GetGlobalFontBitmap(fsLocator& o_FontPath) = 0;

};


//============================================================================
//============================================================================
namespace g2dFontUtil
{
	//------------------------------------------------------------------------
	//	enums
	//------------------------------------------------------------------------
	enum
	{
		e_NoFontIndex = -1,
	};

	//------------------------------------------------------------------------
	// Libraries call this to add implementation
	//------------------------------------------------------------------------
	void SetImplementation(g2dFontUtilImpl* i_pImpl);
	g2dFontUtilImpl* Implementation();

	//------------------------------------------------------------------------
	//	SetFontTSF gives the FontUtil the location of the .tsf to use for indexing
	//------------------------------------------------------------------------
	//void SetFontTSF(const fsLocator& i_TSFLoc);

	//------------------------------------------------------------------------
	//	LoadFont make the given font name available for use and returns
	//	a g2dFontHandle which can be used later.  When you are done with the
	//	font, call ReleaseFont. i_Index is an index into the fontutil's .tsf
	//	identifying the name of the font
	//------------------------------------------------------------------------
	g2dFontHandle LoadFont(const itString& i_Name, int i_Size);
	//g2dFontHandle LoadFont(int i_Index, int i_Size);

	//------------------------------------------------------------------------
	//	ReleaseFont informs the system that you are done with the given
	//	g2dFontHandle.
	//------------------------------------------------------------------------
	void ReleaseFont(g2dFontHandle i_Handle);

	//------------------------------------------------------------------------
	//	GetTextSizeInPixels returns the width and height in pixels of the given text and font
	//------------------------------------------------------------------------
	void GetTextSizeInPixels(g2dFontHandle i_Font, const itString& i_Text, int& o_Width, int& o_Height);

	//------------------------------------------------------------------------
	//	GetFontInfoFromHandle returns the name and size of the given fonthandle
	//------------------------------------------------------------------------
	bool GetFontInfoFromHandle(g2dFontHandle i_Font, itString& o_Name, int& o_Size, int& o_Index);

	//------------------------------------------------------------------------
	//	ReleaseAllFonts releases all fonts and clears the map
	//------------------------------------------------------------------------
	void ReleaseAllFonts();

	//------------------------------------------------------------------------
	//	Get and set the global font path
	//------------------------------------------------------------------------
	void SetGlobalFontBitmap(const fsLocator& i_FontPath);
	
	void GetGlobalFontBitmap(fsLocator& o_FontPath);

	//------------------------------------------------------------------------
	//	Don't call Init() and CleanUp() yourself; they are called
	//	by the package Init and Cleanup.
	//------------------------------------------------------------------------
	void Init();
	void CleanUp() throw();
}
