/****************************************************************************\
**	pick3dPickList.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003-6 - All Rights Reserved
\****************************************************************************/
#include "Tool/pick3d/pick3dPickList.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"

#include <iterator>

//----------------------------------------------------------------------------
//	constructor
//----------------------------------------------------------------------------
pick3dPickList::pick3dPickList()
{
}

//----------------------------------------------------------------------------
//	constructor
//----------------------------------------------------------------------------
pick3dPickList::~pick3dPickList()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pick3dPickList::AddItem(pick3dPickObject* i_pObject, float i_tVal)
{
	if(sel3dMgr::getSelectionLock())
		return;
	pick3dPickItem item(i_pObject, i_tVal);

	// check if the object is in the list already.
	if (!envSTLHelpers::Contains(m_List, item))
	{
		//	it wasn't in the list, so add it.
		m_List.push_back(item);
		this->sort_list();
	}
}

//--------------------------------------------------------------------
//	get a pick3dPickObject from the list
//--------------------------------------------------------------------
const pick3dPickItem* pick3dPickList::GetItem( int i_Index ) const
{
	if ( m_List.empty() )
		return 0;

	DBG_ASSERT( (i_Index >= 0) && (i_Index < m_List.size()), "index out of range for pick list item" );
	return &(m_List[ i_Index ]);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//std::vector<pick3dPickItem>& pick3dPickList::GetList()
//{
//	return m_List;
//}

//--------------------------------------------------------------------
//	CopyFrom() - copy FROM the list passed in into this list.
//--------------------------------------------------------------------
void pick3dPickList::CopyFrom( pick3dPickList& i_PickList )
{
	m_List = i_PickList.m_List;
}

//--------------------------------------------------------------------
//	AppendFrom() - Append FROM the list passed in into this list.
//--------------------------------------------------------------------
void pick3dPickList::AppendFrom( pick3dPickList& i_PickList )
{
	std::copy( i_PickList.m_List.begin(), i_PickList.m_List.end(), std::back_inserter(m_List)  );
	this->sort_list();
}

//----------------------------------------------------------------------------
//	Clear() - clear the list
//----------------------------------------------------------------------------
void pick3dPickList::Clear()
{
	m_List.clear();
}


//----------------------------------------------------------------------------
//	GetSize() - get the number of items in the list
//----------------------------------------------------------------------------
int pick3dPickList::GetSize() const
{
	return m_List.size();
}

//--------------------------------------------------------------------
// Return the object that was picked (is first in the list).
// Will return NULL is the list is empty.
//--------------------------------------------------------------------
pick3dPickObject* pick3dPickList::GetFirstPickObject()
{
	if (m_List.empty())
		return NULL; 
	return m_List[0].GetObject();
}

//--------------------------------------------------------------------
// keep the list sorted so that the closest item is in the front 
//	of the list.
//--------------------------------------------------------------------
void pick3dPickList::sort_list()
{
	std::sort(m_List.begin(), m_List.end());
}


