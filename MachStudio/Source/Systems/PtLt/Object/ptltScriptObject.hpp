/*****************************************************************************
**  ptltScriptObject.hpp
**
**      A ptltScriptObject is a derived class for displaying a point
**	light's position.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PTLT_SCRIPTOBJECT_HPP
#error ptltScriptObject.hpp multiply included
#endif
#define PTLT_SCRIPTOBJECT_HPP

#ifndef PTLT_DATA_HPP
#include "Systems/PtLt/Data/ptltData.hpp"
#endif
#ifndef PTLT_POINTLIGHTOBJECT_HPP
#include "Systems/PtLt/Object/ptltPointLightObject.hpp"
#endif
#ifndef PTLT_SCRIPTDATA_HPP
#include "Systems/PtLt/Data/ptltScriptData.hpp"
#endif

#ifndef CMM_SCRIPTOBJECT_HPP
#include "Systems/Common/Object/cmmScriptObject.hpp"
#endif 
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef LYER_OBJECT_HPP
#include "Support/lyer/lyerObject.hpp"
#endif


#include <string>


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class g3dPointLight;
class tmlnChannelBoolean;
class tmlnChannelColor;
class tmlnChannelFloat;
class tmlnChannelPosition;


//============================================================================
//============================================================================
class ptltScriptObject : public cmmScriptObject,
						 public lyerObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		ptltScriptObject(const ptltScriptData &i_Data);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~ptltScriptObject();

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
		ptltScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from light data structure
		//--------------------------------------------------------------------
		void SetScriptData(const ptltScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a light base data structure
		//--------------------------------------------------------------------
		ptltData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from light base data structure
		//--------------------------------------------------------------------
		void SetBaseData(const ptltData &i_Data);

	//========================================================================
	//	Timeline
	//========================================================================

		//--------------------------------------------------------------------
		// Accessors to channels
		//--------------------------------------------------------------------
		tmlnChannelPosition& PositionChannel();
		tmlnChannelBoolean& EnabledChannel();
		tmlnChannelColor& ColorChannel();
		tmlnChannelFloat& RangeChannel();
		tmlnChannelFloat& IntensityChannel();
		tmlnChannelBoolean& ShadowSourceChannel();
		tmlnChannelBoolean& EnableDiffuseChannel();
		tmlnChannelBoolean& EnableSpecularChannel();
		tmlnChannelBoolean& AffectsGlowChannel();
		tmlnChannelPosition& FalloffChannel();

	//========================================================================
	//	
	//========================================================================

		//--------------------------------------------------------------------
		//	Name - passes functions on to pick object
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);
		const nameString& GetName() const;


		//--------------------------------------------------------------------
		//	ShowIcons - show icons for the light and drivers
		//--------------------------------------------------------------------
		void ShowIcons(bool i_bVisible);

		//--------------------------------------------------------------------
		//	Set whether this object is selected in order to control
		//	display of icons or render style, etc.
		//--------------------------------------------------------------------
		void SetSelected(bool i_bSelected);

		//--------------------------------------------------------------------
		// Set object active state in the render layer
		//--------------------------------------------------------------------
		void SetActiveRenderLayer(bool i_bActive);

		//--------------------------------------------------------------------
		//  Changes visible state of light
		//--------------------------------------------------------------------
		void  SetEditorVisible(bool i_bVisible);

		//--------------------------------------------------------------------
		//  returns the visible state of light
		//--------------------------------------------------------------------
		bool  GetEditorVisible();

		//--------------------------------------------------------------------
		//	LayerVisible represents if objects in the layer are visible
		//--------------------------------------------------------------------
		virtual void SetLayerVisible(bool i_bVisible);

		//--------------------------------------------------------------------
		//	LayerPickable represents if objects in the layer can be picked.
		//--------------------------------------------------------------------
		virtual void SetLayerPickable(bool i_bPickable);

		//--------------------------------------------------------------------
		//	LayerWireframe represents if the objects are rendered
		//	in a wireframe style.
		//--------------------------------------------------------------------
		virtual void SetLayerWireframe(bool i_bWireframe);

		//--------------------------------------------------------------------
		// This is called when a driver in this script object is changed,
		//	or if a driver is added or deleted from this object.
		//--------------------------------------------------------------------
		virtual void NotifyDriverChanged();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		ptltPointLightObject*	GetPickObject() const;

	private:
		ptltPointLightObject*		m_pIcon;	// Manipulation object for the compass

		// Timeline related
		tmlnChannelPosition*	m_pPosChannel;
		tmlnChannelBoolean*		m_pEnableChannel;
		tmlnChannelColor*		m_pColorChannel;
		tmlnChannelFloat*		m_pRangeChannel;
		tmlnChannelFloat*		m_pIntensityChannel;
//		tmlnChannelBoolean*		m_pShadowSourceChannel;
		tmlnChannelBoolean*		m_pEnableDiffuseChannel;
		tmlnChannelBoolean*		m_pEnableSpecularChannel;
		tmlnChannelBoolean*		m_pAffectsGlowChannel;
		tmlnChannelPosition*	m_pFalloffChannel;

		bool m_bShowDriverIcons;
		bool m_bSelected;
		bool m_bPreRangeState;
};

