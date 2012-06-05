/*****************************************************************************
**	fcmdModeNewMovie.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcmd/fcmdModeNewMovie.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "FCSupport/anim/qtGUI/animPixmapItem.hpp"
#include "FCSupport/fcui/qtGUI/fcuiLabel.hpp"
#include "FCSupport/fcmd/fcmdModeMgr.hpp"
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include "FCSupport/path/pathDirectoryParser.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"

#include <vector>

//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------
fcmdModeNewMovie::fcmdModeNewMovie()
: fcmdModeTemplate(fsLocator())
{
	DBG_TRACE("Created New Movie Mode");
}
fcmdModeNewMovie::fcmdModeNewMovie(const fsLocator& i_Directory)
: fcmdModeTemplate(i_Directory)
{
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
fcmdModeNewMovie::~fcmdModeNewMovie()
{}

//----------------------------------------------------------------------------
// Activate the Make mode and any other systems associated with it
//----------------------------------------------------------------------------
void fcmdModeNewMovie::Activate()
{
	DBG_TRACE("Home - New Movie Mode Activated");
}

//----------------------------------------------------------------------------
// Deactivate the Make mode and clean up any systems
//----------------------------------------------------------------------------
void fcmdModeNewMovie::DeActivate()
{
	DBG_TRACE("Home - New Movie Mode DeActivated");
}

