/*****************************************************************************\
**	cmmDialogInterestMgr.hpp
**
**		Provides method for getting object throughout all systems into common
**	dialogs.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_DIALOGINTEREST_MGR_HPP
#error cmmDialogInterestMgr.hpp multiply included
#endif
#define CMM_DIALOGINTEREST_MGR_HPP

#ifndef CMM_DIALOGINTEREST_HPP
#include "Systems/Common/GUI/cmmDialogInterest.hpp"
#endif
#ifndef FSYS_FILELIST_HPP
#include "Support/fsys/fsysFileList.hpp"
#endif

#include <vector>


//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
namespace cmmDialogInterestMgr
{

	void Init();

	//--------------------------------------------------------------------
	//	RegisterInterest() - add a DialogInterest to the system
	//--------------------------------------------------------------------
	void RegisterInterest( cmmDialogInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterInterest() - remove a DialogInterest from the system.
	//
	//	Note: this will NOT delete the DialogInterest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterInterest( cmmDialogInterest* i_pInterest );

	//--------------------------------------------------------------------
	// Return true if a system exists with the given name.
	//--------------------------------------------------------------------
	bool SystemExists(const std::string& i_SystemName);

	//--------------------------------------------------------------------
	//	Add Object to Placed
	//--------------------------------------------------------------------
	void AddObject(const itString& i_FileName, const fsLocator& i_Path);

	//--------------------------------------------------------------------
	//	Replace Selected Object with this asset path from available
	//--------------------------------------------------------------------
	void ReplaceObject(const itString& i_FileName, const fsLocator& i_Path);

	//--------------------------------------------------------------------
	//	Delete Object from Placed
	//--------------------------------------------------------------------
	void DeleteObject(const nameString& i_Name, const std::string& i_SystemName);

	//--------------------------------------------------------------------
	//	Duplicate Object from Placed, returns name of duplicate.
	//--------------------------------------------------------------------
	nameString DuplicateObject(const nameString& i_Name, 
							   const std::string& i_SystemName,
							   std::map<nameString, nameString> &o_DuplicateNameMap);

	//--------------------------------------------------------------------
	//	Second part of duplication step, remap all internal name
	//	attachments so that the new objects are attached to each other
	//  and not to the original objects anymore.
	//--------------------------------------------------------------------
	void RemapNames(const nameString& i_Name, 
				    const std::string& i_SystemName,
				    const std::map<nameString, nameString> &i_DuplicateNameMap);

	//--------------------------------------------------------------------
	//	Reload Object from Placed
	//--------------------------------------------------------------------
	void ReloadObject(const nameString& i_Name, const std::string& i_SystemName);

	//--------------------------------------------------------------------
	//	Reload Object using the model name
	//--------------------------------------------------------------------
	void ReloadObjectByName(const fsLocator& i_Path, const std::string i_SystemName);

	//--------------------------------------------------------------------
	//	Select Object from Placed
	//--------------------------------------------------------------------
	void SelectObject(const nameString& i_Name, 
					  const std::string& i_SystemName, 
					  bool i_bAppendSelection = false);

	//--------------------------------------------------------------------
	//	Select Object from other places.
	//
	//	Note: This could return incorrect results if more than one
	//	object have the same name!
	//--------------------------------------------------------------------
	void SelectObject(const nameString& i_Name, 
					  bool i_bAppendSelection = false);

	//--------------------------------------------------------------------
	//	Remove object from selection list through Placed list
	//--------------------------------------------------------------------
	void DeselectObject(const nameString& i_Name, 
						const std::string& i_SystemName);

	//--------------------------------------------------------------------
	//	Activate object from a double-click in the Placed list
	//--------------------------------------------------------------------
	void ActivateObject(const nameString& i_Name, 
						const std::string& i_SystemName);

	//--------------------------------------------------------------------
	//	ReceiveDrag represents a drag and drop of the
	//		selected items onto the given item in the tree view in 
	//		in the scene manager.
	//--------------------------------------------------------------------
	void ReceiveDrag(const nameString& i_Name, 
					 const std::string& i_SystemName);

	//--------------------------------------------------------------------
	//	Select Object Part from Placed
	//--------------------------------------------------------------------
	void SelectObjectPart(const nameString& i_Name, 
					  const std::string& i_SystemName, 
					  const std::string& i_PartName, 
					  const std::string& i_CategoryName, 
					  cmmDialogInterest::SelectionMode i_Mode = cmmDialogInterest::e_Select);

	//--------------------------------------------------------------------
	//	ActivateObjectPart from Placed, represents a double-click
	//		on a part item in the placed menu.
	//--------------------------------------------------------------------
	void ActivateObjectPart(const nameString& i_Name, 
							const std::string& i_SystemName, 
							const std::string& i_PartName, 
							const std::string& i_CategoryName);

	//--------------------------------------------------------------------
	//	DeleteObjectPart from Placed, represents DELETE key or button
	//		when a part item is selected in the placed menu.
	//--------------------------------------------------------------------
	void DeleteObjectPart(const nameString& i_Name, 
						  const std::string& i_SystemName, 
						  const std::string& i_PartName, 
						  const std::string& i_CategoryName);

	//--------------------------------------------------------------------
	//	GetPlacedObjects - Get Placed objects
	//--------------------------------------------------------------------
	void GetPlacedObjects(cmmDialogDataList& io_DataList);

	//--------------------------------------------------------------------
	//	GetAvailableObjects - Get available objects
	//--------------------------------------------------------------------
	void GetAvailableObjects(fsysFileList& io_FileList);

	//--------------------------------------------------------------------
	//	GetAllowedInteractions - returns a set of bits for which
	//	interactions are allowed (edit, delete, etc.)
	//--------------------------------------------------------------------
	int GetAllowedInteractions(const fsLocator& i_Path);

	//--------------------------------------------------------------------
	//	SortList
	//--------------------------------------------------------------------
	void SortList(cmmDialogDataList& io_DataList);

	//--------------------------------------------------------------------
	//	update the list with a given system name
	//--------------------------------------------------------------------
	void UpdateList(const std::string& i_SystemName, 
					cmmDialogDataList& i_DataList, 
					cmmDialogDataList& o_DataList);
};
