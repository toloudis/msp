/*****************************************************************************
**  sbrdScriptObject.hpp
**
**      A sbrdScriptObject is a derived class for a storyboard, which is a textured
**	polygon that always faces the camera.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SBRD_SCRIPTOBJECT_HPP
#error sbrdScriptObject.hpp multiply included
#endif
#define SBRD_SCRIPTOBJECT_HPP

#ifndef SBRD_SCRIPTDATA_HPP
#include "Systems/Storyboards/Data/sbrdScriptData.hpp"
#endif

#ifndef CMPS_MANIPOBJECT_HPP
#include "Support/cmps/cmpsManipObject.hpp"
#endif
#ifndef LYER_OBJECT_HPP
#include "Support/lyer/lyerObject.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif
#ifndef TMLN_SCRIPTOBJECT_HPP
#include "Support/tmln/tmlnScriptObject.hpp"
#endif

#include <string>


//============================================================================
//	forward references
//============================================================================
class api3dBillboard;
class api3dObject;
class fsLocator;
class matTexture;
class sbrdBillboardObject;
class sbrdAdapterGetPosition;
class tmlnChannelColor;
class tmlnChannelFloat;
class tmlnChannelOrientation;
class tmlnChannelPosition;
class tmlnChannelBoolean;
class tmlnChannelFileName;


//============================================================================
//============================================================================
class sbrdScriptObject : public pick3dPickObject, 
						 public tmlnScriptObject,
						 public lyerObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		explicit sbrdScriptObject( api3dBillboard * i_pBillboard );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~sbrdScriptObject();


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


	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a data structure
		//--------------------------------------------------------------------
		sbrdScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetScriptData(const sbrdScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a light base data structure
		//--------------------------------------------------------------------
		sbrdObjectData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from light base data structure
		//--------------------------------------------------------------------
		void SetBaseData(const sbrdObjectData &i_Data);


	//============================================================================
	//	members
	//============================================================================

		//--------------------------------------------------------------------
		//	Name - passes functions on to pick object
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);
		const nameString& GetName() const;

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the sbrdScriptObject.
		//	If it does, the t value is also returned in o_T.
		//--------------------------------------------------------------------
		virtual bool RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T);

		//--------------------------------------------------------------------
		//	ShowIcons - show driver icons
		//--------------------------------------------------------------------
		void ShowIcons(bool i_Renderable);

		//--------------------------------------------------------------------
		//	Set whether this object is selected in order to control
		//	display of icons or render style, etc.
		//--------------------------------------------------------------------
		void SetSelected(bool i_bSelected);

		//--------------------------------------------------------------------
		//  Changes visible state of prop
		//--------------------------------------------------------------------
		void  SetEditorVisible(bool i_bVisible);

		//--------------------------------------------------------------------
		//  Returns the visible state of prop 
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
		// For polling if an manipulation operation is currently enabled
		//--------------------------------------------------------------------
		//bool IsOperationEnabled(cmpsManipObject::Operations i_Operation, float i_Time);

		//--------------------------------------------------------------------
		//	Access to billboard object
		//--------------------------------------------------------------------
		api3dBillboard * GetBillboardObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		sbrdBillboardObject*	GetPickObject() const;

		//--------------------------------------------------------------------
		// This is called when a driver in this script object is changed,
		//	or if a driver is added or deleted from this object.
		//--------------------------------------------------------------------
		virtual void NotifyDriverChanged();

	//============================================================================
	//	Timeline
	//============================================================================

		//--------------------------------------------------------------------
		// Return adapter to access point light's position
		//--------------------------------------------------------------------
		sbrdAdapterGetPosition& AdapterGetPosition();

		//--------------------------------------------------------------------
		// Accessors to channels
		//--------------------------------------------------------------------
		tmlnChannelPosition& ChannelPosition();
		tmlnChannelOrientation& ChannelOrientation();
		tmlnChannelFileName& ChannelTexture();
		tmlnChannelColor& ChannelColor();
		tmlnChannelFloat& ScaleChannel();
		tmlnChannelBoolean& ChannelVisible();
		tmlnChannel& ChannelSound();

	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);

		sbrdBillboardObject*	m_pIcon;

		bool m_bShowDriverIcons;
		bool m_bSelected;
		sbrdAdapterGetPosition*	m_pAdapterGetPosition;

		//sbrdScriptData		m_Data;

		tmlnChannelPosition*	m_pChannelPos;
		tmlnChannelOrientation*	m_pChannelOrientation;
		tmlnChannelFileName*	m_pChannelTexture;
		tmlnChannelColor*		m_pChannelColor;
		tmlnChannelFloat*		m_pScaleChannel;
		tmlnChannelBoolean*		m_pChannelVisible;
		tmlnChannel*			m_pChannelSound;
};

