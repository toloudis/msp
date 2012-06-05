/****************************************************************************\
**	keyfKeyCreator.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/Keyframing/keyfKeyCreator.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Systems/Common/GUI/cmmAddDriverOperation.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Core/undo/undoUndoMgr.hpp"


namespace keyfKeyCreator
{
	namespace
	{
	}


	//--------------------------------------------------------------------
	//	Creates a key driver for the given channel at the current time.
	//--------------------------------------------------------------------
	bool CreateKey(tmlnScriptObject *i_pObject, 
				   tmlnChannel *i_pChannel,
				   bool i_bDoUndo)
	{
		// Create Driver
		//
		tmlnDriver* pDriver = tmlnCreator::CreateKeyForChannel(i_pObject, i_pChannel);

		// Give driver to script object to own
		if (pDriver)
		{
			// tmlnCreator function calls AddDriver
			//i_pObject->AddDriver(pDriver);
			i_pObject->NotifyDriverChanged();

			if (i_bDoUndo)
			{
				// Create an undo operation for this driver
				undoUndoMgr::AddOperation( new cmmAddDriverOperation(i_pObject->CreateReferenceToSelf(), pDriver) );
			}

			chnlDialogUtil::ObjectSelected(i_pObject);
			return true;
		}

		return false;
	}

} // end of namespace
