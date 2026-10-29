/****************************************************************************\
**  mnmAppUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/mnmAppUtil.hpp"

#include "Support/mnm/mnmConstants.hpp"

#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMainWindow.hpp"

#include <string>
#include <sstream>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
	int l_ErrorCode = 0;
}

//------------------------------------------------------------------------
//	UpdateTitleBar - update the titlebar with the appropriate text
//------------------------------------------------------------------------
void mnmAppUtil::UpdateTitleBar( bool i_bDisplayDirtyFlag )
{
	std::string title_str(mnmConstants::c_PRODUCT_FOR_DISPLAY);
	title_str += " - ";

	std::string sceneFilename;
	if ( docSingleDocumentMgr::GetFilename().GetNumNames() > 0 )
	{
		sceneFilename = docSingleDocumentMgr::GetFilename().GetLastName();
	}
	else
	{
		sceneFilename = "Untitled";
	}
	title_str += sceneFilename;

	if (i_bDisplayDirtyFlag)
		title_str += "*";
	guiMainWindow::SetAppTitle(title_str);

	//std::ostringstream buffer;
	//buffer << mnmConstants::c_PRODUCTTM << " - " << sceneFilename << (i_bDisplayDirtyFlag ? "*":" ");
	//guiMainWindow::SetAppTitle( buffer.str().c_str() );
}

//------------------------------------------------------------------------
// Get/Set the error code for the app
//------------------------------------------------------------------------
int mnmAppUtil::GetErrorCode()
{
	return l_ErrorCode;
}
void mnmAppUtil::SetErrorCode( int i_ErrorCode )
{
	l_ErrorCode = i_ErrorCode;
}
