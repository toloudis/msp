/*****************************************************************************
**  fgtCommandShowFrame.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/FilmGates/fgtCommandShowFrame.hpp"

#include "Features/FilmGates/fgtFrameMgr.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


#include <assert.h>


///////////////////////////////////////////////////
// Event Handler(s)
//
namespace fgtCommandShowFrameNS
{
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandExecuteHandler( cmaCommand* pCmd )
	{
		fgtCommandShowFrame* pCSM = dynamic_cast<fgtCommandShowFrame*>(pCmd);

		DBG_ASSERT( pCSM != NULL, "Invalid command hooked up to ShowFrame command" );

		// TODO: - improve control over frames - including custom frames and on-the-fly frames
		bool bChecked = !pCmd->GetChecked();
		cmaCommandMgr::CommandSetChecked( pCmd, bChecked );

		//DBG_LOG3( "Command ShowFrame Executed (%02d-%s) [%s]", pCSM->GetObjectID(), pCSM->GetMenuDesc().c_str(), (bChecked?"true":"false") );

		//fgtFrameMgr::Show( pCSM->GetMenuDesc(), pCmd->GetChecked() );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::string fgtCommandShowFrame::GetConstTagName()
{
	return std::string("fgtShowFrame");
}

//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
fgtCommandShowFrame::fgtCommandShowFrame(const std::string& i_MenuDesc)
:	cmaCommand( std::string(""),
				fgtCommandShowFrameNS::CommandExecuteHandler,
				NULL ),
	m_MenuDesc( i_MenuDesc )
{
	//	create the name truncated to 32 chars
	const int MAX_SIZE_OF_TAG = 48;
	const char * desc = "Safe Frame ";
	std::string name;
	name = desc;
	name.append( i_MenuDesc.c_str(), (MAX_SIZE_OF_TAG - sizeof(desc)) );

	//char name[64];
	//assert( (i_MenuDesc.size()+ sizeof(desc)) < sizeof(name));
	//sprintf(name, "%s%s", desc, m_MenuDesc.c_str() );

	this->SetTag(name);
	this->SetDescription(std::string("Show a frame overlay"));
	this->SetCategory(std::string("Safe Frames"));
}


//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
//virtual
fgtCommandShowFrame::~fgtCommandShowFrame()
{
}

