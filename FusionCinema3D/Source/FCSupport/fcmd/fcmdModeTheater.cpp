/*****************************************************************************
**	fcmdModeTheater.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcmd/fcmdModeTheater.hpp"

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
fcmdModeTheater::fcmdModeTheater()
: fcmdModeTemplate(fsLocator())
{
	DBG_TRACE("Created Theater Mode");
}
fcmdModeTheater::fcmdModeTheater(const fsLocator& i_Directory)
: fcmdModeTemplate(i_Directory)
{
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
fcmdModeTheater::~fcmdModeTheater()
{}

//----------------------------------------------------------------------------
// Activate the Theater mode and any other systems associated with it
//----------------------------------------------------------------------------
void fcmdModeTheater::Activate()
{
	DBG_TRACE("Theater Mode Activated");
	QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
	i_ui->ui.stackedWidget->setCurrentIndex(4);
}

//----------------------------------------------------------------------------
// Deactivate the Theater mode and clean up any systems
//----------------------------------------------------------------------------
void fcmdModeTheater::DeActivate()
{
	DBG_TRACE("Theater Mode DeActivated");
}

