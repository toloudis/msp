/*****************************************************************************
**	mainOperations.cpp
**
**	Utility for doing operations that are undoable in MainApp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/Commands/mainOperations.hpp"

#include "Systems/Cameras/Object/cmraScriptObject.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Undo/cmraOperations.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#include "Systems/Character/Undo/chtrOperations.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"

#include "Core/gf/gfDirectoryCategories.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"


namespace mainOperations
{
	namespace
	{
		const char* c_MultipleLoadOperationName = "Load Animations";
		const char* c_AnimationDirCategory = "Animation";
		
		//--------------------------------------------------------------------
		// browse_for_anims - allow multiple file selection to 
		// load character animation
		//--------------------------------------------------------------------
		bool browse_for_object_anims(std::vector<fsLocator> &o_ChosenLocators) 
		{
			std::string fileFilter = "Animation Files (*.gab)|*.gab|All files (*.*)|*.*";

			// Use the animation directory category here
			return (guiFileDialogUtils::GetOpenFileNames(fileFilter, c_AnimationDirCategory, o_ChosenLocators));
		}		
		//--------------------------------------------------------------------
		// browse_for_camera_anims - allow multiple file selection to 
		// load camera animation
		//--------------------------------------------------------------------
		bool browse_for_camera_anims(std::vector<fsLocator> &o_ChosenLocators) 
		{
			std::string fileFilter = "Camera Animation Files (*.cam)|*.cam|All files (*.*)|*.*";

			// Use the animation directory category here
			return (guiFileDialogUtils::GetOpenFileNames(fileFilter, c_AnimationDirCategory, o_ChosenLocators));
		}
	}

	//------------------------------------------------------------------------
	// Let user search for animation to load.
	//------------------------------------------------------------------------
	void BrowseLoadAnimation()
	{
		bool bNeedsWarning = true;
		if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetSelectedScriptObject())
		{
			if ( chtrScriptObject *pCharacter = dynamic_cast<chtrScriptObject*>(pScriptObject) )
			{
				bNeedsWarning = false;

				// Got a character
				int char_index = chtrObjectMgr::GetIndexForObject(pCharacter->GetPickObject());
				std::vector<fsLocator> browseFileNames;
				if( browse_for_object_anims( browseFileNames ) )
				{
					const int num_files = browseFileNames.size();
					if (num_files > 0)
						undoUndoMgr::BeginMultipleOperationBlock(c_MultipleLoadOperationName);
					for (int i=0; i<num_files; i++)
					{
						chtrOperations::LoadAnimation( char_index, browseFileNames[i] );
					}
					if (num_files > 0)
						undoUndoMgr::EndMultipleOperationBlock();
				}
			}
			else if ( cmraScriptObject *pCamera = dynamic_cast<cmraScriptObject*>(pScriptObject) )
			{
				bNeedsWarning = false;

				// Got a camera
				int cam_index = cmraObjectMgr::GetIndexForObject(pCamera->GetPickObject());
				std::vector<fsLocator> browseFileNames;
				if( browse_for_camera_anims( browseFileNames ) )
				{
					const int num_files = browseFileNames.size();
					if (num_files > 0)
						undoUndoMgr::BeginMultipleOperationBlock(c_MultipleLoadOperationName);
					for (int i=0; i<num_files; i++)
					{
						cmraOperations::LoadAnimation( cam_index, browseFileNames[i] );
					}
					if (num_files > 0)
						undoUndoMgr::EndMultipleOperationBlock();
				}
			}
		}

		// If we didnt have the correct type of object selected, tell the user
		if (bNeedsWarning)
		{
			guiMessageBox::Show("Please select either an object or camera in order to load animation.", "Need selection for animation");
		}
	}

}	// end of namespace
