/*****************************************************************************
**  g2dFontUtil.cpp
**
**      g2dFontUtil contains functions related to font support in the
**	g2d package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g2d/g2dFontUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsLocator.hpp"
//#include "Core/gf/gfTextSetFile.hpp"

#include <map>


//============================================================================
//============================================================================
namespace g2dFontUtil
{

namespace
{
g2dFontUtilImpl* l_pImpl = NULL;
//gfTextSetFile *l_TSF = NULL;


//============================================================================
//index is stored so it can be returned when the parsers write
//============================================================================
struct FontKeyStruct
{
	itString m_Name;
	int m_Size;
	int m_Index;
	FontKeyStruct(const itString& i_Name, int i_Size, int i_Index = e_NoFontIndex)
		: m_Name(i_Name), m_Size(i_Size), m_Index(i_Index)
	{
	}
};

//============================================================================
//============================================================================
struct FontValueStruct
{
	g2dFontHandle m_Font;
	int m_RefCount;
	FontValueStruct()
		: m_Font(NULL), m_RefCount(0)
	{
	}
	FontValueStruct(g2dFontHandle i_Font)
		: m_Font(i_Font), m_RefCount(1)
	{
	}
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool operator<( const FontKeyStruct& a, const FontKeyStruct& b )
{
	return (a.m_Name < b.m_Name) || (a.m_Name == b.m_Name && a.m_Size < b.m_Size);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool operator==( const FontKeyStruct& a, const FontKeyStruct& b )
{
	return a.m_Name == b.m_Name && a.m_Size == b.m_Size;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
typedef std::map<FontKeyStruct, FontValueStruct> FontMap;
FontMap l_FontMap;

}

//------------------------------------------------------------------------
// Libraries call this to add implementation
//------------------------------------------------------------------------
void SetImplementation(g2dFontUtilImpl* i_pImpl)
{
	l_pImpl = i_pImpl;
}
g2dFontUtilImpl* Implementation()
{
	return l_pImpl;
}

//------------------------------------------------------------------------
//	SetFontTSF gives the FontUtil the location of the .tsf to use for indexing
//------------------------------------------------------------------------
//void SetFontTSF(const fsLocator& i_TSFLoc)
//{
//	l_TSF = new gfTextSetFile(i_TSFLoc);
//}

//------------------------------------------------------------------------
//	LoadFont make the given font name available for use and returns
//	a g2dFontHandle which can be used later.  When you are done with the
//	font, call ReleaseFont.
//------------------------------------------------------------------------
g2dFontHandle LoadFont(const itString& i_Name, int i_Size)
{
	FontMap::iterator i = l_FontMap.find(FontKeyStruct(i_Name, i_Size));
	if (i != l_FontMap.end())
	{
		(i->second.m_RefCount)++;
		return i->second.m_Font;
	}

	DBG_ASSERT(l_pImpl, "No font util implementation.");
	g2dFontHandle Font;
	if (l_pImpl)
		Font = l_pImpl->LoadFont(i_Name, i_Size);
	l_FontMap[FontKeyStruct(i_Name, i_Size)] = FontValueStruct(Font);
	return Font;
}

//g2dFontHandle LoadFont(int i_Index, int i_Size)
//{
//	DBG_ASSERT(l_TSF, "trying to index a font without having SetFontTSF, BAD NEWS!!!!");
//
//	FontMap::iterator i = l_FontMap.find(FontKeyStruct(l_TSF->GetText(i_Index), i_Size, i_Index));
//	if (i != l_FontMap.end())
//	{
//		(i->second.m_RefCount)++;
//		return i->second.m_Font;
//	}
//
//	DBG_ASSERT(l_pImpl, "No font util implementation.");
//	g2dFontHandle Font = l_pImpl->LoadFont(l_TSF->GetText(i_Index), i_Size);
//	l_FontMap[FontKeyStruct(l_TSF->GetText(i_Index), i_Size, i_Index)] = FontValueStruct(Font);
//	return Font;
//}

//------------------------------------------------------------------------
//	ReleaseFont informs the system that you are done with the given
//	g2dFontHandle.
//------------------------------------------------------------------------
void ReleaseFont(g2dFontHandle i_Handle)
{
	FontMap::iterator i;
	for (i = l_FontMap.begin(); i != l_FontMap.end(); ++i)
	{
		if (i_Handle == i->second.m_Font)
		{
			if (!((i->second.m_RefCount)--))
			{
				DBG_ASSERT(l_pImpl, "No font util implementation.");
				if (l_pImpl)
					l_pImpl->ReleaseFont(i_Handle);
				l_FontMap.erase(i);
				return;
			}
			return;
		}
	}
}

//------------------------------------------------------------------------
//	GetTextSizeInPixels returns the width and height in pixels of the given text and font
//------------------------------------------------------------------------
void GetTextSizeInPixels(g2dFontHandle i_Font, const itString& i_Text, int& o_Width, int& o_Height)
{
	if (!i_Text.GetLength())
	{
		//if there are no characters, then these values must be zero
		o_Width = 0;
		o_Height = 0;
	}
	else
	{
		DBG_ASSERT(l_pImpl, "No font util implementation.");
		if (l_pImpl)
			l_pImpl->GetTextSizeInPixels(i_Font, i_Text, o_Width, o_Height);
	}
}

//------------------------------------------------------------------------
//	GetFontInfoFromHandle returns the name and size of the given fonthandle
//------------------------------------------------------------------------
bool GetFontInfoFromHandle(g2dFontHandle i_Font, itString& o_Name, int& o_Size, int& o_Index)
{
	FontMap::iterator i;
	for (i = l_FontMap.begin(); i != l_FontMap.end(); ++i)
	{
		if (i_Font == i->second.m_Font)
		{
			o_Name = i->first.m_Name;
			o_Size = i->first.m_Size;
			o_Index = i->first.m_Index;
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------
//	ReleaseAllFonts releases all fonts and clears the map
//------------------------------------------------------------------------
void ReleaseAllFonts()
{
	DBG_ASSERT(l_pImpl, "No font util implementation.");
	if (l_pImpl)
	{
		FontMap::iterator i;
		for (i = l_FontMap.begin(); i != l_FontMap.end(); ++i)
		{
			l_pImpl->ReleaseFont(i->second.m_Font);
		}
	}

	l_FontMap.clear();
}

//------------------------------------------------------------------------
//	Get and set the global font path
//------------------------------------------------------------------------
void SetGlobalFontBitmap(const fsLocator& i_FontPath)
{
	DBG_ASSERT(l_pImpl, "No font util implementation.");
	if (l_pImpl)
	{
		l_pImpl->SetGlobalFontBitmap(i_FontPath);
	}
}

void GetGlobalFontBitmap(fsLocator& o_FontPath)
{
	DBG_ASSERT(l_pImpl, "No font util implementation.");
	if (l_pImpl)
	{
		l_pImpl->GetGlobalFontBitmap(o_FontPath);
	}
}

//------------------------------------------------------------------------
//	Don't call Init() and CleanUp() yourself; they are called
//	by the package Init and Cleanup.
//------------------------------------------------------------------------
void Init()
{
}

void CleanUp() throw()
{

	//delete l_TSF;
	//l_TSF = NULL;
}

}
