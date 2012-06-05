/*****************************************************************************
**  aoScriptObject.hpp
**
**      A aoScriptObject is a derived class for a storyboard, which is a textured
**	polygon that always faces the camera.
**
**	StudioGPU
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

#ifndef CMM_SCRIPTOBJECT_HPP
#include "Systems/Common/Object/cmmScriptObject.hpp"
#endif 

#include <string>


//============================================================================
//	forward references
//============================================================================
class aoAOObject;
class tmlnChannelColor;
class tmlnChannelFloat;

//============================================================================
//============================================================================
class aoScriptObject : 	public cmmScriptObject
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
		tmlnChannelFloat&		ClipPlaneEpsilonChannel();
		tmlnChannelFloat&		NoClipPlaneEpsilonChannel();
		tmlnChannelFloat&		AreaRatioChannel();
		tmlnChannelFloat&		BehindPlaneEpsilonChannel();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		aoAOObject* GetPickObject() const;

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
		aoAOObject* m_pIcon;

		tmlnChannelFloat*		m_pRadiusChannel;
		tmlnChannelFloat*		m_pRadiusFarChannel;
		tmlnChannelFloat*		m_pAngleBiasChannel;
		tmlnChannelFloat*		m_pAttenuationChannel;
		tmlnChannelFloat*		m_pContrastChannel;
		tmlnChannelFloat*		m_pBlurWidthChannel;
		tmlnChannelFloat*		m_pBlurSharpnessChannel;
		tmlnChannelColor*		m_pColorChannel;
		tmlnChannelFloat*		m_pClipPlaneEpsilonChannel;
		tmlnChannelFloat*		m_pNoClipPlaneEpsilonChannel;
		tmlnChannelFloat*		m_pAreaRatioChannel;
		tmlnChannelFloat*		m_pBehindPlaneEpsilonChannel;

};

