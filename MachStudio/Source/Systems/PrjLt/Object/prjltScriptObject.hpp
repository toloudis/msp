/*****************************************************************************
**  prjltScriptObject.hpp
**
**      A prjltScriptObject is a derived class for displaying a projected
**	light's position.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PRJLT_SCRIPTOBJECT_HPP
#error prjltScriptObject.hpp multiply included
#endif
#define PRJLT_SCRIPTOBJECT_HPP

#ifndef PRJLT_SCRIPTDATA_HPP
#include "Systems/PrjLt/Data/prjltScriptData.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef CMM_SCRIPTOBJECT_HPP
#include "Systems/Common/Object/cmmScriptObject.hpp"
#endif 
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef LYER_OBJECT_HPP
#include "Support/lyer/lyerObject.hpp"
#endif

#include <string>


//============================================================================
//	forward references
//============================================================================
class fsLocator;
class gfFileTxt;
class prjltIconObject;
class prjltProjectedLightObject;
class tmlnChannelBoolean;
class tmlnChannelColor;
class tmlnChannelTextureFileName;
class tmlnChannelFloat;
class tmlnChannelPosition;
class tmlnChannelOrientation;

//============================================================================
//============================================================================
class prjltScriptObject : public cmmScriptObject,
						  public lyerObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prjltScriptObject(const prjltScriptData &i_Data);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prjltScriptObject();

		//------------------------------------------------------------------------
		// Set directory to find projected textures
		//------------------------------------------------------------------------
		static void SetTextureDir(const fsLocator &i_Dir);


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

		//--------------------------------------------------------------------
		//  get a list of resources.  the resources will be appended to the
		//	passed in list.
		//--------------------------------------------------------------------
		virtual void GetResourceList( fsResourceTrackerData& io_List );


	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a light data structure
		//--------------------------------------------------------------------
		prjltScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from light data structure
		//--------------------------------------------------------------------
		void SetScriptData(const prjltScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a light data structure
		//--------------------------------------------------------------------
		prjltData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from light data structure
		//--------------------------------------------------------------------
		void SetBaseData(const prjltData &i_Data);

	//============================================================================
	//	Timeline
	//============================================================================

		//--------------------------------------------------------------------
		// Accessors to channels
		//--------------------------------------------------------------------
		//tmlnChannelFilePath& TextureChannel();
		tmlnChannelTextureFileName& TextureChannel();
		tmlnChannelPosition& PositionChannel();
		tmlnChannelPosition& TargetChannel();
		tmlnChannelOrientation& OrientationChannel();
		tmlnChannelBoolean& EnabledChannel();
		tmlnChannelColor& ColorChannel();
		tmlnChannelFloat& TiltChannel();
		tmlnChannelFloat& RangeChannel();
		tmlnChannelFloat& IntensityChannel();
		tmlnChannelFloat& AngleChannel();
		tmlnChannelFloat& PenumbraChannel();
		tmlnChannelFloat& ScaleChannel();
		tmlnChannelBoolean& ShadowSourceChannel();
		tmlnChannelFloat& AspectChannel();
		tmlnChannelFloat& ShadowIntensityChannel();
		tmlnChannelFloat& LightSizeChannel();
		tmlnChannelFloat& PCSSAdjustChannel();
		tmlnChannelFloat& DepthMapSizeChannel();
		tmlnChannelFloat& DepthBiasChannel();
		tmlnChannelBoolean& ShaftVisibleChannel();
		tmlnChannelFloat& ShaftAlphaChannel();
		tmlnChannelFloat& ShaftDensityChannel();
		tmlnChannelBoolean& EnableDiffuseChannel();
		tmlnChannelBoolean& EnableSpecularChannel();
		tmlnChannelBoolean& AffectsGlowChannel();
		tmlnChannelPosition& FalloffChannel();
		tmlnChannelFloat& ShaftFalloffStartChannel();
		tmlnChannelFloat& ShaftFalloffEndChannel();

	//============================================================================
	//	3D icon/geometry
	//============================================================================

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
		//  Returns the visible state of light 
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
		//--------------------------------------------------------------------
		prjltProjectedLightObject* GetPickObject() const;

		//--------------------------------------------------------------------
		// This is called when a driver in this script object is changed,
		//	or if a driver is added or deleted from this object.
		//--------------------------------------------------------------------
		virtual void NotifyDriverChanged();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void ReportMemory(gfFileTxt& i_File);

	private:
		prjltProjectedLightObject*	m_pIcon;

		// Timeline related
		//tmlnChannelFilePath* m_pTextureChannel;
		tmlnChannelTextureFileName* m_pTextureChannel;
		tmlnChannelPosition* m_pPosChannel;
		tmlnChannelPosition* m_pTargetChannel;
		tmlnChannelOrientation* m_pOrientationChannel;
		tmlnChannelBoolean* m_pEnableChannel;
		tmlnChannelColor* m_pColorChannel;
		tmlnChannelFloat* m_pTiltChannel;
		tmlnChannelFloat* m_pRangeChannel;
		tmlnChannelFloat* m_pIntensityChannel;
		tmlnChannelFloat* m_pAngleChannel;
		tmlnChannelFloat* m_pPenumbraChannel;
		tmlnChannelFloat* m_pScaleChannel;
		tmlnChannelBoolean* m_pShadowSourceChannel;
		tmlnChannelFloat* m_pAspectChannel;
		tmlnChannelFloat* m_pShadowIntensityChannel;
		tmlnChannelFloat* m_pLightSizeChannel;
		tmlnChannelFloat* m_pPCSSAdjustChannel;
		tmlnChannelFloat* m_pDepthMapSizeChannel;
		tmlnChannelFloat* m_pDepthBiasChannel;
		tmlnChannelBoolean* m_pShaftVisibleChannel;
		tmlnChannelFloat* m_pShaftAlphaChannel;
		tmlnChannelFloat* m_pShaftDensityChannel;
		tmlnChannelBoolean* m_pEnableDiffuseChannel;
		tmlnChannelBoolean* m_pEnableSpecularChannel;
		tmlnChannelBoolean* m_pAffectsGlowChannel;
		tmlnChannelPosition* m_pFalloffChannel;
		tmlnChannelFloat* m_pShaftFalloffStartChannel;
		tmlnChannelFloat* m_pShaftFalloffEndChannel;

		bool m_bShowDriverIcons;
		bool m_bSelected;
		bool m_bPreRangeState;
};

