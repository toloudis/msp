/*****************************************************************************
**  trfnTransformObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/Transforms/Object/trfnTransformObject.hpp"
#include "Systems/Transforms/GUI/trfnDialogDataUtil.hpp"
#include "Systems/Transforms/Data/trfnDocumentChunk.hpp"
#include "Systems/Transforms/Undo/trfnOperations.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/xfrm/xfrmApplyTransformUtil.hpp"
#include "Support/xfrm/xfrmTransformGroup.hpp"
#include "Support/xfrm/xfrmTransformMgr.hpp"

#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Graphics/G3d/g3dSceneNode.hpp"
#include "Graphics/Sc/scObject.hpp"
#include "Tool/api3d/api3dObjectNode.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gpx/gpxSceneObject.hpp"

#include <boost/bind.hpp>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
trfnTransformObject::trfnTransformObject()
:	m_pParent(NULL)
{	
	m_p3DNode = new api3dObjectNode();
	
	//	add it to the scene
	api3dScene::AddObject(m_p3DNode);

	// create thread-safe proxy
	m_p3DNodeProxy = new gpxSceneObject(*m_p3DNode);

	ltstLightSetMgr::AddObject(this, m_p3DNode);
	m_pTransform = xfrmTransformMgr::CreateTransform(this, m_p3DNode->Object()->GetBase(), this);

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Asset", "Name of the object");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bVisible), "Asset", "Animatable Visibility");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bPickable), "Asset", "Pickable in viewport");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bWireframe), "Asset", "Wireframe draw style");
	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bInheritsTransform), "Transform", "Inheirt parent transformation");
	AddProperty( pPUII );
	pPUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Position), "Transform", "Position of the object");
	AddProperty( pPUII );
	prtyVector3dEditUpDownUIInfo *pPVEUDUII  = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Orientation), "Transform", "Orientation of the object");
	pPVEUDUII->SetIncrement(0.5f, 0.5f, 0.5f);
	pPVEUDUII->SetDecimalPlaces(1);
	AddProperty( pPVEUDUII );		
	pPUII  = new prtyFloatEditUIInfo(&(m_Data.m_Scale), "Transform", "Scale of the object");
	AddProperty( pPUII );
	pPVEUDUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_PivotPoint), "Transform", "Pivot position in local space");
	pPVEUDUII->SetIncrement(0.1f, 0.1f, 0.1f);
	AddProperty( pPVEUDUII );

	// Register callbacks to update particle generator and icons
	// when properties change
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<trfnTransformObject>(this, &trfnTransformObject::NameChanged));
	m_Data.m_Position.AddCallback(new prtyCallbackWrapper<trfnTransformObject>(this, &trfnTransformObject::PositionChanged));
	m_Data.m_Orientation.AddCallback(new prtyCallbackWrapper<trfnTransformObject>(this, &trfnTransformObject::OrientationChanged));
	m_Data.m_Scale.AddCallback(new prtyCallbackWrapper<trfnTransformObject>(this, &trfnTransformObject::ScaleChanged));
	m_Data.m_PivotPoint.AddCallback(new prtyCallbackWrapper<trfnTransformObject>(this, &trfnTransformObject::PivotChanged));
	m_Data.m_PivotCompensation.AddCallback(new prtyCallbackWrapper<trfnTransformObject>(this, &trfnTransformObject::PivotCompensationChanged));
	m_Data.m_bVisible.AddCallback(new prtyCallbackWrapper<trfnTransformObject>(this, &trfnTransformObject::VisibleChanged));
	m_Data.m_bInheritsTransform.AddCallback(new prtyCallbackWrapper<trfnTransformObject>(this, &trfnTransformObject::InheritsTransformChanged));
	m_Data.m_bPickable.AddCallback(new prtyCallbackWrapper<trfnTransformObject>(this, &trfnTransformObject::PickableChanged));
	m_Data.m_bWireframe.AddCallback(new prtyCallbackWrapper<trfnTransformObject>(this, &trfnTransformObject::WireframeChanged));

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
trfnTransformObject::~trfnTransformObject()
{
	ltstLightSetMgr::RemoveObject(this, m_p3DNode);

	xfrmTransformMgr::DeleteTransform(this->GetName());

	//	remove it to the scene
	api3dScene::RemoveObject(m_p3DNode);

	delete m_p3DNodeProxy;
	delete m_p3DNode;
}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string trfnTransformObject::GetDisplayName() const
{
	return GetName().GetString();
}

//--------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//--------------------------------------------------------------------
//virtual 
//sel3dObject* trfnTransformObject::GetParentObject() const
//{
//	return m_pParent;
//}
//void trfnTransformObject::SetParentObject(sel3dObject* i_pPO)
//{
//	m_pParent = i_pPO;
//}


//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void trfnTransformObject::GetResourceList( fsResourceTrackerData& io_List )
{
	// no resources in transform
}

//--------------------------------------------------------------------
// Get values as a transform data structure
//--------------------------------------------------------------------
const trfnData& trfnTransformObject::GetData() const
{
	// Since we don't have a property for checked lists, the transform manager
	// still maintains the "true" object connectíons.
	// We have to refresh that info from the manager whenever
	// someone asks for the data.
	m_Data.m_Objects.clear();
	xfrmTransformMgr::GetNodesInTransform(this->GetName(), m_Data.m_Objects);

	return m_Data;
}

//--------------------------------------------------------------------
// Set from transform data structure
//--------------------------------------------------------------------
void trfnTransformObject::SetData(const trfnData &i_Data)
{
	xfrmTransformMgr::ClearTransform(m_Data.m_Name.GetValue());

	// This may change the name of the transform
	m_Data = i_Data;

	nameString set_name = m_Data.m_Name.GetValue();
	const int num_objects = m_Data.m_Objects.size();
	for (int i=0; i<num_objects; ++i)
	{
		// Don't want to preserve transform here because we are reconstructing an
		// existing hierarchy from loading a file.
		const bool c_bPreserveTransform = false; 
		xfrmTransformMgr::AddNodeToTransform(set_name, m_Data.m_Objects[i], c_bPreserveTransform);
	}
}

//--------------------------------------------------------------------
// Name property access
//--------------------------------------------------------------------
prtyName&	trfnTransformObject::PropertyName()
{
	return m_Data.m_Name;
}
const prtyName&	trfnTransformObject::GetPropertyName() const
{
	return m_Data.m_Name;
}

//--------------------------------------------------------------------
// Position property access
//--------------------------------------------------------------------
prtyPoint3d&	trfnTransformObject::PropertyPosition()
{
	return m_Data.m_Position;
}
const prtyPoint3d&	trfnTransformObject::GetPropertyPosition() const
{
	return m_Data.m_Position;
}

//--------------------------------------------------------------------
// Orientation property access
//--------------------------------------------------------------------
prtyRotation&	trfnTransformObject::PropertyOrientation()
{
	return m_Data.m_Orientation;
}
const prtyRotation&	trfnTransformObject::GetPropertyOrientation() const
{
	return m_Data.m_Orientation;
}

//--------------------------------------------------------------------
// Scale property access
//--------------------------------------------------------------------
prtyFloat&	trfnTransformObject::PropertyScale()
{
	return m_Data.m_Scale;
}
const prtyFloat&	trfnTransformObject::GetPropertyScale() const
{
	return m_Data.m_Scale;
}

//--------------------------------------------------------------------
// Visible property access
//--------------------------------------------------------------------
prtyBoolean&	trfnTransformObject::PropertyVisible()
{
	return m_Data.m_bVisible;
}
const prtyBoolean&	trfnTransformObject::GetPropertyVisible() const
{
	return m_Data.m_bVisible;
}

//----------------------------------------------------------------------------
//	GetWorldBox returns a box which completely encloses the trfnTransformObject.
//----------------------------------------------------------------------------
maAxisBox trfnTransformObject::GetWorldBox(int i_IconLayerIndex) const
{
	return m_p3DNodeProxy->GetWorldBox();
}

//--------------------------------------------------------------------
//	GetWorldPivot returns the point in world space that this
//		object will rotate around. Used to center rotation and
//		scale compasses.
//--------------------------------------------------------------------
//virtual 
maPoint3d trfnTransformObject::GetWorldPivot() const
{
	// pivot point in object space is just local pivot plus position
	maPoint3d pivot(m_p3DNodeProxy->GetPivotPoint() + m_p3DNodeProxy->GetPivotCompensation() + this->GetPosition());

	// transform to world space
	maMatrix4x4 parent_matrix;
	this->GetParentMatrix(parent_matrix);
	parent_matrix.Transform(pivot);
	return pivot;
}

//--------------------------------------------------------------------
//  Get sum of matrices of all parents of this node. 
//--------------------------------------------------------------------
//virtual 
void trfnTransformObject::GetParentMatrix(maMatrix4x4 &o_Transformation) const
{
	// let transform manager compute the sum of the parent matrices
	xfrmTransformMgr::ComputeExclusiveMatrixForNode(this->GetName(), o_Transformation);
}


//--------------------------------------------------------------------
// Change the pivot point of this object to the center 
// of its bounding box
//--------------------------------------------------------------------
//virtual 
void trfnTransformObject::CenterPivot()
{
	// Going to directly access the scene graph here, so stop any running threads	
	gpxRenderControl::ConfirmSingleThread();

	maPoint3d pivot_pt;

	// This call will update the scene graph transformations
	maAxisBox bbox = m_p3DNode->GetWorldBox();
	if (!bbox.IsEmpty())
		pivot_pt += bbox.GetCenter(); // this is the goal pivot point in world space

	// Have to also incorporate the icon centers of child nodes
	maPoint3d child_pvt(0,0,0);
	bool bIconPivot = xfrmTransformMgr::GetIconPivotPoint(*m_pTransform, child_pvt);
	if (bIconPivot)
		pivot_pt += child_pvt;

	// Average pivot points if we had both
	if (!bbox.IsEmpty() && bIconPivot)
		pivot_pt *= 0.5f;

	// Get full matrix at this node in order to convert from world to local space
	maMatrix4x4 total_matrix = m_p3DNode->GetObject()->GetBase()->GetTotalTransform();
	total_matrix.Invert();
	total_matrix.Transform(pivot_pt);

	// Create undo operation and set value
	this->CreateUndoForProperty(m_Data.m_PivotPoint);
	const bool bSetDirty = true;
	m_Data.m_PivotPoint.SetValue(pivot_pt, bSetDirty);
}

//----------------------------------------------------------------------------
//	Position
//----------------------------------------------------------------------------
maPoint3d trfnTransformObject::GetPosition() const
{
	return m_Data.m_Position.GetValue();
}
void trfnTransformObject::UpdatePosition(const maPoint3d& i_Position, 
								bool i_bNewOperation)
{
	if (i_bNewOperation)
		this->CreateUndoForProperty(m_Data.m_Position);
	const bool bSetDirty = true;
	m_Data.m_Position.SetValue(i_Position, bSetDirty);
}


//----------------------------------------------------------------------------
//	Orientation
//----------------------------------------------------------------------------
maRotation trfnTransformObject::GetOrientation() const
{
	return m_Data.m_Orientation.GetQuaternion();
}
void trfnTransformObject::UpdateOrientation(const maRotation& i_Orientation, 
										bool i_bNewOperation)
{
	if (i_bNewOperation)
		this->CreateUndoForProperty(m_Data.m_Orientation);
	const bool bSetDirty = true;
	m_Data.m_Orientation.SetQuaternion(i_Orientation, bSetDirty);
}

//----------------------------------------------------------------------------
//	Scale
//----------------------------------------------------------------------------
maPoint3d trfnTransformObject::GetScale() const
{
	return m_p3DNodeProxy->GetScale();
}
void trfnTransformObject::UpdateScale(const maPoint3d& i_Scale, 
								bool i_bNewOperation)
{
	if (i_bNewOperation)
		this->CreateUndoForProperty(m_Data.m_Scale);
	// Uniform Scale, just take off the X value
	const bool bSetDirty = true;
	m_Data.m_Scale.SetValue(i_Scale.m_X, bSetDirty);
}

//----------------------------------------------------------------------------
// Flags for how object can be modified
//----------------------------------------------------------------------------
mnmObject::RotateFlags	trfnTransformObject::GetRotateFlags()
{
	return mnmObject::e_RotateAll;
}
mnmObject::ScaleFlags	trfnTransformObject::GetScaleFlags()
{
	return mnmObject::e_ScaleUniform;
}
mnmObject::TranslateFlags	trfnTransformObject::GetTranslateFlags()
{
	return mnmObject::e_TranslateAll;
}

//--------------------------------------------------------------------
// Set object active state in the render layer
//--------------------------------------------------------------------
//void trfnTransformObject::SetActiveRenderLayer(bool i_bActive)
//{
//	m_bLayerVisible = i_bActive;
//	m_p3DNodeProxy->SetActiveFromRenderLayer(i_bActive);
//}

//--------------------------------------------------------------------
//  Changes visible state of node and its children based on GUI
//--------------------------------------------------------------------
void  trfnTransformObject::SetEditorVisible(bool i_bVisible)
{
	// Transforms don't have a "editor visible" state themselves,
	// but they do pass the visible state to all child nodes.
	xfrmTransformMgr::SetChildrenVisibleInEditor(*m_pTransform, i_bVisible);

//	m_Data.m_bEditorVisible.SetValue(i_bVisible);
//
//	// object is visible only if channel and gui and layer are all
//	// are set visible == true;
//	m_p3DNodeProxy->SetActiveFromSceneMgr(i_bVisible);
//	m_p3DNodeProxy->SetRenderable(m_Data.m_bVisible.GetValue() 
//							&& m_Data.m_bEditorVisible.GetValue()); 
//							//&& m_bLayerVisible);
}
//bool trfnTransformObject::GetEditorVisible() const
//{
//	return m_Data.m_bEditorVisible.GetValue();
//}

//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
void trfnTransformObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
    nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number)
    m_Data.m_Name.SetValue(this->GetName());
}

//--------------------------------------------------------------------
// Called from transformation manager once per frame, update camera
//	position based on parent transformation matrix.
//--------------------------------------------------------------------
void trfnTransformObject::UpdateParentTransform()
{
	// Give the property the transformation matrix and let them compute
	// the object and world space as needed.
	maMatrix4x4 parent_matrix;
	this->GetParentMatrix(parent_matrix);
	m_Data.m_Position.SetTransformation(parent_matrix);
}

//--------------------------------------------------------------------
// Called from transformation manager when the parenting of this 
// object changes in a way that we need to alter our values to
// saty in the same world position.
//--------------------------------------------------------------------
void trfnTransformObject::ApplyTransformation(const maMatrix4x4& i_Matrix)
{
	xfrmApplyTransformUtil::ApplyTransformation(i_Matrix,
												m_Data.m_Position,
												m_Data.m_Orientation,
												m_Data.m_Scale,
												m_Data.m_PivotPoint,
												m_Data.m_PivotCompensation);
}


//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void trfnTransformObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// The property's check will prevent an infinite
	// loop here.
	this->SetName( m_Data.m_Name.GetValue() );

	if (i_bDirty)
	{
		trfnDialogDataUtil::UpdateListDialog();
		trfnDocumentChunk::ActiveDataChanged();
	}
}

void trfnTransformObject::PositionChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	const maPoint3d& position = m_Data.m_Position.GetValue();
	m_p3DNodeProxy->SetPosition( position );

	// Update our local transformation matrix into the xfrmTransformGroup
	maMatrix4x4 node_mtx;
	m_p3DNodeProxy->GetTransformation(node_mtx);
	m_pTransform->SetTransformation(node_mtx);

	if (i_bDirty)
		trfnDocumentChunk::ActiveDataChanged();
}
void trfnTransformObject::OrientationChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	const maRotation& orientation = m_Data.m_Orientation.GetQuaternion();
	m_p3DNodeProxy->SetOrientation( orientation );

	// Update our local transformation matrix into the xfrmTransformGroup
	maMatrix4x4 node_mtx;
	m_p3DNodeProxy->GetTransformation(node_mtx);
	m_pTransform->SetTransformation(node_mtx);

	if (i_bDirty)
		trfnDocumentChunk::ActiveDataChanged();
}
void trfnTransformObject::ScaleChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	m_p3DNodeProxy->SetUniformScale( m_Data.m_Scale.GetValue() );

	// Update our local transformation matrix into the xfrmTransformGroup
	maMatrix4x4 node_mtx;
	m_p3DNodeProxy->GetTransformation(node_mtx);
	m_pTransform->SetTransformation(node_mtx);

	if (i_bDirty)
		trfnDocumentChunk::ActiveDataChanged();
}
void trfnTransformObject::PivotChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	const maPoint3d& pivot = m_Data.m_PivotPoint.GetValue();
	if (i_bDirty)
	{
		const bool preserve_transformation = true;	// preserve transformation if coming from GUI
		m_p3DNodeProxy->SetPivotPoint( pivot, preserve_transformation );

		// When preserving transformation, the "pivot compensation" value will be updated.
		// So, we need to set that value into our data also
		m_Data.m_PivotCompensation = m_p3DNodeProxy->GetPivotCompensation();

		// Update our local transformation matrix into the xfrmTransformGroup
		maMatrix4x4 node_mtx;
		m_p3DNodeProxy->GetTransformation(node_mtx);
		m_pTransform->SetTransformation(node_mtx);

		trfnDocumentChunk::ActiveDataChanged();
	}
	else
	{
		const bool dont_preserve_transformation = false;	// preserve transformation if coming from GUI
		m_p3DNodeProxy->SetPivotPoint( pivot, dont_preserve_transformation );

		// Update our local transformation matrix into the xfrmTransformGroup
		maMatrix4x4 node_mtx;
		m_p3DNodeProxy->GetTransformation(node_mtx);
		m_pTransform->SetTransformation(node_mtx);
	}
}
void trfnTransformObject::PivotCompensationChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	const maVector3d& vec = m_Data.m_PivotCompensation.GetValue();
	m_p3DNodeProxy->SetPivotCompensation( vec );

	if (i_bDirty)
		trfnDocumentChunk::ActiveDataChanged();
}
void trfnTransformObject::InheritsTransformChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_pTransform->SetInheritsTransform( m_Data.m_bInheritsTransform.GetValue() );
	m_p3DNodeProxy->SetInheritsTransform( m_Data.m_bInheritsTransform.GetValue() );

	if (i_bDirty)
		trfnDocumentChunk::ActiveDataChanged();
}
void trfnTransformObject::VisibleChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	m_p3DNodeProxy->SetRenderable(m_Data.m_bVisible.GetValue() );
		//&& m_Data.m_bEditorVisible.GetValue() );
}
void trfnTransformObject::PickableChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	m_p3DNodeProxy->SetGPUPickable( m_Data.m_bPickable.GetValue() );
	m_pTransform->SetPickable( m_Data.m_bPickable.GetValue() );
}
void trfnTransformObject::WireframeChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	m_p3DNodeProxy->SetWireframe(m_Data.m_bWireframe.GetValue() );
}
