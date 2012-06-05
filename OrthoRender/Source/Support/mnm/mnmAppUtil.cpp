/****************************************************************************\
**  mnmAppUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/mnmAppUtil.hpp"

#include "Support/mnm/mnmConstants.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMainWindow.hpp"

#include <string>
#include <sstream>


//------------------------------------------------------------------------
//	UpdateTitleBar - update the titlebar with the appropriate text
//------------------------------------------------------------------------
void mnmAppUtil::UpdateTitleBar( bool i_bDisplayDirtyFlag )
{
	std::string sceneFilename;
	if ( docSingleDocumentMgr::GetFilename().GetNumNames() > 0 )
	{
		sceneFilename = itStringUtil::GetStdString( (docSingleDocumentMgr::GetFilename().GetLastName()) );
	}
	else
	{
		sceneFilename = "Untitled";
	}

	std::ostringstream buffer;
	buffer << mnmConstants::c_PRODUCT << " - " << sceneFilename << (i_bDisplayDirtyFlag ? "*":" ");
	guiMainWindow::SetAppTitle( buffer.str().c_str() );
}
