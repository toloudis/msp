/****************************************************************************\
**	cmmDialogInterestMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"

//	library
#include "Core/env/envSTLHelpers.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/fs/private/fsFileNotifyMgr.hpp"
#include "Core/fs/fsResourceTracker.hpp"


#include <map>


//============================================================================
//============================================================================
namespace
{
	std::map<std::string, cmmDialogInterest*>	l_DialogInterestMap;
	fsResourceTrackerInterest* rsrInterest;
}


void cmmDialogInterestMgr::Init()
{
	rsrInterest = fsResourceTracker::GetInterestList();
	rsrInterest->ObjectReloadFunction(&cmmDialogInterestMgr::ReloadObjectByName);
	rsrInterest->ObjectAddFunction(&cmmDialogInterestMgr::AddObject);
}

//--------------------------------------------------------------------
//	RegisterInterest() - add a DialogInterest to the system
//--------------------------------------------------------------------
void cmmDialogInterestMgr::RegisterInterest( cmmDialogInterest* i_pInterest )
{
	DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL DialogInterest" );

	const std::string system_name = i_pInterest->GetSystemName();
	DBG_ASSERT( l_DialogInterestMap.find(system_name) == l_DialogInterestMap.end(), "System name is already used: " << system_name.c_str());
	l_DialogInterestMap[system_name] = i_pInterest;
}

//--------------------------------------------------------------------
//	UnRegisterInterest() - remove a DialogInterest from the system.
//
//	Note: this will NOT delete the DialogInterest.  It is 
//	up to the registerer.
//--------------------------------------------------------------------
void cmmDialogInterestMgr::UnRegisterInterest( cmmDialogInterest* i_pInterest )
{
	//envSTLHelpers::RemoveOneValue( l_DialogInterestMap, i_pInterest );
	
	std::map<std::string, cmmDialogInterest*>::iterator it, end = l_DialogInterestMap.end();
	for ( it = l_DialogInterestMap.begin(); it != end; ++it )
	{
		if (it->second == i_pInterest)
		{
			l_DialogInterestMap.erase( it );
			return;
		}
	}
}

//--------------------------------------------------------------------
// Return true if a system exists with the given name.
//--------------------------------------------------------------------
bool cmmDialogInterestMgr::SystemExists(const std::string& i_SystemName)
{
	std::map<std::string, cmmDialogInterest*>::iterator it = l_DialogInterestMap.find(i_SystemName);
	return (it != l_DialogInterestMap.end());
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
void cmmDialogInterestMgr::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{
	DBG_ASSERT( i_Path.GetNumNames() > 0, "Empty path" );
	std::string system_name = itStringUtil::GetStdString(i_Path.GetName(0));
	std::map<std::string, cmmDialogInterest*>::iterator it = l_DialogInterestMap.find(system_name);
	DBG_ASSERT(it != l_DialogInterestMap.end(), "Cannot find system with name: " << system_name.c_str());
	it->second->AddObject(i_FileName, i_Path);
}

//--------------------------------------------------------------------
//	Replace Selected Object with this asset path from available
//--------------------------------------------------------------------
void cmmDialogInterestMgr::ReplaceObject(const itString& i_FileName, const fsLocator& i_Path)
{
	DBG_ASSERT( i_Path.GetNumNames() > 0, "Empty path" );
	std::string system_name = itStringUtil::GetStdString(i_Path.GetName(0));
	std::map<std::string, cmmDialogInterest*>::iterator it = l_DialogInterestMap.find(system_name);
	DBG_ASSERT(it != l_DialogInterestMap.end(), "Cannot find system with name: " << system_name.c_str());
	it->second->ReplaceObject(i_FileName, i_Path);
}

//--------------------------------------------------------------------
//	Delete Object from Placed
//--------------------------------------------------------------------
void cmmDialogInterestMgr::DeleteObject(const nameString& i_Name, const std::string& i_SystemName)
{
	std::map<std::string, cmmDialogInterest*>::iterator it = l_DialogInterestMap.find(i_SystemName);
	DBG_ASSERT(it != l_DialogInterestMap.end(), "Cannot find system with name: " << i_SystemName.c_str());
	it->second->DeleteObject(i_Name);
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
nameString cmmDialogInterestMgr::DuplicateObject(const nameString& i_Name, 
												 const std::string& i_SystemName,
												 std::map<nameString, nameString> &o_DuplicateNameMap)
{
	std::map<std::string, cmmDialogInterest*>::iterator it = l_DialogInterestMap.find(i_SystemName);
	DBG_ASSERT(it != l_DialogInterestMap.end(), "Cannot find system with name: " << i_SystemName.c_str());
	return it->second->DuplicateObject(i_Name, o_DuplicateNameMap);
}

//--------------------------------------------------------------------
//	Second part of duplication step, remap all internal name
//	attachments so that the new objects are attached to each other
//  and not to the original objects anymore.
//--------------------------------------------------------------------
void cmmDialogInterestMgr::RemapNames(const nameString& i_Name, 
									  const std::string& i_SystemName,
									  const std::map<nameString, nameString> &i_DuplicateNameMap)
{
	std::map<std::string, cmmDialogInterest*>::iterator it = l_DialogInterestMap.find(i_SystemName);
	DBG_ASSERT(it != l_DialogInterestMap.end(), "Cannot find system with name: " << i_SystemName.c_str());
	return it->second->RemapNames(i_Name, i_DuplicateNameMap);
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
void cmmDialogInterestMgr::ReloadObject(const nameString& i_Name, const std::string& i_SystemName)
{
	std::map<std::string, cmmDialogInterest*>::iterator it = l_DialogInterestMap.find(i_SystemName);
	DBG_ASSERT(it != l_DialogInterestMap.end(), "Cannot find system with name: " << i_SystemName.c_str());
	it->second->ReloadObject(i_Name);
}

//--------------------------------------------------------------------
//	Reload Object using model name
//--------------------------------------------------------------------
void cmmDialogInterestMgr::ReloadObjectByName(const fsLocator& i_Path, const std::string i_SystemName)
{
	cmmDialogDataList data_list;
	cmmDialogInterestMgr::GetPlacedObjects(data_list);
	cmmDialogDataList::iterator it;
	for (it = data_list.begin(); it != data_list.end(); ++it)
	{
		cmmDialogData &data = (*it);
		if (data.m_Filename == i_Path.GetLastName() )
		{
			ReloadObject(data.m_Name, i_SystemName );
		}
	}
}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
void cmmDialogInterestMgr::SelectObject(const nameString& i_Name, 
										const std::string& i_SystemName, 
										bool i_bAppendSelection)
{
	std::map<std::string, cmmDialogInterest*>::iterator it = l_DialogInterestMap.find(i_SystemName);
	DBG_ASSERT(it != l_DialogInterestMap.end(), "Cannot find system with name: " << i_SystemName.c_str());
	it->second->SelectObject(i_Name, i_bAppendSelection);
}

//--------------------------------------------------------------------
//	Select Object from other places.
//
//	Note: This could return incorrect results if more than one
//	object have the same name!
//--------------------------------------------------------------------
void cmmDialogInterestMgr::SelectObject(const nameString& i_Name, 
										bool i_bAppendSelection )
{
	//	since the system wasn't given, go through and ask all the systems to
	//	try and select this object.
	std::map<std::string, cmmDialogInterest*>::iterator it, end = l_DialogInterestMap.end();
	for (it = l_DialogInterestMap.begin(); it != end; ++it)
	{
		it->second->SelectObject(i_Name, i_bAppendSelection);
	}
}

//--------------------------------------------------------------------
//	Remove object from selection list through Placed list
//--------------------------------------------------------------------
void cmmDialogInterestMgr::DeselectObject(const nameString& i_Name, 
										const std::string& i_SystemName)
{
	std::map<std::string, cmmDialogInterest*>::iterator it = l_DialogInterestMap.find(i_SystemName);
	DBG_ASSERT(it != l_DialogInterestMap.end(), "Cannot find system with name: " << i_SystemName.c_str());
	it->second->DeselectObject(i_Name);
}

//--------------------------------------------------------------------
//	Activate object from a double-click in the Placed list
//--------------------------------------------------------------------
void cmmDialogInterestMgr::ActivateObject(const nameString& i_Name, 
										  const std::string& i_SystemName)
{
	std::map<std::string, cmmDialogInterest*>::iterator it = l_DialogInterestMap.find(i_SystemName);
	DBG_ASSERT(it != l_DialogInterestMap.end(), "Cannot find system with name: " << i_SystemName.c_str());
	it->second->ActivateObject(i_Name);
}

//--------------------------------------------------------------------
//	ReceiveDrag represents a drag and drop of the
//		selected items onto the given item in the tree view in 
//		in the scene manager.
//--------------------------------------------------------------------
void cmmDialogInterestMgr::ReceiveDrag(const nameString& i_Name, 
									   const std::string& i_SystemName)
{
	std::map<std::string, cmmDialogInterest*>::iterator it = l_DialogInterestMap.find(i_SystemName);
	DBG_ASSERT(it != l_DialogInterestMap.end(), "Cannot find system with name: " << i_SystemName.c_str());	
	if (it != l_DialogInterestMap.end())
	{
		it->second->ReceiveDrag(i_Name);
	}
}

//--------------------------------------------------------------------
//	Select Object Part from Placed
//--------------------------------------------------------------------
void cmmDialogInterestMgr::SelectObjectPart(const nameString& i_Name, 
										  const std::string& i_SystemName, 
										  const std::string& i_PartName, 
										  const std::string& i_CategoryName, 
										  cmmDialogInterest::SelectionMode i_Mode)
{
	std::map<std::string, cmmDialogInterest*>::iterator it = l_DialogInterestMap.find(i_SystemName);
	DBG_ASSERT(it != l_DialogInterestMap.end(), "Cannot find system with name: " << i_SystemName.c_str());
	it->second->SelectObjectPart(i_Name, i_PartName, i_CategoryName, i_Mode);
}

//--------------------------------------------------------------------
//	ActivateObjectPart from Placed, represents a double-click
//		on a part item in the placed menu.
//--------------------------------------------------------------------
void cmmDialogInterestMgr::ActivateObjectPart(const nameString& i_Name, 
						const std::string& i_SystemName, 
						const std::string& i_PartName, 
						const std::string& i_CategoryName)
{
	std::map<std::string, cmmDialogInterest*>::iterator it = l_DialogInterestMap.find(i_SystemName);
	DBG_ASSERT(it != l_DialogInterestMap.end(), "Cannot find system with name: " << i_SystemName.c_str());
	it->second->ActivateObjectPart(i_Name, i_PartName, i_CategoryName);
}

//--------------------------------------------------------------------
//	DeleteObjectPart from Placed, represents DELETE key or button
//		when a part item is selected in the placed menu.
//--------------------------------------------------------------------
void cmmDialogInterestMgr::DeleteObjectPart(const nameString& i_Name, 
					  const std::string& i_SystemName, 
					  const std::string& i_PartName, 
					  const std::string& i_CategoryName)
{
	std::map<std::string, cmmDialogInterest*>::iterator it = l_DialogInterestMap.find(i_SystemName);
	DBG_ASSERT(it != l_DialogInterestMap.end(), "Cannot find system with name: " << i_SystemName.c_str());
	it->second->DeleteObjectPart(i_Name, i_PartName, i_CategoryName);
}


//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
void cmmDialogInterestMgr::GetPlacedObjects(cmmDialogDataList& io_DataList)
{
	std::map<std::string, cmmDialogInterest*>::iterator it, end = l_DialogInterestMap.end();
	for (it = l_DialogInterestMap.begin(); it != end; ++it)
	{
		it->second->GetPlacedObjects(io_DataList);
	}
}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
void cmmDialogInterestMgr::GetAvailableObjects(fsysFileList& io_FileList)
{
	io_FileList.Clear();

	std::map<std::string, cmmDialogInterest*>::iterator it, end = l_DialogInterestMap.end();
	for (it = l_DialogInterestMap.begin(); it != end; ++it)
	{
		it->second->GetAvailableObjects(io_FileList);
	}
}

//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
int cmmDialogInterestMgr::GetAllowedInteractions(const fsLocator& i_Path)
{
	DBG_ASSERT( i_Path.GetNumNames() > 0, "Empty path" );
	std::string system_name = itStringUtil::GetStdString(i_Path.GetName(0));
	std::map<std::string, cmmDialogInterest*>::iterator it = l_DialogInterestMap.find(system_name);
	DBG_ASSERT(it != l_DialogInterestMap.end(), "Cannot find system with name: " << system_name.c_str());
	return it->second->GetAllowedInteractions(i_Path);
}

//----------------------------------------------------------------------------
//private:
// Return whether first element is greater than the second
//----------------------------------------------------------------------------
bool UDgreater( const cmmDialogData& elem1, const cmmDialogData& elem2 )
{
	int strcmp_system	= strcmp(elem1.m_SystemName.c_str(), elem2.m_SystemName.c_str());
	if (strcmp_system == 0)
	{
		int strcmp_name		= strcmp(elem1.m_Name.GetString().c_str(), elem2.m_Name.GetString().c_str());
		return (strcmp_name > 0);
	}
	else return (strcmp_system > 0);

	//return (   (strcmp_system > 0)
	//		|| ((strcmp_system == 0) && (strcmp_name > 0)));
}

//--------------------------------------------------------------------
//	SortList
//--------------------------------------------------------------------
void cmmDialogInterestMgr::SortList(cmmDialogDataList& io_DataList)
{
	sort(io_DataList.begin(), io_DataList.end(), UDgreater );
}

//--------------------------------------------------------------------
//	update the list with a given system name
//--------------------------------------------------------------------
void cmmDialogInterestMgr::UpdateList(const std::string& i_SystemName, cmmDialogDataList& i_DataList, cmmDialogDataList& o_DataList)
{
	// TODO this function needs to be improved when adding/removing items.

	bool bFoundItems = false;
	cmmDialogDataList::iterator it;

	//DBG_LOG2("-----UPDATEPLACEDLIST - (%s) %d items", i_SystemName.c_str(), i_DataList.size());

	//	find the range of items for this system.
	//
	int count = 0;
	int start = 0;
	int end = -1;
	it = o_DataList.begin();
	while (it != o_DataList.end())
	{
		//DBG_LOG3("   %02d) system (%s) vs (%s)", count, (*it).m_SystemName.c_str(), i_SystemName.c_str());

		if (strcmp((*it).m_SystemName.c_str(),i_SystemName.c_str()) == 0)
		{
			if (!bFoundItems)
			{
				bFoundItems = true;
				start = count;
			}
		}
		else
		{
			if (bFoundItems)
			{
				end = count;
				break;
			}
		}

		++it;
		++count;
	}

	//	and then remove the items
	//
	if (bFoundItems)
	{
		cmmDialogDataList::iterator it_end = o_DataList.end();
		if (end != -1)
			it_end = o_DataList.begin() + end;
		o_DataList.erase( o_DataList.begin() + start, it_end );
	}
	//DBG_LOG(" after erase DataListPlaced size = " << o_DataList.size() << " (btw_s_p)" );

	// debug section
	//
	//DBG_LOG(" i_DataList Place size = " << i_DataList.size() << " (btw_s_p)" );
	//cmmDialogDataList::iterator dbgit2;
	//for(dbgit2 = i_DataList.begin(); dbgit2 != i_DataList.end(); dbgit2++)
	//{
	//	DBG_LOG2("   idata list1 (%s)-(%s)", (*dbgit2).m_SystemName.c_str(), (*dbgit2).m_Name.GetString().c_str());
	//}
	//for(dbgit2 = o_DataList->begin(); dbgit2 != o_DataList->end(); dbgit2++)
	//{
	//	DBG_LOG2("   placeddata list1 (%s)-(%s)", (*dbgit2).m_SystemName.c_str(), (*dbgit2).m_Name.GetString().c_str());
	//}

	//	add the list
	//
	//DBG_LOG(" DataList Place size = " << o_DataList->size() << " (UPL1)" );
	//DBG_LOG(" i_DataList Place size = " << i_DataList.size() << "(UPL1)" );
	o_DataList.insert(o_DataList.begin(), i_DataList.begin(), i_DataList.end());

	SortList(o_DataList);
	// debug section
	//cmmDialogDataList::iterator dbgit;
	//for(dbgit = o_DataList->begin(); dbgit != o_DataList->end(); dbgit++)
	//{
	//	DBG_LOG2("   mdata list2 (%s)-(%s)", (*dbgit).m_SystemName.c_str(), (*dbgit).m_Name.GetString().c_str());
	//}
}
