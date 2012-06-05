/*****************************************************************************
**  chtrScriptObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Object/chtrScriptObject.hpp"

#include "Systems/Character/Data/chtrDocumentChunk.hpp"
#include "Systems/Character/Expressions/chtrExpressionObject.hpp"
#include "Systems/Character/Expressions/chtrExpressionOperations.hpp"
#include "Systems/Character/GUI/chtrPartConstants.hpp"
#include "Systems/Character/GUI/chtrDialogDataUtil.hpp"
#include "Systems/Character/Timeline/chtrAdapterGetPosition.hpp"
#include "Systems/Character/Timeline/chtrChannelAnimationSub.hpp"
#include "Systems/Character/Undo/chtrOperations.hpp"

#include "Support/dyn/GUI/dynOperations.hpp"
#include "Support/dyn/GUI/dynPropertyObject.hpp"
#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"
#include "Support/fgmt/GUI/fgmtOperations.hpp"
#include "Support/grps/grpsGroupMgr.hpp"
#include "Support/lyer/lyerLayerMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"
#include "Support/tmln/tmlnChannelAnimationFull.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

// library
#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"


//----------------------------------------------------------------------------
// ownership for the api3dObject passes to this object
//----------------------------------------------------------------------------
chtrScriptObject::chtrScriptObject( api3dObjectEntity* i_pObject, 
									chtrExpressionObject* i_pExpressions, 
									const fsLocator& i_ModelFile, 
									const fsLocator& i_AssetDir )
:	dynScriptObject(i_pObject), 
	mtrlScriptObject(i_pObject, i_ModelFile),
	fgmtScriptObject(i_pObject, i_ModelFile),
	m_pExpressions(i_pExpressions),
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
	aoTexDir.Push("Characters");
	aoTexDir.Push("General");
	aoTexDir.Push(gfPaths::GetSubPath(gfPaths::e_Textures));
	SetAOTextureLocator(aoTexDir);

	//DBG_LOG("Chtr created " << i_Dir);

	//	create the base object
	// NOTE the chtrObject constructor no longer changes the subdiv level
	m_pIcon = new chtrObject(i_pObject, i_AssetDir);
	m_pIcon->SetParentObject(this);

	// Connect up a parent relationship between the
	// selectable parts and the pick object
	fgmtScriptObject::SetParent(m_pIcon);
	mtrlScriptObject::SetParent(m_pIcon);
	dynScriptObject::SetParent(m_pIcon);

	// Connect up fgmtscriptobject prty's to be shown in this UI
	fgmtScriptObject::SetPrtyObject(m_pIcon);

	// Throw all of the expressions into our prtyObject:
	if (m_pExpressions != NULL)
		m_pIcon->GetListContainer().Add(m_pExpressions->GetList());

	// Timeline stuff
	tmlnTimelineMgr::AddObject(this,"Character");

	// Layer manager registration
	lyerLayerMgr::AddObject(m_pIcon, this);
	grpsGroupMgr::AddObject(m_pIcon, m_pIcon);
	
	//	Add the channels
	//
	m_pChannelAnimationFull = new tmlnChannelAnimationFull( "Anim-Full", m_pIcon->GetEntity() );
	m_pChannelAnimationFull->SetPriority(9);	// Delay until after expressions and controls
	this->AddChannel(m_pChannelAnimationFull);
	m_pChannelAnimationSub = new chtrChannelAnimationSub( "SubAnim", m_pIcon->GetEntity() );
	m_pChannelAnimationSub->SetPriority(10);	// Needs to add subanims after full anim clears animation
	this->AddChannel(m_pChannelAnimationSub);
	m_pChannelPosition = new tmlnChannelPositionProperty( "Position", m_pIcon->PropertyPosition() );
	this->AddChannel(m_pChannelPosition);
	m_pChannelOrientation = new tmlnChannelOrientationProperty( "Orientation", m_pIcon->PropertyOrientation() );
	this->AddChannel(m_pChannelOrientation);
	m_pChannelScale = new tmlnChannelFloatProperty( "Scale", m_pIcon->PropertyScale() );
	this->AddChannel(m_pChannelScale);
	m_pChannelVisible = new tmlnChannelBooleanProperty( "Visible", m_pIcon->PropertyVisible() );
	this->AddChannel(m_pChannelVisible);
	m_pChannelSound = this->AddChannel("Sound");

	m_pAdapterGetPosition = new chtrAdapterGetPosition( m_pIcon->GetEntity() );

	// Add channels for expressions
	//
	if (m_pExpressions)
	{
		const int nExpressions = m_pExpressions->GetNumExpressions();
		for (int e=0; e<nExpressions; e++)
		{
			std::string name = m_pExpressions->GetExpressionName(e);
			tmlnChannelRangedFloat *channel_exp = m_pExpressions->CreateChannel( name );
			this->AddChannel(channel_exp);
		}
	}
	else
	{
		if (m_pExpressions == NULL)
		{
			m_pExpressions = new chtrExpressionObject(m_pIcon->GetEntity(), i_ModelFile);
		}
		m_pExpressions->SetParent( m_pIcon );
	}

	// Add name callback in order to notify the layer manager
	m_pIcon->PropertyName().AddCallback(new prtyCallbackWrapper<chtrScriptObject>(this, &chtrScriptObject::NameChanged));
	
	// get directory from model file
	fsLocator dir(i_ModelFile);
	dir.Pop();
	SetDirectory( dir );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chtrScriptObject::~chtrScriptObject()
{
	//DBG_LOG("In chtrScriptObject destructor");

	// Timeline stuff
	tmlnTimelineMgr::RemoveObject(this);

	// Unregister from layers
	lyerLayerMgr::RemoveObject(m_pIcon, this);
	grpsGroupMgr::RemoveObject(m_pIcon, m_pIcon);

	delete m_pAdapterGetPosition;
	delete m_pIcon;

	if (m_pExpressions) 
		delete m_pExpressions;

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
std::string chtrScriptObject::GetTmlnName() const
{
	return m_pIcon->GetName().GetString();
}

//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void chtrScriptObject::GetResourceList( fsResourceTrackerData& io_List )
{
	io_List.Add( this->GetBaseData().m_Filename.GetValue() );

	// add in material resources (textures used by overriden materials)
	mtrlScriptObject::GetMaterialResources( io_List );

	// add in fragment resources (textures used by AO)
	fgmtScriptObject::GetFragmentResources( io_List );

	//	add all the driver resources as well
	tmlnScriptObject::GetResourceList( io_List );
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
////virtual 
//std::string chtrScriptObject::GetPick3dName() const
//{
//	return m_pIcon->GetName().GetString();
//}

//--------------------------------------------------------------------
//	Name - simply pass functions to icon object
//--------------------------------------------------------------------
void chtrScriptObject::SetName(const nameString& i_Name)
{
    m_pIcon->SetName( i_Name );
}
const nameString& chtrScriptObject::GetName() const
{
	return m_pIcon->GetName();
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the chtrScriptObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool chtrScriptObject::RayPick(	const maPoint3d& i_RayStart,
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
void chtrScriptObject::ShowIcons(bool i_bVisible)
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
void chtrScriptObject::SetSelected(bool i_bSelected)
{
	m_bSelected = i_bSelected;
	bool editor_visible = m_pIcon->GetEditorVisible();
	this->ShowDriverIcons(m_bShowDriverIcons && editor_visible 
		&& this->GetLayerPickable() && m_bSelected);
}

//--------------------------------------------------------------------
//  Changes visible state of character 
//--------------------------------------------------------------------
void  chtrScriptObject::SetEditorVisible(bool i_bVisible)
{
	m_pIcon->SetEditorVisible(i_bVisible);
	this->ShowDriverIcons(i_bVisible && m_bShowDriverIcons 
		&& this->GetLayerPickable() && m_bSelected);
}

//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
//virtual 
void chtrScriptObject::SetLayerVisible(bool i_bVisible)
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
void chtrScriptObject::SetLayerPickable(bool i_bPickable)
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
void chtrScriptObject::SetLayerWireframe(bool i_bWireframe)
{
	m_pIcon->SetWireframe(i_bWireframe);
}

//--------------------------------------------------------------------
//	LayerLowRes represents if the objects are rendered
//	using a low resolution model.
//--------------------------------------------------------------------
//virtual 
void chtrScriptObject::SetLayerLowRes(bool i_bLowRes)
{
	m_pIcon->SetLowRes(i_bLowRes);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrObject* chtrScriptObject::GetPickObject() const
{
	return m_pIcon;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const chtrExpressionObject*	chtrScriptObject::GetExpressionObject() const
{
	return m_pExpressions;
}
chtrExpressionObject* chtrScriptObject::ExpressionObject()
{
	return m_pExpressions;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrScriptObject::AddSingleExpression( const std::string& i_ExpressionName, const itString& i_ExpressionAnimFileName )
{
	try
	{
		//	add the expression
		m_pExpressions->AddSingleProperty( i_ExpressionName, i_ExpressionAnimFileName );

		//	add the channel
		//
		tmlnChannelRangedFloat *channel_exp = m_pExpressions->CreateChannel( i_ExpressionName );
		DBG_ASSERT0(channel_exp != NULL, "Channel is null cannot add it");
		this->AddChannel(channel_exp);
	}
	catch ( fsInvalidLocatorX& /*i_Ex*/ )
	{
		DBG_ERROR( "Can't find the expression " << i_ExpressionName << " " << i_ExpressionAnimFileName );
		return;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrScriptObject::AddDualExpression( const std::string& i_ExpressionName, const itString& i_ExpressionAnimFileNameLeft, const itString& i_ExpressionAnimFileNameRight )
{
	try
	{
		//	add the expression
		m_pExpressions->AddDualProperty( i_ExpressionName, i_ExpressionAnimFileNameLeft, i_ExpressionAnimFileNameRight );

		//	add the channel
		//
		tmlnChannelRangedFloat *channel_exp = m_pExpressions->CreateChannel( i_ExpressionName );
		DBG_ASSERT0(channel_exp != NULL, "Channel is null cannot add it");
		this->AddChannel(channel_exp);
	}
	catch ( fsInvalidLocatorX& /*i_Ex*/ )
	{
		DBG_ERROR( "Can't find the expressions " << i_ExpressionName.c_str() << " " << i_ExpressionAnimFileNameLeft << " " << i_ExpressionAnimFileNameRight );
		return;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrScriptObject::AddQuadExpression( const std::string& i_ExpressionName, const itString& i_ExpressionAnimFileNameLeft, const itString& i_ExpressionAnimFileNameRight, const itString& i_ExpressionAnimFileNameUp, const itString& i_ExpressionAnimFileNameDown )
{
	try
	{
		//	add the expression
		m_pExpressions->AddQuadProperty( i_ExpressionName, i_ExpressionAnimFileNameLeft, i_ExpressionAnimFileNameRight, i_ExpressionAnimFileNameUp, i_ExpressionAnimFileNameDown );

		//	add the channel
		//
		tmlnChannelRangedFloat *channel_exp = m_pExpressions->CreateChannel( i_ExpressionName );
		DBG_ASSERT0(channel_exp != NULL, "Channel is null cannot add it");
		this->AddChannel(channel_exp);
	}
	catch ( fsInvalidLocatorX& /*i_Ex*/ )
	{
		DBG_ERROR( "Can't find the expressions " << i_ExpressionName.c_str() << " " << i_ExpressionAnimFileNameLeft << " " << i_ExpressionAnimFileNameRight << " " << i_ExpressionAnimFileNameUp << " " << i_ExpressionAnimFileNameDown );
		return;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrScriptObject::DeleteExpression( const std::string& i_ExpressionName )
{
	DBG_ASSERT1(m_pExpressions != NULL, "Cannot delete expression (%s), there is not expression object", i_ExpressionName.c_str() );
	
	//	delete the expression
	m_pExpressions->DeleteExpression( i_ExpressionName );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int chtrScriptObject::GetNumExpressions() const
{
	if (m_pExpressions != NULL)
	{
		return m_pExpressions->GetNumExpressions();
	}
	else
	{
		return 0;
	}
}

//--------------------------------------------------------------------
//	filename
//--------------------------------------------------------------------
void chtrScriptObject::GetFilename( itString& o_Filename ) const
{
	m_pIcon->GetFilename(o_Filename);
}

//--------------------------------------------------------------------
//	Directory
//--------------------------------------------------------------------
void chtrScriptObject::SetDirectory( const fsLocator& i_Dir )
{
	m_Directory = i_Dir;
}
const fsLocator& chtrScriptObject::GetDirectory() const
{
	return m_Directory;
}

//--------------------------------------------------------------------
// Return the channel for the expression with the given name
//--------------------------------------------------------------------
tmlnChannelRangedFloat* chtrScriptObject::GetExpressionChannel(const std::string& i_Name)
{
	return m_pExpressions->GetExpressionChannel( i_Name );
}

//--------------------------------------------------------------------
// If this channel is an expression channel, return true and
//	set o_Name to the name of the expression.
//--------------------------------------------------------------------
bool chtrScriptObject::GetNameFromExpressionChannel(tmlnChannelRangedFloat* i_pChannel,
													std::string& o_Name)
{
	return m_pExpressions->GetNameFromExpressionChannel( i_pChannel, o_Name );
}

//--------------------------------------------------------------------
// This is called when a driver in this script object is changed,
//	or if a driver is added or deleted from this object.
//--------------------------------------------------------------------
void chtrScriptObject::NotifyDriverChanged()
{
	chtrOperations::ChangeDriverData(this->GetScriptData());
}

//--------------------------------------------------------------------
// OverrideMaterials is used by the user interface. It just calls 
// GatherMaterials; but since it is virtual, it can also set a 
// dirty bit on the chunk and update the object part interface.
//--------------------------------------------------------------------
void chtrScriptObject::OverrideMaterials()
{
	mtrlScriptObject::OverrideMaterials();

	// If we were to call this notify right away, it would alter the
	// control that is causing this callback. 
	mnmThinkMgr::CallFunctionDelayed(&chtrDialogDataUtil::UpdateListDialog);
	chtrDocumentChunk::ActiveDataChanged();
	sel3dMgr::Renotify();
}

//--------------------------------------------------------------------
//	Lock the materials
//--------------------------------------------------------------------
void chtrScriptObject::LockMaterials(const bool i_bLock)
{
	mtrlScriptObject::LockMaterials(i_bLock);

	chtrData data = GetBaseData();
	data.m_bLockedMaterials.SetValue(i_bLock);
	SetBaseData(data);
}

//--------------------------------------------------------------------
// Set subdivision level being used.
//--------------------------------------------------------------------
void chtrScriptObject::SetSubdivLevel(int i_SubdivLevel)
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
chtrScriptData chtrScriptObject::GetScriptData() const
{
	chtrScriptData data( m_pIcon->GetData() );

	data.m_BaseData.m_Position	= m_pChannelPosition->GetOriginalPosition();
	float x=0,y=0,z=0;
	m_pChannelOrientation->GetOriginalValue(x,y,z);
	data.m_BaseData.m_Orientation.SetEuler(x,y,z);
	data.m_BaseData.m_Scale		= m_pChannelScale->GetOriginalValue();

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

	// get the expression info
	if (m_pExpressions != NULL)
	{
		m_pExpressions->GetData( data.m_Expressions );
	}
		
	return data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void chtrScriptObject::SetScriptData(const chtrScriptData &i_Data)
{
	this->SetBaseData(i_Data.m_BaseData);

	//DBG_LOG2( "Set Prop Data %02d (%s)", i_Data.m_Name.GetUID(), i_Data.m_Name.GetString().c_str() );
	//DBG_LOG2( "              %02d (%s)", this->GetName().GetUID(), this->GetName().GetString().c_str() );

	// set control data
	this->SetControlData( i_Data.m_Controls );

	// set material data
	this->SetMaterialData( i_Data.m_Materials );

	// set fragment data
	itString modelName = i_Data.m_BaseData.m_Filename.GetValue();
	modelName.StripExtension();
	this->SetFragmentData( i_Data.m_Fragments, i_Data.m_AOData, itStringUtil::GetStdString(modelName) );

	//
	this->SetEditorVisible( i_Data.m_BaseData.m_bEditorVisible.GetValue() );

	// set the channels 
	this->ChannelSet().SetChannelInfo( i_Data.m_ChannelInfo );

	// set the expression info
	if (m_pExpressions != NULL)
	{
		m_pExpressions->SetData( i_Data.m_Expressions );

		//	add the channels
		// 
		for (int i = 0; i < i_Data.m_Expressions.size(); ++i)
		{
			if (!(this->ChannelExists(i_Data.m_Expressions[i]->m_Name.GetValue().c_str())))
			{
				tmlnChannelRangedFloat *channel_exp = m_pExpressions->CreateChannel( i_Data.m_Expressions[i]->m_Name.GetValue() );
				DBG_ASSERT0(channel_exp != NULL, "Channel is null cannot add it");
				this->AddChannel(channel_exp);
			}
		}
	}

	// create drivers from info
	tmlnCreator::SetDriverInfo( this, (i_Data.m_Drivers) );

	//DBG_LOG2( "Set Character Data %02d (%s)", i_Data.m_Name.GetUID(), i_Data.m_Name.GetString().c_str() );
	//DBG_LOG2( "              %02d (%s)", this->GetName().GetUID(), this->GetName().GetString().c_str() );
}

//--------------------------------------------------------------------
// Get values as a base data structure
//--------------------------------------------------------------------
chtrData chtrScriptObject::GetBaseData() const
{
	chtrData data		= m_pIcon->GetData();

	data.m_Position		= m_pChannelPosition->GetOriginalPosition();
	float x=0,y=0,z=0;
	m_pChannelOrientation->GetOriginalValue(x,y,z);
	data.m_Orientation.SetEuler(x,y,z);
	data.m_Scale		= m_pChannelScale->GetOriginalValue();

	return data;
}

//--------------------------------------------------------------------
// Set from base data structure
//--------------------------------------------------------------------
void chtrScriptObject::SetBaseData(const chtrData &i_Data)
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);

	mtrlScriptObject::LockMaterials(i_Data.m_bLockedMaterials.GetValue());

	m_pChannelPosition->SetOriginalPosition(i_Data.m_Position.GetValue());
	float x=0,y=0,z=0;
	i_Data.m_Orientation.GetEuler(x,y,z);
	m_pChannelOrientation->SetOriginalValue(x,y,z);
	m_pChannelScale->SetOriginalValue(i_Data.m_Scale.GetValue());
}

//--------------------------------------------------------------------
//	Select object part, like surface or material
//--------------------------------------------------------------------
void chtrScriptObject::SelectObjectPart(const std::string i_PartName, 
									  const std::string i_CategoryName, 
									  bool i_bAppend)
{
	bool local_part = true;
	pick3dPickObject *pPart = NULL;

	if (i_CategoryName == chtrPartConstants::c_SurfaceCategoryName)
	{
		if (this->GetNumFragments() > 0)
			pPart = this->GetFragmentUI(i_PartName);
	}
	else if (i_CategoryName == chtrPartConstants::c_MaterialCategoryName)
	{
		if (this->GetNumMaterials() > 0)
			pPart = this->GetMaterialUI(i_PartName);
	}
	else if (i_CategoryName == chtrPartConstants::c_ControlCategoryName)
	{
		if (this->GetNumControls() > 0)
			pPart = this->GetControlUI(i_PartName);
	}
	else if (i_CategoryName == chtrPartConstants::c_ExpressionCategoryName)
	{
		local_part = false;
		if (this->GetNumExpressions() > 0)
		{
			int index = m_pExpressions->GetExpression(i_PartName);
			if (index != -1)
				pPart = m_pExpressions->GetExpressionUI(index);
		}
	}

	//if (local_part)
	{
		if (pPart && (sel3dMgr::GetSelected() != pPart))
		{
			sel3dMgr::CreateUndoOperation();
			if (i_bAppend)
				sel3dMgr::AddToSelection(pPart);
			else
				sel3dMgr::Select(pPart);
		}
	}
	//else
	//{
	//}
}

//--------------------------------------------------------------------
//	ActivateObjectPart from Placed, represents a double-click
//		on a part item in the placed menu.
//--------------------------------------------------------------------
void chtrScriptObject::ActivateObjectPart(const std::string i_PartName, 
										  const std::string i_CategoryName)
{
	// Activate is used to activate the "override" operation for materials
	// or fragments
	if (i_CategoryName == chtrPartConstants::c_SurfaceCategoryName)
	{
		if (this->GetNumFragments() == 0)
		{
			fgmtOperations::OverrideFragments(this);
			// If we were to call this notify right away, it would alter the
			// control that is causing this callback. 
			mnmThinkMgr::CallFunctionDelayed(&chtrDialogDataUtil::UpdateListDialog);
			chtrDocumentChunk::ActiveDataChanged();	
		}
	}
	else if (i_CategoryName == chtrPartConstants::c_MaterialCategoryName)
	{
		if (this->GetNumMaterials() == 0)
		{
			mtrlOperations::OverrideMaterials(this);
		}
	}
	else if (i_CategoryName == chtrPartConstants::c_ControlCategoryName)
	{
		if (i_PartName == chtrPartConstants::c_NewControlString)
		{
			dynOperations::CreateNewControl(this);
			// If we were to call this notify right away, it would alter the
			// control that is causing this callback. 
			mnmThinkMgr::CallFunctionDelayed(&chtrDialogDataUtil::UpdateListDialog);
			chtrDocumentChunk::ActiveDataChanged();
		}
	}
	else if (i_CategoryName == chtrPartConstants::c_ExpressionCategoryName)
	{
		if (i_PartName == chtrPartConstants::c_NewExpressionSingleString)
		{
			chtrExpressionOperations::CreateSingleExpression(this);

			// If we were to call this notify right away, it would alter the
			// Expression that is causing this callback. 
			mnmThinkMgr::CallFunctionDelayed(&chtrDialogDataUtil::UpdateListDialog);
			chtrDocumentChunk::ActiveDataChanged();
		}
		else if (i_PartName == chtrPartConstants::c_NewExpressionDualString)
		{
			chtrExpressionOperations::CreateDualExpression(this);

			// If we were to call this notify right away, it would alter the
			// Expression that is causing this callback. 
			mnmThinkMgr::CallFunctionDelayed(&chtrDialogDataUtil::UpdateListDialog);
			chtrDocumentChunk::ActiveDataChanged();
		}
		else if (i_PartName == chtrPartConstants::c_NewExpressionQuadString)
		{
			chtrExpressionOperations::CreateQuadExpression(this);

			// If we were to call this notify right away, it would alter the
			// Expression that is causing this callback. 
			mnmThinkMgr::CallFunctionDelayed(&chtrDialogDataUtil::UpdateListDialog);
			chtrDocumentChunk::ActiveDataChanged();
		}
	}
}

//--------------------------------------------------------------------
//	DeleteObjectPart from Placed, represents DELETE key or button
//		when a part item is selected in the placed menu.
//--------------------------------------------------------------------
void chtrScriptObject::DeleteObjectPart(const std::string i_PartName, 
										const std::string i_CategoryName)
{
	if (i_CategoryName == "Controls")
	{
		// control is a selectable object, needs to be removed from selection
		sel3dMgr::ClearSelection(); 

		dynOperations::DeleteControl(this, i_PartName);

		// If we were to call this notify right away, it would alter the
		// control that is causing this callback. 
		mnmThinkMgr::CallFunctionDelayed(&chtrDialogDataUtil::UpdateListDialog);
		chtrDocumentChunk::ActiveDataChanged();
	}
	else if (i_CategoryName == "Expressions")
	{
		// Expression is a selectable object, needs to be removed from selection
		sel3dMgr::ClearSelection(); 

		chtrExpressionOperations::DeleteExpression(this, i_PartName);

		// If we were to call this notify right away, it would alter the
		// Expression that is causing this callback. 
		mnmThinkMgr::CallFunctionDelayed(&chtrDialogDataUtil::UpdateListDialog);
		chtrDocumentChunk::ActiveDataChanged();
	}
}

//--------------------------------------------------------------------
// Return adapter to access point light's position
//--------------------------------------------------------------------
chtrAdapterGetPosition& chtrScriptObject::AdapterGetPosition()
{
	return (*m_pAdapterGetPosition);
}


//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
tmlnChannelAnimationFull& chtrScriptObject::ChannelAnimationFull()
{
	return (*m_pChannelAnimationFull);
}
chtrChannelAnimationSub& chtrScriptObject::ChannelAnimationSub()
{
	return (*m_pChannelAnimationSub);
}
tmlnChannelOrientation& chtrScriptObject::ChannelOrientation()
{
	return (*m_pChannelOrientation);
}
tmlnChannelPosition& chtrScriptObject::ChannelPosition()
{
	return (*m_pChannelPosition);
}
tmlnChannelFloat& chtrScriptObject::ChannelScale()
{
	return (*m_pChannelScale);
}
tmlnChannelBoolean& chtrScriptObject::ChannelVisible()
{
	return (*m_pChannelVisible);
}
tmlnChannel& chtrScriptObject::ChannelSound()
{
	return (*m_pChannelSound);
}

//--------------------------------------------------------------------
// This virtual function is called when the materials are saved 
//	to a new filename. The locator representing this geometry
//	should now point to the new filename.
//--------------------------------------------------------------------
//virtual 
void chtrScriptObject::NotifyLocatorChanged(const fsLocator& i_NewLocator)
{
	// Can't notify because it will cause a reload of the
	// character which is going to delete the object that
	// is calling this virtual function.
	itString filename = i_NewLocator.GetLastName();
	m_pIcon->SetFilenameWithoutNotify(filename);

	fsLocator dir(i_NewLocator);
	dir.Pop();
	this->SetDirectory(dir);

	//	handle the expression object
	//if (m_pExpressions != NULL)
	//{
	//	// TODO - handle the locator changing (necessary?)
	//	//m_pExpressions->
	//}
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void chtrScriptObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Update LayerMgr
	lyerLayerMgr::ObjectRenamed(m_pIcon);
	evmtEnvironmentMgr::ObjectRenamed(m_pIcon);
	grpsGroupMgr::ObjectRenamed(m_pIcon);
}

