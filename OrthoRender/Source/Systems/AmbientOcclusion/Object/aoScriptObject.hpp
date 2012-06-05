/*****************************************************************************
**  aoScriptObject.hpp
**
**      A aoScriptObject is a derived class for a storyboard, which is a textured
**	polygon that always faces the camera.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef AO_SCRIPTOBJECT_HPP
#error aoScriptObject.hpp multiply included
#endif
#define AO_SCRIPTOBJECT_HPP

#ifndef AO_AODATA_HPP
#include "Systems/AmbientOcclusion/Data/aoAOData.hpp"
#endif
#ifndef AO_SCRIPTDATA_HPP
#include "Systems/AmbientOcclusion/Data/aoScriptData.hpp"
#endif

#ifndef TMLN_SCRIPTOBJECT_HPP
#include "Support/tmln/tmlnScriptObject.hpp"
#endif


#include <string>


//============================================================================
//	forward references
//============================================================================
class aoAOObject;

//============================================================================
//============================================================================
class aoScriptObject 
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		aoScriptObject( );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~aoScriptObject();

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		//  get a list of resources.  the resources will be appended to the
		//	passed in list.
		//--------------------------------------------------------------------
		virtual void GetResourceList( fsResourceTrackerData& io_List );

	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a data structure
		//--------------------------------------------------------------------
		aoScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetScriptData(const aoScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a light base data structure
		//--------------------------------------------------------------------
		aoAOData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from light base data structure
		//--------------------------------------------------------------------
		void SetBaseData(const aoAOData &i_Data);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		aoAOObject* GetPickObject() const;

		//--------------------------------------------------------------------
		//	Name - passes functions on to pick object
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);
		const nameString& GetName() const;

	private:
		aoAOObject* m_pIcon;
};

