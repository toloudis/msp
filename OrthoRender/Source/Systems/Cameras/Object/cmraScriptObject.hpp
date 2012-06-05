/*****************************************************************************
**  cmraScriptObject.hpp
**
**      A cmraScriptObject is a derived class for recording and displaying
**	a camera's position.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_SCRIPTOBJECT_HPP
#error cmraScriptObject.hpp multiply included
#endif
#define CMRA_SCRIPTOBJECT_HPP

#ifndef MNM_OBJECT_HPP
#include "Support/mnm/mnmObject.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef TMLN_SCRIPTOBJECT_HPP
#include "Support/tmln/tmlnScriptObject.hpp"
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
class prtyProperty;
class tmlnChannelPosition;
class tmlnChannelFloat;
class tmlnChannelFloatProperty;
class tmlnChannelBoolean;


//============================================================================
//============================================================================
class cmraScriptObject : public pick3dPickObject, 
						 public tmlnScriptObject,
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
		virtual std::string GetTmlnName() const;


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

	//============================================================================
	//	data
	//============================================================================

		//--------------------------------------------------------------------
		//	Name - passes functions on to pick object
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);
		const nameString& GetName() const;

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the cmraScriptObject.
		//	If it does, the t value is also returned in o_T.
		//--------------------------------------------------------------------
		virtual bool RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T);

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
		//  Changes visible state of camera
		//--------------------------------------------------------------------
		void  SetEditorVisible(bool i_bVisible);

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
};

