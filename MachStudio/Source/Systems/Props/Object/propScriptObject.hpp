/*****************************************************************************
**  propScriptObject.hpp
**
**      A propScriptObject is a derived class for displaying an object's position.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PROP_SCRIPTOBJECT_HPP
#error propScriptObject.hpp multiply included
#endif
#define PROP_SCRIPTOBJECT_HPP

#ifndef PROP_SCRIPTDATA_HPP
#include "Systems/Props/Data/propScriptData.hpp"
#endif
#ifndef PROP_PROPOBJECT_HPP
#include "Systems/Props/Object/propPropObject.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef DYN_SCRIPTOBJECT_HPP
#include "Support/dyn/dynScriptObject.hpp"
#endif
#ifndef MTRL_SCRIPTOBJECT_HPP
#include "Support/mtrl/mtrlScriptObject.hpp"
#endif
#ifndef FGMT_SCRIPTOBJECT_HPP
#include "Support/fgmt/fgmtScriptObject.hpp"
#endif
#ifndef LYER_OBJECT_HPP
#include "Support/lyer/lyerObject.hpp"
#endif

#include <string>


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class api3dObjectEntity;
class tmlnChannelAnimationFull;
class propAdapterGetPosition;
class tmlnChannel;
class tmlnChannelFloat;
class tmlnChannelOrientation;
class tmlnChannelPosition;
class tmlnChannelBoolean;


//============================================================================
//============================================================================
class propScriptObject : public pick3dPickObject, 
						 public dynScriptObject,
						 public mtrlScriptObject,
						 public fgmtScriptObject,
						 public lyerObject
{
	public:
		//--------------------------------------------------------------------
		// ownership for the api3dObject passes to this object.
		// Locator should point to the geometry file in order to parse
		//	and write materials.
		//--------------------------------------------------------------------
		propScriptObject(api3dObject* i_pObject, const fsLocator& i_ModelFile);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~propScriptObject();

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
	//	members
	//============================================================================

		//--------------------------------------------------------------------
		//	Name - simply pass functions to icon object
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);
		const nameString& GetName() const;

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the propScriptObject.
		//	If it does, the t value is also returned in o_T.
		//--------------------------------------------------------------------
		bool RayPick(	const maPoint3d& i_RayStart,
						const maPoint3d& i_RayEnd,
						float& o_T);

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
		//	LayerLowRes represents if the objects are rendered
		//	using a low resolution model.
		//--------------------------------------------------------------------
		virtual void SetLayerLowRes(bool i_bLowRes);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		propPropObject*	GetPickObject() const;

		//--------------------------------------------------------------------
		//	filename
		//--------------------------------------------------------------------
		void GetFilename( itString& o_Filename ) const;

		//--------------------------------------------------------------------
		//	Directory
		//--------------------------------------------------------------------
		void SetDirectory( const fsLocator& i_Dir );
		void GetDirectory( fsLocator& o_Dir ) const;

		//--------------------------------------------------------------------
		// This is called when a driver in this script object is changed,
		//	or if a driver is added or deleted from this object.
		//--------------------------------------------------------------------
		virtual void NotifyDriverChanged();

		//--------------------------------------------------------------------
		// OverrideMaterials is used by the user interface. It just calls 
		// GatherMaterials; but since it is virtual, it can also set a 
		// dirty bit on the chunk and update the object part interface.
		//--------------------------------------------------------------------
		virtual void OverrideMaterials();

		//--------------------------------------------------------------------
		//	Lock the materials
		//--------------------------------------------------------------------
		virtual void LockMaterials(const bool i_bLock);

		//--------------------------------------------------------------------
		// Set subdivision level being used.
		//--------------------------------------------------------------------
		void SetSubdivLevel(int i_SubdivLevel);


	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a data structure
		//--------------------------------------------------------------------
		propScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetScriptData(const propScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a base data structure
		//--------------------------------------------------------------------
		propData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from base data structure
		//--------------------------------------------------------------------
		void SetBaseData(const propData &i_Data);

	//============================================================================
	//	Parts
	//============================================================================

		//--------------------------------------------------------------------
		// Gather up names of selectable parts grouped by category
		//--------------------------------------------------------------------
		void GatherPartNames(std::map<std::string, std::vector<std::string> > &o_Parts);

		//--------------------------------------------------------------------
		//	Select object part, like surface or material
		//--------------------------------------------------------------------
		void SelectObjectPart(const std::string i_PartName, 
							  const std::string i_CategoryName, 
							  bool i_bAppend);

	//============================================================================
	//	Timeline
	//============================================================================

		//--------------------------------------------------------------------
		// Return adapter to access point light's position
		//--------------------------------------------------------------------
		propAdapterGetPosition& AdapterGetPosition();

		//--------------------------------------------------------------------
		// Accessors to channels
		//--------------------------------------------------------------------
		tmlnChannelAnimationFull& ChannelAnimationFull();
		tmlnChannelPosition& ChannelPosition();
		tmlnChannelOrientation& ChannelOrientation();
		tmlnChannelFloat& ChannelScale();
		tmlnChannelBoolean& ChannelVisible();
		tmlnChannel& ChannelSound();

	protected:
		//--------------------------------------------------------------------
		// This virtual function is called when the materials are saved 
		//	to a new filename. The locator representing this geometry
		//	should now point to the new filename.
		//--------------------------------------------------------------------
		//virtual void NotifyLocatorChanged(const fsLocator& i_NewLocator);

	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void propScriptObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty);

		propPropObject*		m_pIcon;	// Manipulation object for the compass

		fsLocator	m_Directory;

		bool			m_bShowDriverIcons;
		bool			m_bSelected;

		tmlnChannelAnimationFull*	m_pChannelAnimationFull;
		tmlnChannelOrientation*		m_pChannelOrientation;
		tmlnChannelPosition*		m_pChannelPosition;
		tmlnChannelFloat*			m_pChannelScale;
		tmlnChannelBoolean*			m_pChannelVisible;
		tmlnChannel*				m_pChannelSound;

		propAdapterGetPosition*		m_pAdapterGetPosition;
};

