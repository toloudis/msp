/*****************************************************************************
**  g2dImageSave.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g2d/g2dImageSave.hpp"

#include "Core/dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
namespace g2dImageSave
{
namespace
{
	g2dImageSaveImpl *l_pImpl = NULL;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void SetImplementation(g2dImageSaveImpl *i_pImpl)
{
	l_pImpl = i_pImpl;
}

//------------------------------------------------------------------------
//	Save will attempt to figure out the type based on the filename 
//	passed in.
//
//	If the file format is not known the function will throw 
//	g2dUnknownImageFileTypeX.
//------------------------------------------------------------------------
void Save(const fsLocator& i_FileName, g2dWindow* i_pWin)
{
	DBG_ASSERT(l_pImpl, "No image save implmentation.");
	if (!l_pImpl)
		return;

	return l_pImpl->Save( i_FileName, i_pWin );
}
void Save(const fsLocator& i_FileName, const g2dImage* i_pImage)
{
	DBG_ASSERT(l_pImpl, "No image save implmentation.");
	if (!l_pImpl)
		return;

	return l_pImpl->Save( i_FileName, i_pImage );
}
}

