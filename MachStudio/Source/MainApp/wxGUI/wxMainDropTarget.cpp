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
#ifndef ENV_STRING_HPP
#include "Core/env/envString.hpp"
#endif
#include "Tool/tma3d/tma3dRenderView.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
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
		fsFileUtil::UnicodeStringToLocator( filenames[0].utf8_string(), file_loc );

		std::string filename = file_loc.GetLastName();
		std::string ext;
		envString::GetExtension(filename,  ext );

		if (envString::EqualNoCase(ext, std::string("mtl")))
		{
			// convert the panel position to a pixel in the rendered image
			int render_x = x;
			int render_y = y;
			m_pRenderView->PanelToRenderPixel(render_x, render_y);
			mbrwPaintUtil::PasteMaterialFile( m_pRenderView, render_x, render_y, file_loc);
		}
		// a scene, or a scene file of another format (glTF), opened as a new scene
		if(envString::EqualNoCase(ext, std::string("mab")) || docSingleTypeMgr::IsForeignFile(file_loc))
		{
			//DBG_LOG("Received drag and drop");
			guiSingleDocHandler::Open(file_loc, false);
		}
		if(envString::EqualNoCase(ext, std::string("gxb")))
		{
			chtrOperations::AddObject(chtrScriptData(file_loc));
		}
	}

	// Always return false, because we are never really receiving a file;
	// just loading it.
	return false;
}

#endif	// USE_WXWIDGETS
