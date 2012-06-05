/****************************************************************************\
**	cmmDialogInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
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
cmmDialogPartData::cmmDialogPartData(const std::string& i_Name, pick3dPickObject* i_pPickObject)
: m_Name(i_Name), m_pPickObject(i_pPickObject)
{}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmDialogData::cmmDialogData()
: m_pPickObject(NULL)
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
