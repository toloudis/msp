/****************************************************************************\
**	pick3dPickItem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/pick3d/pick3dPickItem.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pick3dPickItem::pick3dPickItem()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pick3dPickItem::pick3dPickItem(pick3dPickObject* i_pObject, float i_tVal)
: m_pObject(i_pObject), m_tVal(i_tVal)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pick3dPickItem::~pick3dPickItem()
{
}

//--------------------------------------------------------------------
// operator== checks if the object pointers match
//--------------------------------------------------------------------
bool pick3dPickItem::operator == (const pick3dPickItem& i_Item) const
{
	return (m_pObject == i_Item.m_pObject);
}

//--------------------------------------------------------------------
// operator< compares t-values
//--------------------------------------------------------------------
bool operator<( const pick3dPickItem& a, const pick3dPickItem& b )
{
	return (a.GetTVal() < b.GetTVal());
}
