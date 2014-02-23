/*****************************************************************************
**  wxMainDropTarget.hpp
**
**     Drop target for receiving drag and drop into render panel
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "StdAfx.h"
#include "MainApp/wxGUI/wxMainDropTarget.hpp"

#include "Features/MaterialBrowse/mbrwPaintUtil.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsLocator.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Systems/Character/Undo/chtrOperations.hpp"
#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
wxMainDropTarget::wxMainDropTarget( tma3dRenderView *i_pRenderView ) 
: m_pRenderView(i_pRenderView)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
wxMainDropTarget::~wxMainDropTarget()
{
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
bool wxMainDropTarget::OnDropFiles(wxCoord x, wxCoord y, 
						 const wxArrayString& filenames)
{
	//DBG_LOG("Received drag and drop");

	if (filenames.size() == 1)
	{
		fsLocator file_loc;
		fsFileUtil::UnicodeStringToLocator( itString((const char*)filenames[0].c_str()), file_loc );

		itString filename = file_loc.GetLastName();
		itString ext;
		filename.GetExtension( ext );

		if (itStringUtil::Equal(ext, itString("mtl")))
		{
			mbrwPaintUtil::PasteMaterialFile( m_pRenderView, x, y, file_loc);
		}
		if(itStringUtil::Equal(ext, itString("mab")))
		{
			//DBG_LOG("Received drag and drop");
			guiSingleDocHandler::Open(file_loc, false);
		}
		if(itStringUtil::Equal(ext, itString("gxb")))
		{
			chtrOperations::AddObject(chtrScriptData(file_loc));
		}
	}

	// Always return false, because we are never really receiving a file;
	// just loading it.
	return false;
}

#endif	// USE_WXWIDGETS
