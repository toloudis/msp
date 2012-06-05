/*****************************************************************************
**  giScriptObject.hpp
**
**      A giScriptObject is a derived class for a storyboard, which is a textured
**	polygon that always faces the camera.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GI_SCRIPTOBJECT_HPP
#error giScriptObject.hpp multiply included
#endif
#define GI_SCRIPTOBJECT_HPP

#ifndef GI_GIDATA_HPP
#include "Systems/GlobalIllumination/Data/giGIData.hpp"
#endif
#ifndef GI_SCRIPTDATA_HPP
#include "Systems/GlobalIllumination/Data/giScriptData.hpp"
#endif

#ifndef CMM_SCRIPTOBJECT_HPP
#include "Systems/Common/Object/cmmScriptObject.hpp"
#endif 

#include <string>


//============================================================================
//	forward references
//============================================================================
class giGIObject;
class tmlnChannelColor;
class tmlnChannelFloat;

//============================================================================
//============================================================================
class giScriptObject : 	public cmmScriptObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		giScriptObject( );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~giScriptObject();

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
		giScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetScriptData(const giScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a light base data structure
		//--------------------------------------------------------------------
		giGIData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from light base data structure
		//--------------------------------------------------------------------
		void SetBaseData(const giGIData &i_Data);


	//========================================================================
	//	Timeline
	//========================================================================

		//--------------------------------------------------------------------
		// Accessors to channels
		//--------------------------------------------------------------------
		tmlnChannelFloat&		RadiusChannel();
		tmlnChannelFloat&		RadiusFarChannel();
		tmlnChannelFloat&		AngleBiasChannel();
		tmlnChannelFloat&		AttenuationChannel();
		tmlnChannelFloat&		ContrastChannel();
		tmlnChannelFloat&		BlurWidthChannel();
		tmlnChannelFloat&		BlurSharpnessChannel();
		tmlnChannelColor&		ColorChannel();


		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		giGIObject* GetPickObject() const;

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
		giGIObject* m_pIcon;

		tmlnChannelFloat*		m_pRadiusChannel;
		tmlnChannelFloat*		m_pRadiusFarChannel;
		tmlnChannelFloat*		m_pAngleBiasChannel;
		tmlnChannelFloat*		m_pAttenuationChannel;
		tmlnChannelFloat*		m_pContrastChannel;
		tmlnChannelFloat*		m_pBlurWidthChannel;
		tmlnChannelFloat*		m_pBlurSharpnessChannel;
		tmlnChannelColor*		m_pColorChannel;

};

