/*****************************************************************************
**	cmmSystemDialogUtil.hpp
**
**	API for opening dialogs for the SystemDialog
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SYSTEMDIALOGUTIL_HPP
#error cmmSystemDialogUtil.hpp multiply included
#endif
#define CMM_SYSTEMDIALOGUTIL_HPP

//#ifndef CMM_SYSTEMDIALOGINTEREST_HPP
//#include "Systems/Common/GUI/cmmSystemDialogInterest.hpp"
//#endif

#ifndef CMM_DIALOGINTEREST_HPP
#include "Systems/Common/GUI/cmmDialogInterest.hpp"
#endif
#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
//using namespace System;
//using namespace System::Windows::Forms;

class nameObject;
class cmmSystemDialogInterest;
class pick3dPickList;
class sel3dObject;


//============================================================================
//============================================================================
namespace cmmSystemDialogUtil
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Init();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CleanUp();

	//--------------------------------------------------------------------
	// ShowInitial - checks is this dialog is supposed to be visible or not
	//--------------------------------------------------------------------
	void ShowInitial();

	//--------------------------------------------------------------------
	// Show
	//--------------------------------------------------------------------
	void  Show();

	//--------------------------------------------------------------------
	//  Hide
	//--------------------------------------------------------------------
	void  Hide();

	//--------------------------------------------------------------------
	// Update the dialog
	//--------------------------------------------------------------------
	void UpdateDialog(prtyObject* i_pObject);
	void UpdateDialog();
	void UpdateDialog_SceneProperties();


	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateSceneProperties();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AddSystemCommand(const std::string& i_System, 
						  const std::string& i_DisplayText, 
						  const cmaCommand* i_pCommand );

	//------------------------------------------------------------------------
	//	find the item on the placed tree an highlight it.
	//------------------------------------------------------------------------
	void SelectObjectOnPlacedList(sel3dObject* i_pPickObject);

	//------------------------------------------------------------------------
	//	Update placed list multiple selection and highlight
	//------------------------------------------------------------------------
	void AddToSelectedObjectsOnPlacedList(sel3dObject* i_pPickObject);
	void RemoveFromSelectedObjectsOnPlacedList(sel3dObject* i_pPickObject);

	//------------------------------------------------------------------------
	// Update the placed items within the given system name
	//------------------------------------------------------------------------
	void UpdatePlacedList(const std::string& i_SystemName, cmmDialogDataList& i_DataList);

	//------------------------------------------------------------------------
	// Update the scene hierarchy tree view
	//------------------------------------------------------------------------
	void UpdateSceneHierarchy();

	//--------------------------------------------------------------------
	//  Update tree view of light sets
	//--------------------------------------------------------------------
	void  UpdateSetRelationships();

	//------------------------------------------------------------------------
	// Update the available items (todo: within the given system name)
	//------------------------------------------------------------------------
	void UpdateAvailableList();

	//------------------------------------------------------------------------
	// Operations on selected objects
	//------------------------------------------------------------------------
	void DuplicateSelected();
	void ReloadSelected();
	void DeleteSelected();

	//
	//	interest functions
	//

	//--------------------------------------------------------------------
	//	RegisterInterest() - add an interest
	//--------------------------------------------------------------------
	void RegisterInterest( cmmSystemDialogInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterInterest() - remove an interest
	//
	//	Note: this will NOT delete the  interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterInterest( cmmSystemDialogInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	Clear() - clear the interest list
	//--------------------------------------------------------------------
	void ClearInterests();

	//--------------------------------------------------------------------
	//	SceneDialogOpen - perform tasks (like adding tabs) relating
	//	to the scene/system dialog opening.  These tasks happen each time
	//	the scene dialog is launched.
	//--------------------------------------------------------------------
	void SceneDialogOpen();

	//--------------------------------------------------------------------
	//	SceneDialogClose - perform tasks (like adding tabs) relating
	//	to the scene/system dialog closing.  These tasks happen each time
	//	the scene dialog is closed.
	//--------------------------------------------------------------------
	void SceneDialogClose();


}	// end of namespace

