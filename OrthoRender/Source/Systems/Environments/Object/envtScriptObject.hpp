/*****************************************************************************
**  envtScriptObject.hpp
**
**      A envtScriptObject is a derived class for an Environment
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef ENVT_SCRIPTOBJECT_HPP
#error envtScriptObject.hpp multiply included
#endif
#define ENVT_SCRIPTOBJECT_HPP

#ifndef ENVT_DATA_HPP
#include "Systems/Environments/Data/envtData.hpp"
#endif
#ifndef ENVT_ENVIRONMENTOBJECT_HPP
#include "Systems/Environments/Object/envtEnvironmentObject.hpp"
#endif
#ifndef ENVT_SCRIPTDATA_HPP
#include "Systems/Environments/Data/envtScriptData.hpp"
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
class tmlnChannelFloat;


//============================================================================
//============================================================================
class envtScriptObject : public pick3dPickObject,
							public tmlnScriptObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		envtScriptObject(const envtScriptData &i_Data,
								   const nameString& i_Name);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~envtScriptObject();

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetTmlnName() const;

		//--------------------------------------------------------------------
		//  get a list of resources.  the resources will be appended to the
		//	passed in list.
		//--------------------------------------------------------------------
		virtual void GetResourceList( fsResourceTrackerData& io_List );

	//========================================================================
	//	Data
	//========================================================================

		//--------------------------------------------------------------------
		// Get values as a light data structure
		//--------------------------------------------------------------------
		envtScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from light data structure
		//--------------------------------------------------------------------
		void SetScriptData(const envtScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a light base data structure
		//--------------------------------------------------------------------
		envtData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from light base data structure
		//--------------------------------------------------------------------
		void SetBaseData(const envtData &i_Data);

	//========================================================================
	//	Timeline
	//========================================================================

		//--------------------------------------------------------------------
		// Accessors to channels
		//--------------------------------------------------------------------
		tmlnChannelFloat& DiffuseFactorChannel();
		tmlnChannelFloat& DiffuseAngleChannel();
		tmlnChannelFloat& SpecularFactorChannel();
		tmlnChannelFloat& SpecularAngleChannel();

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
		envtEnvironmentObject*	GetPickObject() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void GetObjectsInEnvironment(std::vector<nameString>& o_Names) const;
		bool HasObject(const nameString& i_Name) const;
		void Add(const nameString& i_ObjectName);
		void Remove(const nameString& i_ObjectName);
		void RefreshNames();

	private:
		envtEnvironmentObject*		m_pIcon;	// Manipulation object for the compass

		// Timeline related
		tmlnChannelFloat* m_pDiffuseFactorChannel;
		tmlnChannelFloat* m_pDiffuseAngleChannel;
		tmlnChannelFloat* m_pSpecularFactorChannel;
		tmlnChannelFloat* m_pSpecularAngleChannel;
};

