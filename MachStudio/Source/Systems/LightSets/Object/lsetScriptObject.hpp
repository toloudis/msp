/*****************************************************************************
**  lsetScriptObject.hpp
**
**      A lsetScriptObject is a derived class for an LightSet
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef LSET_SCRIPTOBJECT_HPP
#error lsetScriptObject.hpp multiply included
#endif
#define LSET_SCRIPTOBJECT_HPP

#ifndef LSET_DATA_HPP
#include "Systems/LightSets/Data/lsetData.hpp"
#endif
#ifndef LSET_LIGHTSETOBJECT_HPP
#include "Systems/LightSets/Object/lsetLightSetObject.hpp"
#endif
#ifndef LSET_SCRIPTDATA_HPP
#include "Systems/LightSets/Data/lsetScriptData.hpp"
#endif

#ifndef CMM_SCRIPTOBJECT_HPP
#include "Systems/Common/Object/cmmScriptObject.hpp"
#endif 
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <string>


//============================================================================
//	forward references
//============================================================================
class tmlnChannelColor;


//============================================================================
//============================================================================
class lsetScriptObject : public cmmScriptObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		lsetScriptObject(const lsetScriptData &i_Data,
								   const nameString& i_Name);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~lsetScriptObject();

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetDisplayName() const;

		//--------------------------------------------------------------------
		//	CreateReferenceToSelf - create a reference that refers to the
		//	given object. This reference should be able to refer to 
		//  future instances of this object persistent through deletion
		//	and recreation through undo.
		//--------------------------------------------------------------------
		virtual shared_ptr<relObjectReference> CreateReferenceToSelf();


	//========================================================================
	//	Data
	//========================================================================

		//--------------------------------------------------------------------
		// Get values as a light data structure
		//--------------------------------------------------------------------
		lsetScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from light data structure
		//--------------------------------------------------------------------
		void SetScriptData(const lsetScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a light base data structure
		//--------------------------------------------------------------------
		lsetData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from light base data structure
		//--------------------------------------------------------------------
		void SetBaseData(const lsetData &i_Data);

	//========================================================================
	//	Timeline
	//========================================================================

	//========================================================================
	//	
	//========================================================================

		//--------------------------------------------------------------------
		//	Name - passes functions on to pick object
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);
		const nameString& GetName() const;

		//--------------------------------------------------------------------
		// This is called when a driver in this script object is changed,
		//	or if a driver is added or deleted from this object.
		//--------------------------------------------------------------------
		virtual void NotifyDriverChanged();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		lsetLightSetObject*	GetPickObject() const;

	private:
		lsetLightSetObject*		m_pIcon;	// Manipulation object for the compass
};

