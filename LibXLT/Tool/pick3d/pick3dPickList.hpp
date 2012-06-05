/****************************************************************************\
**	pick3dPickList.hpp
**
**		Pick list is a sorted list of item that intersect a pick ray.
**
**	StudioGPU
**	Copyright(C) 2003-6 - All Rights Reserved
\****************************************************************************/
#ifdef PICK3D_PICKLIST_HPP
#error pick3dPickList.hpp multiply included
#endif
#define PICK3D_PICKLIST_HPP

#ifndef PICK3D_PICKITEM_HPP
#include "Tool/pick3d/pick3dPickItem.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class pick3dPickList
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pick3dPickList();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~pick3dPickList();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void AddItem(pick3dPickObject* i_pObject, float i_tVal);

		//--------------------------------------------------------------------
		//	get a pick3dPickObject from the list
		//--------------------------------------------------------------------
		const pick3dPickItem* GetItem( int i_Index ) const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//std::vector<pick3dPickItem>& GetList();

		//--------------------------------------------------------------------
		//	CopyFrom() - copy FROM the list passed in into this list.
		//--------------------------------------------------------------------
		void CopyFrom( pick3dPickList& i_PickList );

		//--------------------------------------------------------------------
		//	AppendFrom() - Append FROM the list passed in into this list.
		//--------------------------------------------------------------------
		void AppendFrom( pick3dPickList& i_PickList );

		//--------------------------------------------------------------------
		//	Clear() - clear the list
		//--------------------------------------------------------------------
		void Clear();

		//--------------------------------------------------------------------
		//	GetSize() - get the number of items in the list
		//--------------------------------------------------------------------
		int GetSize() const;

		//--------------------------------------------------------------------
		// Return the object that was picked (is first in the list).
		// Will return NULL is the list is empty.
		//--------------------------------------------------------------------
		pick3dPickObject* GetFirstPickObject();

	private:
		//--------------------------------------------------------------------
		// keep the list sorted so that the closest item is in the front 
		//	of the list.
		//--------------------------------------------------------------------
		void sort_list();

		std::vector<pick3dPickItem>	m_List;
};
