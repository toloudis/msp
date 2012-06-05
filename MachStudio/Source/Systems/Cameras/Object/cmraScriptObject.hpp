/*****************************************************************************
**  cmraScriptObject.hpp
**
**      A cmraScriptObject is a derived class for recording and displaying
**	a camera's position.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_SCRIPTOBJECT_HPP
#error cmraScriptObject.hpp multiply included
#endif
#define CMRA_SCRIPTOBJECT_HPP

#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef CMM_SCRIPTOBJECT_HPP
#include "Systems/Common/Object/cmmScriptObject.hpp"
#endif 
#ifndef LYER_OBJECT_HPP
#include "Support/lyer/lyerObject.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <string>


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class camCamera;
class cmraCameraData;
class cmraCameraObject;
class cmraChannelCapture;
class cmraChannelFloat;
class cmraIconObject;
class cmraScriptData;
class gpxCamera;
class nameString;
class prtyProperty;
class tmlnChannelPosition;
class tmlnChannelFloat;
class tmlnChannelFloatProperty;
class tmlnChannelBoolean;
class tmlnChannelTextureFileName;
class tmlnChannelRenderPass;

//============================================================================
//============================================================================
class cmraScriptObject : public cmmScriptObject,
						 public lyerObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmraScriptObject(const cmraScriptData &i_Data, const nameString& i_CamName);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmraScriptObject();

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


	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a camera data structure
		//--------------------------------------------------------------------
		cmraScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from camera data structure
		//--------------------------------------------------------------------
		void SetScriptData(const cmraScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a base data structure
		//--------------------------------------------------------------------
		cmraCameraData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from base data structure
		//--------------------------------------------------------------------
		void SetBaseData(const cmraCameraData &i_Data);

		//--------------------------------------------------------------------
		// Get the number of capture drivers
		//--------------------------------------------------------------------
		int GetNumCaptureDrivers();


	//============================================================================
	//	Timeline
	//============================================================================

		//--------------------------------------------------------------------
		// Accessors to channels
		//--------------------------------------------------------------------
		tmlnChannelPosition& PositionChannel();
		tmlnChannelPosition& TargetChannel();
		tmlnChannelFloat& FOVChannel();
		tmlnChannelFloat& OrthoWidthChannel();
		tmlnChannelFloat& TiltChannel();
		cmraChannelCapture& CaptureChannel();
		tmlnChannelRenderPass& RenderPassAOChannel();
		tmlnChannelRenderPass& RenderPassGIChannel();
		tmlnChannelRenderPass& RenderPassReflChannel();
		tmlnChannelRenderPass& RenderPassShadowMaskChannel();
		tmlnChannelRenderPass& RenderPassBeautyChannel();
		tmlnChannelFloatProperty& NearFocusDistanceChannel();
		tmlnChannelFloatProperty& FarFocusDistanceChannel();
		tmlnChannelFloatProperty& NearBlurDistanceChannel();
		tmlnChannelFloatProperty& FarBlurDistanceChannel();
		tmlnChannelFloat& NearClipChannel();
		tmlnChannelFloat& FarClipChannel();
		tmlnChannelFloat& DOFMaxFarBlurChannel();
		tmlnChannelFloat& HDRMiddleGrayChannel();
		tmlnChannelFloat& HDRBloomScaleChannel();
		tmlnChannelFloat& HDRStarScaleChannel();
		tmlnChannelFloat& HDRBrightPassThreshChannel();
		tmlnChannelFloat& HDRBrightPassOffsetChannel();
		tmlnChannelFloat& HDRWhiteCutoffChannel();
		tmlnChannelFloat& HDRSceneLuminanceChannel();
		tmlnChannelFloat& DOFMaxCoCChannel();
		tmlnChannelFloat& StereoFDChannel();
		tmlnChannelFloat& StereoIODChannel();
		tmlnChannelFloat& FocalLengthChannel();
		tmlnChannelFloat& HorizontalApertureChannel();
		tmlnChannelFloat& FStopChannel();
		tmlnChannelFloat& FocusDistanceChannel();
		tmlnChannelFloat& CoCChannel();
	//============================================================================
	//	data
	//============================================================================

		//--------------------------------------------------------------------
		//	Name - passes functions on to pick object
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);
		const nameString& GetName() const;

		//--------------------------------------------------------------------
		//	ShowIcons - show icons for the camera and drivers
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
		//  Changes visible state of camera
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
		cmraCameraObject*	GetPickObject() const;

		//--------------------------------------------------------------------
		// This is called when a driver in this script object is changed,
		//	or if a driver is added or deleted from this object.
		//--------------------------------------------------------------------
		void NotifyDriverChanged();

	//========================================================================
	//	Helper Functions
	//========================================================================

		//--------------------------------------------------------------------
		// Accessor to camera
		//--------------------------------------------------------------------
		const camCamera&	GetCamera() const;
		camCamera&	Camera();
		shared_ptr<camCamera> GetCameraPtr();
		shared_ptr<gpxCamera> GetCameraProxyPtr();

	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);

		cmraCameraObject*	m_pIcon;
		bool m_bDebugRenderable;
		bool m_bSelected;

		// Timeline related
		tmlnChannelPosition*	m_pPosChannel;
		tmlnChannelPosition*	m_pTargetChannel;
		tmlnChannelFloat*		m_pFOVChannel;
		tmlnChannelFloat*		m_pOrthoWidthChannel;
		tmlnChannelFloat*		m_pTiltChannel;
		cmraChannelCapture*		m_pCaptureChannel;
		tmlnChannelRenderPass*	m_pRenderPassAOChannel;
		tmlnChannelRenderPass*	m_pRenderPassGIChannel;
		tmlnChannelRenderPass*	m_pRenderPassReflChannel;
		tmlnChannelRenderPass*	m_pRenderPassShadowMaskChannel;
		tmlnChannelRenderPass*	m_pRenderPassBeautyChannel;
		tmlnChannelFloatProperty* m_pNearFocusDistanceChannel;
		tmlnChannelFloatProperty* m_pFarFocusDistanceChannel;
		tmlnChannelFloatProperty* m_pNearBlurDistanceChannel;
		tmlnChannelFloatProperty* m_pFarBlurDistanceChannel;
		tmlnChannelFloat*		m_pNearClipChannel;
		tmlnChannelFloat*		m_pFarClipChannel;
		tmlnChannelFloat*		m_pDOFMaxFarBlurChannel;
		tmlnChannelFloat*		m_pHDRMiddleGrayChannel;
		tmlnChannelFloat*		m_pHDRBloomScaleChannel;
		tmlnChannelFloat*		m_pHDRStarScaleChannel;
		tmlnChannelFloat*		m_pHDRBrightPassThreshChannel;
		tmlnChannelFloat*		m_pHDRBrightPassOffsetChannel;
		tmlnChannelFloat*		m_pHDRWhiteCutoffChannel;
		tmlnChannelFloat*		m_pHDRSceneLuminanceChannel;
		tmlnChannelFloat*		m_pDOFMaxCoCChannel;

		tmlnChannelFloat*		m_pStereoFDChannel;
		tmlnChannelFloat*		m_pStereoIODChannel;

		// Real world camera properties
		tmlnChannelFloat*		m_pFocalLengthChannel;
		tmlnChannelFloat*		m_pHorizontalApertureChannel;
		tmlnChannelFloat*		m_pFStopChannel;
		tmlnChannelFloat*		m_pFocalDistanceChannel;
		tmlnChannelFloat*		m_pCoCChannel;
};

