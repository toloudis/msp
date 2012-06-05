/*****************************************************************************
**  rcdSystem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Record/rcdSystem.hpp"

#include "Record/Float/rcdFloatDialogUtil.hpp"
#include "Record/rcdModeRecord.hpp"
#include "Record/rcdRecordUtil.hpp"

#include "Support/mode/modeModeMgr.hpp"

namespace rcdSystem
{

	namespace
	{
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Initialize namespaces
		rcdFloatDialogUtil::Init();

		// Create mode without gui button
		modeModeID modeID = modeModeMgr::AddMode( new rcdModeRecord() );
		rcdRecordUtil::SetMode(modeID);
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		// Clean up namespaces
		rcdFloatDialogUtil::CleanUp();
	}

}	// end of namespace