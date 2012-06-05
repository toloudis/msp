/*****************************************************************************
**  fogScriptObject.hpp
**
**      A fogScriptObject is a derived class for a storyboard, which is a textured
**	polygon that always faces the camera.
**
**	StudioGPU
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
#ifndef CMM_SCRIPTOBJECT_HPP
#include "Systems/Common/Object/cmmScriptObject.hpp"
#endif 



#include <string>


//============================================================================
//	forward references
//============================================================================
class fogFogObject;
class tmlnChannelBoolean;
class tmlnChannelEnum;
class tmlnChannelColor;
class tmlnChannelDistance;
class tmlnChannelFloat;

//============================================================================
//============================================================================
class fogScriptObject : public cmmScriptObject 
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

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetDisplayName() const;

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


	//========================================================================
	//	Timeline
	//========================================================================

		//--------------------------------------------------------------------
		// Accessors to channels
		//--------------------------------------------------------------------
		tmlnChannelBoolean&		bEnableChannel();
		tmlnChannelBoolean&		bWorldOrientationChannel();
		tmlnChannelEnum&		ModeChannel();
		tmlnChannelColor&		ColorChannel();
		tmlnChannelFloat&		DensityChannel();
		tmlnChannelFloat&		StartChannel();
		tmlnChannelFloat&		EndChannel();
		tmlnChannelFloat&		AltitudeStartChannel();
		tmlnChannelFloat&		AltitudeEndChannel();
		tmlnChannelFloat&		AltitudeDensityChannel();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		fogFogObject* GetPickObject() const;

		//--------------------------------------------------------------------
		//	Name - passes functions on to pick object
		//--------------------------------------------------------------------
		//void SetName(const nameString& i_Name);
		//const nameString& GetName() const;

		//--------------------------------------------------------------------
		// This is called when a driver in this script object is changed,
		//	or if a driver is added or deleted from this object.
		//--------------------------------------------------------------------
		virtual void NotifyDriverChanged();

	private:
		fogFogObject* m_pIcon;

		tmlnChannelBoolean*		m_pbEnableChannel;
		tmlnChannelBoolean*		m_pbWorldOrientationChannel;
		tmlnChannelEnum*		m_pModeChannel;
		tmlnChannelColor*		m_pColorChannel;
		tmlnChannelFloat*		m_pDensityChannel;
		tmlnChannelFloat*		m_pStartChannel;
		tmlnChannelFloat*		m_pEndChannel;
		tmlnChannelFloat*		m_pAltitudeStartChannel;
		tmlnChannelFloat*		m_pAltitudeEndChannel;
		tmlnChannelFloat*		m_pAltitudeDensityChannel;
};

