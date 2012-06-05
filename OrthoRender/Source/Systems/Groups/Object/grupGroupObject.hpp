/*****************************************************************************
**  grupGroupObject.hpp
**
**      A grupGroupObject is the property object for a group
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef GRUP_GROUPOBJECT_HPP
#error grupGroupObject.hpp multiply included
#endif
#define GRUP_GROUPOBJECT_HPP

#ifndef GRUP_DATA_HPP
#include "Systems/Groups/Data/grupData.hpp"
#endif
#ifndef GRPS_GROUPMGR_HPP
#include "Support/grps/grpsGroupMgr.hpp"
#endif 

#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif

#include <vector>

//============================================================================
//	forward references
//============================================================================
class fsResourceTrackerData;
class nameString;

//============================================================================
//============================================================================
class grupGroupObject : 
	public nameObject, 
	public prtyObject,
	public pick3dPickObject
{
public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		grupGroupObject(const grupData& i_Data,
						const nameString& i_Name);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~grupGroupObject();

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetPick3dName() const;

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		virtual pick3dPickObject* GetParentObject() const;
		void SetParentObject(pick3dPickObject* i_pPO);

		//--------------------------------------------------------------------
		//  get a list of resources.  the resources will be appended to the
		//	passed in list.
		//--------------------------------------------------------------------
		virtual void GetResourceList( fsResourceTrackerData& io_List );

	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a group data structure
		//--------------------------------------------------------------------
		const grupData& GetData() const;

		//--------------------------------------------------------------------
		// Set from group data structure
		//--------------------------------------------------------------------
		void SetData(const grupData &i_Data);


	//============================================================================
	//	properties
	//============================================================================

		//--------------------------------------------------------------------
		// Name property access
		//--------------------------------------------------------------------
		prtyName&	PropertyName();
		const prtyName&	GetPropertyName() const;


	//============================================================================
	//	3D icon/geometry
	//============================================================================

		//--------------------------------------------------------------------
		// SetName
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);

	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);

		mutable grupData	m_Data;
		grpsGroupHandle		m_pGroup;

		pick3dPickObject*	m_pParent;
};


