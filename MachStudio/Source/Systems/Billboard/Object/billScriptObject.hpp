/*****************************************************************************
**  billScriptObject.hpp
**
**      A billScriptObject is a derived class for a billboard, which is a textured
**	polygon that always faces the camera.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef BILL_SCRIPTOBJECT_HPP
#error billScriptObject.hpp multiply included
#endif
#define BILL_SCRIPTOBJECT_HPP

#ifndef BILL_SCRIPTDATA_HPP
#include "Systems/Billboard/Data/billScriptData.hpp"
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
class api3dBillboard;
class api3dObject;
class billBillboardObject;
class matTexture;
class tmlnChannelBoolean;
class tmlnChannelColor;
class tmlnChannelFilePath;
class tmlnChannelFloat;
class tmlnChannelOrientation;
class tmlnChannelPosition;
class tmlnChannelScale;
class tmlnChannelFilePath;
class tmlnChannelVideo;
class camCamera;

//============================================================================
//============================================================================
class billScriptObject : public cmmScriptObject,
						 public lyerObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		explicit billScriptObject( api3dBillboard * i_pBillboard,
					std::vector<shared_ptr<camCamera>>& i_CamList);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~billScriptObject();


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
		//	ShowIcons - show driver icons
		//--------------------------------------------------------------------
		void ShowIcons(bool i_Renderable);

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
		//bool IsOperationEnabled(cmpsManipObject::Operations i_Operation, const maTime& i_Time);

		//--------------------------------------------------------------------
		//	Access to billboard object
		//--------------------------------------------------------------------
		api3dBillboard * GetBillboardObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		billBillboardObject*	GetPickObject() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void UpdateCameraList(std::vector<shared_ptr<camCamera>>& i_CameraList,
							std::vector<std::string>& i_NameList);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void DeleteCameraIndex(int i_Index);
		
	
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
		tmlnChannelFilePath& ChannelTexture();
		tmlnChannelColor& ChannelColor();
		tmlnChannelFloat& ScaleChannel();
		tmlnChannelFloat& BrightnessChannel();
		tmlnChannelScale& NonUniformScaleChannel();		
		tmlnChannelVideo& ChannelVideo();
		tmlnChannelBoolean& ChannelVisible();
		

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
		tmlnChannelFilePath*	m_pChannelTexture;
		tmlnChannelColor*		m_pChannelColor;
		tmlnChannelFloat*		m_pScaleChannel;
		tmlnChannelFloat*		m_pBrightnessChannel;
		tmlnChannelScale*		m_pNonUniformScaleChannel;
		tmlnChannelBoolean*		m_pChannelVisible;
		tmlnChannelVideo*		m_pChannelVideo;	
};

