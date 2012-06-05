/*****************************************************************************
**  billScriptObject.hpp
**
**      A billScriptObject is a derived class for a billboard, which is a textured
**	polygon that always faces the camera.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef BILL_SCRIPTOBJECT_HPP
#error billScriptObject.hpp multiply included
#endif
#define BILL_SCRIPTOBJECT_HPP

#ifndef BILL_SCRIPTDATA_HPP
#include "Systems/Billboard/Data/billScriptData.hpp"
#endif
#ifndef CMPS_MANIPOBJECT_HPP
#include "Support/cmps/cmpsManipObject.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
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

#include <string>


//============================================================================
//	forward references
//============================================================================
class api3dBillboard;
class api3dObject;
class billBillboardObject;
class matTexture;
class tmlnChannelColor;
class tmlnChannelFloat;
class tmlnChannelOrientation;
class tmlnChannelPosition;
class tmlnChannelFileName;


//============================================================================
//============================================================================
class billScriptObject : public pick3dPickObject, 
						 public tmlnScriptObject,
						 public lyerObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		explicit billScriptObject( api3dBillboard * i_pBillboard );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~billScriptObject();


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
		billScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetScriptData(const billScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a light base data structure
		//--------------------------------------------------------------------
		billData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from light base data structure
		//--------------------------------------------------------------------
		void SetBaseData(const billData &i_Data);

	//============================================================================
	//	members
	//============================================================================

		//--------------------------------------------------------------------
		//	Name - passes functions on to pick object
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);
		const nameString& GetName() const;

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the billScriptObject.
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
		billBillboardObject*	GetPickObject() const;

		//--------------------------------------------------------------------
		// This is called when a driver in this script object is changed,
		//	or if a driver is added or deleted from this object.
		//--------------------------------------------------------------------
		virtual void NotifyDriverChanged();

	//============================================================================
	//	Timeline
	//============================================================================

		//--------------------------------------------------------------------
		// Accessors to channels
		//--------------------------------------------------------------------
		tmlnChannelPosition& ChannelPosition();
		tmlnChannelOrientation& ChannelOrientation();
		tmlnChannelFileName& ChannelTexture();
		tmlnChannelColor& ChannelColor();
		tmlnChannelFloat& ScaleChannel();

	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);

		billBillboardObject*	m_pIcon;

		bool m_bShowDriverIcons;
		bool m_bSelected;

		//billScriptData		m_Data;

		tmlnChannelPosition*	m_pChannelPos;
		tmlnChannelOrientation*	m_pChannelOrientation;
		tmlnChannelFileName*	m_pChannelTexture;
		tmlnChannelColor*		m_pChannelColor;
		tmlnChannelFloat*		m_pScaleChannel;
};

