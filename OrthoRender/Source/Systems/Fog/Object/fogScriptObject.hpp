/*****************************************************************************
**  fogScriptObject.hpp
**
**      A fogScriptObject is a derived class for a storyboard, which is a textured
**	polygon that always faces the camera.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef FOG_SCRIPTOBJECT_HPP
#error fogScriptObject.hpp multiply included
#endif
#define FOG_SCRIPTOBJECT_HPP

#ifndef FOG_FOGDATA_HPP
#include "Systems/Fog/Data/fogFogData.hpp"
#endif
#ifndef FOG_SCRIPTDATA_HPP
#include "Systems/Fog/Data/fogScriptData.hpp"
#endif

#ifndef TMLN_SCRIPTOBJECT_HPP
#include "Support/tmln/tmlnScriptObject.hpp"
#endif


#include <string>


//============================================================================
//	forward references
//============================================================================
class fogFogObject;

//============================================================================
//============================================================================
class fogScriptObject 
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		fogScriptObject( );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~fogScriptObject();

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
		fogScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetScriptData(const fogScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a light base data structure
		//--------------------------------------------------------------------
		fogFogData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from light base data structure
		//--------------------------------------------------------------------
		void SetBaseData(const fogFogData &i_Data);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		fogFogObject* GetPickObject() const;

		//--------------------------------------------------------------------
		//	Name - passes functions on to pick object
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);
		const nameString& GetName() const;

	private:
		fogFogObject* m_pIcon;
};

