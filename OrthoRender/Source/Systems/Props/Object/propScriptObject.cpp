/*****************************************************************************
**  propScriptObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/Object/propScriptObject.hpp"

#include "Systems/Props/Timeline/propAdapterGetPosition.hpp"
#include "Systems/Props/GUI/propDialogDataUtil.hpp"
#include "Systems/Props/Data/propDocumentChunk.hpp"
#include "Systems/Props/Undo/propOperations.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/fgmt/GUI/fgmtOperations.hpp"
#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"
#include "Support/grps/grpsGroupMgr.hpp"
#include "Support/lyer/lyerLayerMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"
#include "Support/tmln/tmlnChannelAnimationFull.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

// library
#include "Core/fs/fsFileUtil.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"


//----------------------------------------------------------------------------
// ownership for the api3dObject passes to this object
//----------------------------------------------------------------------------
propScriptObject::propScriptObject( api3dObject* i_pObject, const fsLocator& i_ModelFile )
:	dynScriptObject(i_pObject), 
	mtrlScriptObject(i_pObject, i_ModelFile), 
	fgmtScriptObject(i_pObject, i_ModelFile), 
	m_bShowDriverIcons(true), 
	m_bSelected(false)
{
	//m_pDebugObject = NULL;
	//if ( m_pDebugObject )
	//{
	//	m_pDebugObject = api3dShape::CreateSphere(maFloatRGBA(1,0,0,1), l_SphereRadius, 8, 8);
	//	m_pDebugObject->SetPosition(m_Position);
	//	api3dScene::AddObject(m_pDebugObject);
	//}

	fsLocator aoTexDir = gfPaths::GetPath(mnmPaths::e_DataScene);
	aoTexDir.Push("Props");
	aoTexDir.Push("General");
	aoTexDir.Push(gfPaths::GetSubPath(gfPaths::e_Textures));
	SetAOTextureLocator(aoTexDir);

	fsLocator dir(i_ModelFile);
	dir.Pop();
	m_pIcon = new propPropObject(i_pObject, dir);
	m_pIcon->SetParentObject(this);

	// Connect up a parent relationship between the
	// selectable parts and the pick object
	fgmtScriptObject::SetParent(m_pIcon);
	mtrlScriptObject::SetParent(m_pIcon);

	// Connect up fgmtscriptobject prty's to be shown in this UI
	fgmtScriptObject::SetPrtyObject(m_pIcon);

	// Timeline stuff
	//
	tmlnTimelineMgr::AddObject(this,"Prop");

	m_pChannelAnimationFull = new tmlnChannelAnimationFull( "Anim-Full", m_pIcon->GetEntity() );
	this->AddChannel(m_pChannelAnimationFull);
	m_pChannelPosition = new tmlnChannelPositionProperty( "Position", m_pIcon->PropertyPosition() );
	this->AddChannel(m_pChannelPosition);
	m_pChannelOrientation = new tmlnChannelOrientationProperty( "Orientation", m_pIcon->PropertyOrientation() );
	this->AddChannel(m_pChannelOrientation);
	m_pChannelScale = new tmlnChannelFloatProperty( "Scale", m_pIcon->PropertyScale() );
	this->AddChannel(m_pChannelScale);
	m_pChannelVisible = new tmlnChannelBooleanProperty( "Visible", m_pIcon->PropertyVisible() );
	this->AddChannel(m_pChannelVisible);
	m_pChannelSound = this->AddChannel("Sound");

	SetDirectory( dir );

	m_pAdapterGetPosition = new propAdapterGetPosition( m_pIcon->GetEntity() );

	lyerLayerMgr::AddObject(m_pIcon, this);
	grpsGroupMgr::AddObject(m_pIcon, m_pIcon);

	// Add name callback in order to notify the layer manager
	m_pIcon->PropertyName().AddCallback(new prtyCallbackWrapper<propScriptObject>(this, &propScriptObject::NameChanged));


	//DEBUG only
	//std::string tempstr;
	//fsFileUtil::LocatorToANSIFilename(i_Dir, tempstr);
	//DBG_LOG1( "prop created %s", tempstr.c_str() );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
propScriptObject::~propScriptObject()
{
	// Timeline stuff
	tmlnTimelineMgr::RemoveObject(this);

	// Unregister from layers
	lyerLayerMgr::RemoveObject(m_pIcon, this);
	grpsGroupMgr::RemoveObject(m_pIcon, m_pIcon);

	delete m_pAdapterGetPosition;
	delete m_pIcon;

	//if ( m_pDebugObject )
	//{
	//	api3dScene::RemoveObject(m_pDebugObject);
	//	delete m_pDebugObject;
	//}
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string propScriptObject::GetTmlnName() const
{
	return m_pIcon->GetName().GetString();
}


//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void propScriptObject::GetResourceList( fsResourceTrackerData& io_List )
{
	io_List.Add( this->GetBaseData().m_Filename.GetValue() );

	// add in material resources (textures used by overriden materials)
	mtrlScriptObject::GetMaterialResources( io_List );

	//	add all the driver resources as well
	tmlnScriptObject::GetResourceList( io_List );
}


//--------------------------------------------------------------------
//	Name - simply pass functions to icon object
//--------------------------------------------------------------------
void propScriptObject::SetName(const nameString& i_Name)
{
    m_pIcon->SetName( i_Name );
}
const nameString& propScriptObject::GetName() const
{
	return m_pIcon->GetName();
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the propScriptObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool propScriptObject::RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T)
{
	if (!this->GetLayerPickable())
		return false;

	return m_pIcon->RayPick(i_RayStart, i_RayEnd, o_T);
}

//--------------------------------------------------------------------
//	ShowIcons sets whether the driver icons are visible
//--------------------------------------------------------------------
void propScriptObject::ShowIcons(bool i_bVisible)
{
	m_bShowDriverIcons = i_bVisible;
	bool editor_visible = m_pIcon->GetEditorVisible();
	this->ShowDriverIcons(i_bVisible && editor_visible 
		&& this->GetLayerPickable() && m_bSelected);
}

//--------------------------------------------------------------------
//	Set whether this object is selected in order to control
//	display of icons or render style, etc.
//--------------------------------------------------------------------
void propScriptObject::SetSelected(bool i_bSelected)
{
	m_bSelected = i_bSelected;
	bool editor_visible = m_pIcon->GetEditorVisible();
	this->ShowDriverIcons(m_bShowDriverIcons && editor_visible 
		&& this->GetLayerPickable() && m_bSelected);
}


//--------------------------------------------------------------------
//  Changes visible state of prop 
//--------------------------------------------------------------------
void  propScriptObject::SetEditorVisible(bool i_bVisible)
{
	m_pIcon->SetEditorVisible(i_bVisible);
	this->ShowDriverIcons(i_bVisible && m_bShowDriverIcons 
		&& this->GetLayerPickable() && m_bSelected);
}

//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
//virtual 
void propScriptObject::SetLayerVisible(bool i_bVisible)
{
	// base function sets private flag
	lyerObject::SetLayerVisible(i_bVisible);

	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	m_pIcon->SetLayerVisible(i_bVisible);

}

//--------------------------------------------------------------------
//	LayerPickable represents if objects in the layer can be picked.
//--------------------------------------------------------------------
//virtual 
void propScriptObject::SetLayerPickable(bool i_bPickable)
{
	// base function sets private flag
	lyerObject::SetLayerPickable(i_bPickable);

	// Set GPU pickable flag of object
	m_pIcon->GetEntity()->SetGPUPickable( i_bPickable );

	// driver icons should only be shown when the object is pickable
	this->ShowDriverIcons( m_bShowDriverIcons 
						&& m_pIcon->GetEditorVisible() 
						&& this->GetLayerPickable()
						 && m_bSelected);
}


//--------------------------------------------------------------------
//	LayerWireframe represents if the objects are rendered
//	in a wireframe style.
//--------------------------------------------------------------------
//virtual 
void propScriptObject::SetLayerWireframe(bool i_bWireframe)
{
	m_pIcon->SetWireframe(i_bWireframe);
}

//--------------------------------------------------------------------
//	LayerLowRes represents if the objects are rendered
//	using a low resolution model.
//--------------------------------------------------------------------
//virtual 
void propScriptObject::SetLayerLowRes(bool i_bLowRes)
{
	m_pIcon->SetLowRes(i_bLowRes);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
propPropObject* propScriptObject::GetPickObject() const
{
	return m_pIcon;
}

//--------------------------------------------------------------------
//	filename
//--------------------------------------------------------------------
void propScriptObject::GetFilename( itString& o_Filename ) const
{
	m_pIcon->GetFilename( o_Filename );
}

//--------------------------------------------------------------------
//	Directory
//--------------------------------------------------------------------
void propScriptObject::SetDirectory( const fsLocator& i_Dir )
{
	m_Directory = i_Dir;
}
void propScriptObject::GetDirectory( fsLocator& o_Dir ) const
{
	o_Dir = m_Directory;
}

//--------------------------------------------------------------------
// This is called when a driver in this script object is changed,
//	or if a driver is added or deleted from this object.
//--------------------------------------------------------------------
void propScriptObject::NotifyDriverChanged()
{
	propOperations::ChangeDriverData(this->GetScriptData());
}

//--------------------------------------------------------------------
// OverrideMaterials is used by the user interface. It just calls 
// GatherMaterials; but since it is virtual, it can also set a 
// dirty bit on the chunk and update the object part interface.
//--------------------------------------------------------------------
void propScriptObject::OverrideMaterials()
{
	mtrlScriptObject::OverrideMaterials();

	// If we were to call this notify right away, it would alter the
	// control that is causing this callback. 
	mnmThinkMgr::CallFunctionDelayed(&propDialogDataUtil::UpdateListDialog);
	propDocumentChunk::ActiveDataChanged();
	sel3dMgr::Renotify();
}

//--------------------------------------------------------------------
//	Lock the materials
//--------------------------------------------------------------------
void propScriptObject::LockMaterials(const bool i_bLock)
{
	mtrlScriptObject::LockMaterials(i_bLock);

	propData data = GetBaseData();
	data.m_bLockedMaterials.SetValue(i_bLock);
	SetBaseData(data);
}

//--------------------------------------------------------------------
// Set subdivision level being used.
//--------------------------------------------------------------------
void propScriptObject::SetSubdivLevel(int i_SubdivLevel)
{
	// only do this step if the subdiv level is changing!
	if (m_pIcon->GetSubdivLevel() != i_SubdivLevel)
	{
		this->CleanUpFragments();

		m_pIcon->SetSubdivLevel(i_SubdivLevel);

		this->RefreshFragments();
	}
}


//============================================================================
//	Data
//============================================================================


//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
propScriptData propScriptObject::GetScriptData() const
{
	propScriptData data(this->GetBaseData());

	// get control data
	this->GetControlData(data.m_Controls);

	// get material data
	this->GetMaterialData(data.m_Materials);

	// get fragment data
	this->GetFragmentData(data.m_Fragments, data.m_AOData);

	// get driver info
	tmlnCreator::GetDriverInfo( this, (data.m_Drivers) );

	// get the channel info
	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );

	return data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void propScriptObject::SetScriptData(const propScriptData &i_Data)
{
	this->SetBaseData(i_Data.m_BaseData);

	// set control data
	this->SetControlData( i_Data.m_Controls );

	// set material data
	this->SetMaterialData( i_Data.m_Materials );

	// set fragment data
	itString modelName = i_Data.m_BaseData.m_Filename.GetValue();
	modelName.StripExtension();
	this->SetFragmentData( i_Data.m_Fragments, i_Data.m_AOData, itStringUtil::GetStdString(modelName) );

	// create drivers from info
	tmlnCreator::SetDriverInfo( this, (i_Data.m_Drivers) );

	// set the channels 
	this->ChannelSet().SetChannelInfo( i_Data.m_ChannelInfo );

	this->SetEditorVisible( i_Data.m_BaseData.m_bEditorVisible.GetValue() );
}

//--------------------------------------------------------------------
// Get values as a base data structure
//--------------------------------------------------------------------
propData propScriptObject::GetBaseData() const
{
	propData data( m_pIcon->GetData() );

	data.m_Position.SetValue(m_pChannelPosition->GetOriginalPosition());
	float x=0,y=0,z=0;
	m_pChannelOrientation->GetOriginalValue(x,y,z);
	data.m_Orientation.SetEuler(x,y,z);
	data.m_Scale.SetValue(m_pChannelScale->GetOriginalValue());

	return data;
}

//--------------------------------------------------------------------
// Set from base data structure
//--------------------------------------------------------------------
void propScriptObject::SetBaseData(const propData &i_Data)
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);

	mtrlScriptObject::LockMaterials(i_Data.m_bLockedMaterials.GetValue());

	m_pChannelPosition->SetOriginalPosition(i_Data.m_Position.GetValue());
	float x=0,y=0,z=0;
	i_Data.m_Orientation.GetEuler(x,y,z);
	m_pChannelOrientation->SetOriginalValue(x,y,z);
	m_pChannelScale->SetOriginalValue(i_Data.m_Scale.GetValue());

	//DEBUG only
	//itString fname;
	//this->GetFilename(fname);
	//DBG_LOG1( "prop filename set (%s)", itStringUtil::GetStdString(fname).c_str() );
}


//--------------------------------------------------------------------
// Gather up names of selectable parts grouped by category
//--------------------------------------------------------------------
void propScriptObject::GatherPartNames(std::map<std::string, std::vector<std::string> > &o_Parts)
{
	// Where should the define for the category name go?
	if (this->GetNumFragments() > 0)
		this->GetFragmentNames(o_Parts["Surfaces"]);
	else
		o_Parts["Surfaces"].push_back("Override Surface Flags");
	
	if (this->GetNumMaterials() > 0)
		this->GetMaterialNames(o_Parts["Materials"]);
	else
		o_Parts["Materials"].push_back("Override Materials");
	
}
//--------------------------------------------------------------------
//	Select object part, like surface or material
//--------------------------------------------------------------------
void propScriptObject::SelectObjectPart(const std::string i_PartName, 
									  const std::string i_CategoryName, 
									  bool i_bAppend)
{
	pick3dPickObject *pPart = NULL;
	if (i_CategoryName == "Surfaces")
	{
		if (this->GetNumFragments() > 0)
			pPart = this->GetFragmentUI(i_PartName);
		else
		{
			fgmtOperations::OverrideFragments(this);
			// If we were to call this notify right away, it would alter the
			// control that is causing this callback. 
			mnmThinkMgr::CallFunctionDelayed(&propDialogDataUtil::UpdateListDialog);
			propDocumentChunk::ActiveDataChanged();
		}
	}
	else if (i_CategoryName == "Materials")
	{
		if (this->GetNumMaterials() > 0)
			pPart = this->GetMaterialUI(i_PartName);
		else
		{
			mtrlOperations::OverrideMaterials(this);
		}
	}

	if (pPart && (sel3dMgr::GetSelected() != pPart))
	{
		sel3dMgr::CreateUndoOperation();
		if (i_bAppend)
			sel3dMgr::AddToSelection(pPart);
		else
			sel3dMgr::Select(pPart);
	}
}

//--------------------------------------------------------------------
// Return adapter to access point light's position
//--------------------------------------------------------------------
propAdapterGetPosition& propScriptObject::AdapterGetPosition()
{
	return (*m_pAdapterGetPosition);
}


//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
tmlnChannelAnimationFull& propScriptObject::ChannelAnimationFull()
{
	return (*m_pChannelAnimationFull);
}
tmlnChannelOrientation& propScriptObject::ChannelOrientation()
{
	return (*m_pChannelOrientation);
}
tmlnChannelPosition& propScriptObject::ChannelPosition()
{
	return (*m_pChannelPosition);
}
tmlnChannelFloat& propScriptObject::ChannelScale()
{
	return (*m_pChannelScale);
}
tmlnChannelBoolean& propScriptObject::ChannelVisible()
{
	return (*m_pChannelVisible);
}
tmlnChannel& propScriptObject::ChannelSound()
{
	return (*m_pChannelSound);
}

//--------------------------------------------------------------------
// This virtual function is called when the materials are saved 
//	to a new filename. The locator representing this geometry
//	should now point to the new filename.
//--------------------------------------------------------------------
//virtual 
void propScriptObject::NotifyLocatorChanged(const fsLocator& i_NewLocator)
{
	itString filename = i_NewLocator.GetLastName();
	m_pIcon->SetFilename(filename);

	fsLocator dir(i_NewLocator);
	dir.Pop();
	this->SetDirectory(dir);
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void propScriptObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Update LayerMgr
	lyerLayerMgr::ObjectRenamed(m_pIcon);
	evmtEnvironmentMgr::ObjectRenamed(m_pIcon);
	grpsGroupMgr::ObjectRenamed(m_pIcon);
}
