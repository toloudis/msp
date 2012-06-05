/*****************************************************************************
**  lsetScriptObject.hpp
**
**      A lsetScriptObject is a derived class for an LightSet
**
**	Extra Large Technology
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

#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif
#ifndef TMLN_SCRIPTOBJECT_HPP
#include "Support/tmln/tmlnScriptObject.hpp"
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
class lsetScriptObject : public pick3dPickObject,
							public tmlnScriptObject
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
		virtual std::string GetTmlnName() const;


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

		//--------------------------------------------------------------------
		// Accessors to channels
		//--------------------------------------------------------------------
		tmlnChannelColor& AmbientLightChannel();

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

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//void GetObjectsInLightSet(std::vector<nameString>& o_Names) const;
		//bool HasObject(const nameString& i_Name) const;
		//void Add(const nameString& i_ObjectName);
		//void Remove(const nameString& i_ObjectName);
		//void RefreshNames();

	private:
		lsetLightSetObject*		m_pIcon;	// Manipulation object for the compass

		// Timeline related
		tmlnChannelColor* m_pAmbientLightChannel;
};

