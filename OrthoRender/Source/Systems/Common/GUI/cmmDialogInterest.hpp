/****************************************************************************\
**	cmmDialogInterest.hpp
**
**		A DialogInterest is registered by a system so that the Object and
**	System dialogs can update information.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_DIALOGINTEREST_HPP
#error cmmDialogInterest.hpp multiply included
#endif
#define CMM_DIALOGINTEREST_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef	FSYS_FILELIST_HPP
#include "Support/fsys/fsysFileList.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <string>
#include <deque>
#include <map>


//============================================================================
//	Forward References
//============================================================================
class pick3dPickObject;


//============================================================================
// Data structure for a selectable part of an object
//============================================================================
class cmmDialogPartData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmmDialogPartData();
	cmmDialogPartData(const std::string& i_Name, pick3dPickObject* i_pPickObject = NULL);

public:
	std::string	m_Name;
	pick3dPickObject* m_pPickObject;
};


//============================================================================
//============================================================================
class cmmDialogData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmmDialogData();

public:
	bool		m_bVisible;
	nameString	m_Name;
	itString	m_Filename;
	std::string	m_Desc;
	std::string	m_SystemName;
	// Part names grouped by category
	std::map<std::string, std::vector<cmmDialogPartData> > m_Parts; 
	// By passing in the pick object, it is easier to 
	// determine what is selected by monitoring the selection interest
	pick3dPickObject* m_pPickObject;
};

typedef std::deque<cmmDialogData> cmmDialogDataList;


//============================================================================
//	bits for which types of interaction is possible.
//	each system sets its own.
//============================================================================
enum DialogInterestAllowances
{
	e_DIAllow_None = 0x0000,
	e_DIAllow_Add = 0x0001,
	e_DIAllow_Del = 0x0002,
	e_DIAllow_Dupe = 0x0004,
	e_DIAllow_Edit = 0x0008,
	e_DIAllow_Reload = 0x0010,
	e_DIAllow_All = 0xFFFF
};


//============================================================================
//============================================================================
class cmmDialogInterest
{
public:
	//--------------------------------------------------------------------
	// SystemName is used to make sure that the operations below
	//	are called on the correct system.
	//--------------------------------------------------------------------
	cmmDialogInterest(const char* i_SystemName);
	cmmDialogInterest(const std::string& i_SystemName);

	//--------------------------------------------------------------------
	// Returns SystemName
	//--------------------------------------------------------------------
	const std::string& GetSystemName() const;

	//--------------------------------------------------------------------
	//	Add Object to Placed
	//--------------------------------------------------------------------
	virtual void AddObject(const itString& i_FileName, const fsLocator& i_Path) = 0;

	//--------------------------------------------------------------------
	//	Replace Selected Object with this asset path from available
	//--------------------------------------------------------------------
	virtual void ReplaceObject(const itString& i_FileName, const fsLocator& i_Path) {}

	//--------------------------------------------------------------------
	//	Delete Object from Placed
	//--------------------------------------------------------------------
	virtual void DeleteObject(const nameString& i_Name ) = 0;

	//--------------------------------------------------------------------
	//	Duplicate Object from Placed
	//--------------------------------------------------------------------
	virtual void DuplicateObject(const nameString& i_Name) = 0;

	//--------------------------------------------------------------------
	//	Reload Object from Placed
	//--------------------------------------------------------------------
	virtual void ReloadObject(const nameString& i_Name) = 0;

	//--------------------------------------------------------------------
	//	Select Object from Placed
	//		If i_bAppend==true, add object to selection list.
	//--------------------------------------------------------------------
	virtual void SelectObject(const nameString& i_Name, bool i_bAppend = false) const = 0;

	//--------------------------------------------------------------------
	//	Remove Object from Selection
	//--------------------------------------------------------------------
	virtual void DeselectObject(const nameString& i_Name) const = 0;

	//--------------------------------------------------------------------
	//	ActivateObject from Placed, represents a double-click on the
	//		item in the placed menu.
	//--------------------------------------------------------------------
	virtual void ActivateObject(const nameString& i_Name) {};

	//--------------------------------------------------------------------
	//	Select Object Part from Placed
	//		If i_bAppend==true, add object to selection list.
	//--------------------------------------------------------------------
	virtual void SelectObjectPart(const nameString& i_Name, 
								  const std::string i_PartName, 
								  const std::string i_CategoryName, 
								  bool i_bAppend = false) const {};

	//--------------------------------------------------------------------
	//	ActivateObjectPart from Placed, represents a double-click
	//		on a part item in the placed menu.
	//--------------------------------------------------------------------
	virtual void ActivateObjectPart(const nameString& i_Name, 
								  const std::string i_PartName, 
								  const std::string i_CategoryName) const {};

	//--------------------------------------------------------------------
	//	DeleteObjectPart from Placed, represents DELETE key or button
	//		when a part item is selected in the placed menu.
	//--------------------------------------------------------------------
	virtual void DeleteObjectPart(const nameString& i_Name, 
								  const std::string i_PartName, 
								  const std::string i_CategoryName) const {};

	//--------------------------------------------------------------------
	//	GetPlacedObjects - Get Placed objects
	//--------------------------------------------------------------------
	virtual void GetPlacedObjects(cmmDialogDataList& io_DataList) const = 0;

	//--------------------------------------------------------------------
	//	GetAvailableObjects - Get available objects
	//--------------------------------------------------------------------
	virtual void GetAvailableObjects(fsysFileList& io_FileList) const = 0;

	//--------------------------------------------------------------------
	//	GetAllowedInteractions - returns a set of bits for which
	//	interactions are allowed (edit, delete, etc.)
	//--------------------------------------------------------------------
	virtual int GetAllowedInteractions(const fsLocator& i_Path) const = 0;

private:
	std::string m_SystemName;
};
