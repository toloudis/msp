/*****************************************************************************
**  trfnScriptObject.hpp
**
**      A trfnScriptObject is a derived class for 
**	animating the transformation of a group of objects.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TRFN_SCRIPTOBJECT_HPP
#error trfnScriptObject.hpp multiply included
#endif
#define TRFN_SCRIPTOBJECT_HPP

#ifndef TRFN_SCRIPTDATA_HPP
#include "Systems/Transforms/Data/trfnScriptData.hpp"
#endif
#ifndef TRFN_TRANSFORMOBJECT_HPP
#include "Systems/Transforms/Object/trfnTransformObject.hpp"
#endif
#ifndef CMM_SCRIPTOBJECT_HPP
#include "Systems/Common/Object/cmmScriptObject.hpp"
#endif 
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <map>


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class api3dObjectEntity;
class tmlnChannel;
class tmlnChannelBoolean;
class tmlnChannelFloat;
class tmlnChannelOrientation;
class tmlnChannelPosition;


//============================================================================
//============================================================================
class trfnScriptObject : public cmmScriptObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		trfnScriptObject( );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~trfnScriptObject();

		
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

		//--------------------------------------------------------------------
		//	Remap internal name attachments using the given map.
		//  This is part of the duplication process and makes sures 
		//	internal attachments are passed onto the duplicated objects.
		//--------------------------------------------------------------------
		virtual void RemapNames(const std::map<nameString, nameString> &i_DuplicateNameMap);

	//============================================================================
	//	members
	//============================================================================

		//--------------------------------------------------------------------
		//	Name - simply pass functions to icon object
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);
		const nameString& GetName() const;

		//--------------------------------------------------------------------
		//	ShowIcons sets whether the driver icons are visible
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
		//  Changes visible state of character
		//--------------------------------------------------------------------
		void  SetEditorVisible(bool i_bVisible);

		//--------------------------------------------------------------------
		//  Returns the visible state of character 
		//--------------------------------------------------------------------
		bool  GetEditorVisible();

		//--------------------------------------------------------------------
		// Access to selectable object
		//--------------------------------------------------------------------
		trfnTransformObject*	GetPickObject() const;

		//--------------------------------------------------------------------
		// This is called when a driver in this script object is changed,
		//	or if a driver is added or deleted from this object.
		//--------------------------------------------------------------------
		virtual void NotifyDriverChanged();


	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a data structure
		//--------------------------------------------------------------------
		trfnScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetScriptData(const trfnScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a base data structure
		//--------------------------------------------------------------------
		trfnData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from base data structure
		//--------------------------------------------------------------------
		void SetBaseData(const trfnData &i_Data);


	//============================================================================
	//	Timeline
	//============================================================================

		//--------------------------------------------------------------------
		// Accessors to channels
		//--------------------------------------------------------------------
		tmlnChannelPosition& ChannelPosition();
		tmlnChannelOrientation& ChannelOrientation();
		tmlnChannelFloat& ChannelScale();
		tmlnChannelBoolean& ChannelVisible();

	private:
		trfnTransformObject*					m_pIcon;	

		bool						m_bShowDriverIcons;
		bool						m_bSelected;

		tmlnChannelOrientation*		m_pChannelOrientation;
		tmlnChannelPosition*		m_pChannelPosition;
		tmlnChannelFloat*			m_pChannelScale;
		tmlnChannelBoolean*			m_pChannelVisible;
};

