/****************************************************************************\
**	cmmDialogInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/cmmDialogInterest.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmDialogPartData::cmmDialogPartData()
: m_pPickObject(NULL)
{}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmDialogPartData::cmmDialogPartData(const std::string& i_Name, sel3dObject* i_pPickObject)
: m_Name(i_Name), m_pPickObject(i_pPickObject)
{}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmDialogData::cmmDialogData()
: m_pPickObject(NULL), m_bVisible(true), m_bEnabled(true), m_GroupState(e_NotGroupable)
{}

//--------------------------------------------------------------------
// SystemName is used to make sure that the operations below
//	are called on the correct system.
//--------------------------------------------------------------------
cmmDialogInterest::cmmDialogInterest(const char* i_SystemName)
:	m_SystemName(i_SystemName)
{

}
cmmDialogInterest::cmmDialogInterest(const std::string& i_SystemName)
:	m_SystemName(i_SystemName)
{

}

//--------------------------------------------------------------------
// Returns SystemName
//--------------------------------------------------------------------
const std::string& cmmDialogInterest::GetSystemName() const
{
	return m_SystemName;
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
nameString cmmDialogInterest::DuplicateObject(const nameString& i_Name)
{
	// Default implementation returns empty name
	return nameString();
}

//--------------------------------------------------------------------
//	Duplicate Object from Placed and return map of all 
//  names that were created in duplication. Will call the
//  simpler DuplicateObject function by default.
//--------------------------------------------------------------------
nameString cmmDialogInterest::DuplicateObject(const nameString& i_Name,
											  std::map<nameString, nameString> &o_DuplicateNameMap)
{
	// Call simpler function which is enough for most systems.
	// Only the parent transform system creates a hierarchy with multiple
	// duplicates so far.
	nameString dup_name = this->DuplicateObject(i_Name);
	if (!dup_name.IsEmpty())
		o_DuplicateNameMap[i_Name] = dup_name;
	return dup_name;
}

//--------------------------------------------------------------------
//	Second part of duplication step, remap all internal name
//	attachments so that the new objects are attached to each other
//  and not to the original objects anymore.
//--------------------------------------------------------------------
void cmmDialogInterest::RemapNames(const nameString& i_Name, 
								   const std::map<nameString, nameString> &i_DuplicateNameMap)
{
	// Default implementation does nothing
}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
void cmmDialogInterest::ReloadObject(const nameString& i_Name)
{
	// Default implementation does nothing
}
