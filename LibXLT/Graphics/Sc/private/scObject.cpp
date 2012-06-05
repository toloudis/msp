/*****************************************************************************
**	scObject.hpp
**
**		scObject defines a base class scObject, which represents an object
**	in a 3d scene.  The scObjects provide a bounding box and a "Renderable"
**	flag.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/sc/scObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/sc/scControlAnim.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	// set_fogged - recursively sets fog on all children
	//--------------------------------------------------------------------
	void set_fogged( bool i_bFog, g3dSceneNode* i_pSceneNode )
	{
		i_pSceneNode->SetFogged( i_bFog );

		int nChildren = i_pSceneNode->GetNumChildren();
		for( int i = 0; i < nChildren; ++i )
		{
			set_fogged( i_bFog, i_pSceneNode->GetChild( i ) );
		}
	}
}


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
scObject::scObject()
:	m_pBase( new g3dSceneNode ),
	m_Position( 0.0f, 0.0f, 0.0f ),
	m_Scale( 1.0f, 1.0f, 1.0f ),
	m_bExpired( false ),
	m_nPeriod( 1 ),
	m_nDelayToThink( 0 ),
	m_Pivot( 0.0f, 0.0f, 0.0f ),
	m_PivotCompensation( 0.0f, 0.0f, 0.0f ),
	m_bHasPivotPoint( false )
{
}

//--------------------------------------------------------------------
//  Constructor - sets the fragment of the base scene node
//	Does not own fragment
//--------------------------------------------------------------------
scObject::scObject( g3dFragment* i_pFragment )
:	m_pBase( new g3dSceneNode ),
	m_Position( 0.0f, 0.0f, 0.0f ),
	m_Scale( 1.0f, 1.0f, 1.0f ),
	m_bExpired( false ),
	m_nPeriod( 1 ),
	m_nDelayToThink( 0 ),
	m_Pivot( 0.0f, 0.0f, 0.0f ),
	m_PivotCompensation( 0.0f, 0.0f, 0.0f ),
	m_bHasPivotPoint( false )
{
	m_pBase->SetFragment( i_pFragment );
}

//--------------------------------------------------------------------
//  Constructor - uses given node as root node, takes ownership.
// The given node should not have any custom settings yet, the 
// scObject will overwrite its transform.
//--------------------------------------------------------------------
scObject::scObject( g3dSceneNode* i_pRootNode )
:	m_pBase( i_pRootNode ),
	m_Position( 0.0f, 0.0f, 0.0f ),
	m_Scale( 1.0f, 1.0f, 1.0f ),
	m_bExpired( false ),
	m_nPeriod( 1 ),
	m_nDelayToThink( 0 ),
	m_Pivot( 0.0f, 0.0f, 0.0f ),
	m_PivotCompensation( 0.0f, 0.0f, 0.0f ),
	m_bHasPivotPoint( false )
{
	// set transform to identity
	maMatrix4x4 identity;
	m_pBase->SetTransform( identity );
}

//--------------------------------------------------------------------
//  Destructor
//--------------------------------------------------------------------
scObject::~scObject()
{
	if( m_pBase->GetParent() )
	{
		m_pBase->GetParent()->RemoveChild(m_pBase);
	}

	delete m_pBase;

	envSTLHelpers::DeleteContainer(m_ControlAnims);
}

//--------------------------------------------------------------------
//  SetFragment - sets the fragment of the base scene node
//	Does not own fragment
//--------------------------------------------------------------------
void scObject::SetFragment( g3dFragment* i_pFragment )
{
	m_pBase->SetFragment( i_pFragment );
}

//--------------------------------------------------------------------
//	SetPosition changes the position of the object.
//--------------------------------------------------------------------
void scObject::SetPosition(const maPoint3d& i_Position)
{
	m_Position = i_Position;

	if (m_bHasPivotPoint)
	{
		this->update_full_transformation();
	}
	else
	{
		maMatrix4x4 xform = m_pBase->GetTransform();
		xform(3, 0) = i_Position.m_X;
		xform(3, 1) = i_Position.m_Y;
		xform(3, 2) = i_Position.m_Z;
		m_pBase->SetTransform(xform);
	}
}

//--------------------------------------------------------------------
//	SetOrientation changes the orientation of the object.
//--------------------------------------------------------------------
void scObject::SetOrientation(const maRotation& i_Orientation)
{
	m_Orientation = i_Orientation;
	
	if (m_bHasPivotPoint)
	{
		this->update_full_transformation();
	}
	else
	{
		maMatrix4x4 xform = m_pBase->GetTransform();
		maMatrix3x3 rot_mat = i_Orientation.GetMatrix3x3();
		xform(0, 0) = rot_mat(0, 0) * m_Scale.m_X;
		xform(0, 1) = rot_mat(0, 1) * m_Scale.m_X;
		xform(0, 2) = rot_mat(0, 2) * m_Scale.m_X;
		xform(1, 0) = rot_mat(1, 0) * m_Scale.m_Y;
		xform(1, 1) = rot_mat(1, 1) * m_Scale.m_Y;
		xform(1, 2) = rot_mat(1, 2) * m_Scale.m_Y;
		xform(2, 0) = rot_mat(2, 0) * m_Scale.m_Z;
		xform(2, 1) = rot_mat(2, 1) * m_Scale.m_Z;
		xform(2, 2) = rot_mat(2, 2) * m_Scale.m_Z;
		m_pBase->SetTransform(xform);
	}
}

//--------------------------------------------------------------------
//	SetPositionAndOrientation has the same effect as calling
//	SetPosition, then SetOrientation, but is a little more
//	efficient.
//--------------------------------------------------------------------
void scObject::SetScale(const maVector3d& i_Scale)
{
	m_Scale = i_Scale;
	SetPositionAndOrientation(m_Position, m_Orientation);
}

//--------------------------------------------------------------------
//	SetPositionAndOrientation has the same effect as calling
//	SetPosition, then SetOrientation, but is a little more
//	efficient.
//--------------------------------------------------------------------
void scObject::SetPositionAndOrientation(	const maPoint3d& i_Position,
											const maRotation& i_Orientation)
{
	m_Position = i_Position;
	m_Orientation = i_Orientation;

	if (m_bHasPivotPoint)
	{
		this->update_full_transformation();
	}
	else
	{
		maMatrix4x4 xform = m_pBase->GetTransform();
		xform(3, 0) = i_Position.m_X;
		xform(3, 1) = i_Position.m_Y;
		xform(3, 2) = i_Position.m_Z;

		maMatrix3x3 rot_mat = i_Orientation.GetMatrix3x3();
		xform(0, 0) = rot_mat(0, 0) * m_Scale.m_X;
		xform(0, 1) = rot_mat(0, 1) * m_Scale.m_X;
		xform(0, 2) = rot_mat(0, 2) * m_Scale.m_X;
		xform(1, 0) = rot_mat(1, 0) * m_Scale.m_Y;
		xform(1, 1) = rot_mat(1, 1) * m_Scale.m_Y;
		xform(1, 2) = rot_mat(1, 2) * m_Scale.m_Y;
		xform(2, 0) = rot_mat(2, 0) * m_Scale.m_Z;
		xform(2, 1) = rot_mat(2, 1) * m_Scale.m_Z;
		xform(2, 2) = rot_mat(2, 2) * m_Scale.m_Z;
		m_pBase->SetTransform(xform);
	}
}

//--------------------------------------------------------------------
//	SetPivotPoint changes the point around which the object rotates.
//		If i_bPreserveTransformation is true, then a compensating
//		translation is added to m_PivotCompensation in order to
//		make the total transformation matrix stay the same.
//--------------------------------------------------------------------
void scObject::SetPivotPoint(const maPoint3d& i_Position, 
							 bool i_bPreserveTransformation)
{
	// Compute how mush the pivot is moving, so that we can compensate
	//	when preserving position
	maVector3d offset = i_Position - m_Pivot;

	m_Pivot = i_Position;

	if (i_bPreserveTransformation)
	{
		// Need to push the delta in the pivot point through the matrix
		// and maintain it in a compensation translation at the end
		maVector3d xformed_offset(offset);
		m_pBase->GetTransform().TransformDir( xformed_offset );
		m_PivotCompensation += (xformed_offset - offset);

		// This flag lets us skip the pivot computations in most cases
		// where the pivot has not been set.
		if (m_Pivot != maPoint3d(0,0,0) || m_PivotCompensation != maPoint3d(0,0,0))
			m_bHasPivotPoint = true;

		// Since we are preserving the current transformation, we
		// don't need to recompute it here using 
		// update_full_transformation
	}
	else
	{
		// Not preserving transformation
		m_PivotCompensation.Set(0,0,0); // remove old compensation

		// This flag lets us skip the pivot computations in most cases
		// where the pivot has not been set.
		if (m_Pivot != maPoint3d(0,0,0) || m_PivotCompensation != maPoint3d(0,0,0))
			m_bHasPivotPoint = true;

		this->update_full_transformation();
	}
}


//--------------------------------------------------------------------
//	The pivot compensation is usually only set through calls to 
//	SetPivotPoint in which it is computed. But sometimes, you need
//	to set it directly in order to restore a transformation.
//--------------------------------------------------------------------
void scObject::SetPivotCompensation(const maPoint3d& i_Compensation)
{
	m_PivotCompensation = i_Compensation;

	// This flag lets us skip the pivot computations in most cases
	// where the pivot has not been set.
	if (m_Pivot != maPoint3d(0,0,0) || m_PivotCompensation != maPoint3d(0,0,0))
		m_bHasPivotPoint = true;

	this->update_full_transformation();
}

//--------------------------------------------------------------------
// Get transformation for the root node of the given object.
// This is the matrix for the position, scale, rotation,
// along with pivots stored in this object.
//--------------------------------------------------------------------
void scObject::GetTransformation(maMatrix4x4& o_Transformation)
{
	// If we had a dirty bit, then we would need to compute 
	// the full transformation. But, since the full_transformation
	// is computed often we can just reutrn it.
	o_Transformation = m_pBase->GetTransform();
}

//--------------------------------------------------------------------
//	GetWorldBox returns the bounding box
//--------------------------------------------------------------------
const maAxisBox& scObject::GetWorldBox() const
{
	return GetBase()->GetWorldBox();
}

//--------------------------------------------------------------------
//	PreRender is called by the scScene for each object before it
//	is rendered.  Objects can use this function to animate.
//--------------------------------------------------------------------
void scObject::Animate(float i_SimulationTime)
{
	int nanims = m_ControlAnims.size();
	for (int i=0; i<nanims; i++)
		m_ControlAnims[i]->Animate(i_SimulationTime);
}

//--------------------------------------------------------------------
//	Animate only the scene graph nodes, 
//  fragments should not be animated yet.
//	This is for preparation for attachments that need the
//	transformations in the nodes, but not the bounding boxes.
//--------------------------------------------------------------------
void scObject::AnimateMatrices(float i_SimulationTime)
{
	// default behavior is the same as Animate()
	this->Animate(i_SimulationTime);
}

//--------------------------------------------------------------------
//	SetExpired will cause IsExpired to return true, which will in
//	turn cause the object to be destroyed by the scObjectMgr.
//--------------------------------------------------------------------
void scObject::SetExpired()
{
	m_bExpired = true;
}

//--------------------------------------------------------------------
//	This SetRenderable is really just a helper that calls
//	the base node's SetRenderable.
//--------------------------------------------------------------------
void scObject::SetRenderable( bool i_bRender )
{
	m_pBase->SetRenderable( i_bRender );
}
bool scObject::GetRenderable() const
{
	return m_pBase->GetRenderable();
}

//--------------------------------------------------------------------
//	SetActiveInRenderLayer is just a helper that calls
// the base node's SetActiveInRenderLayer
//--------------------------------------------------------------------
void scObject::SetActiveInRenderLayer(bool i_bRenderable)
{
	m_pBase->SetActiveInRenderLayer(i_bRenderable);
}
bool scObject::GetActiveInRenderLayer() const
{
	return m_pBase->GetActiveInRenderLayer();
}

//--------------------------------------------------------------------
//	SetActiveInRenderLayer is just a helper that calls
// the base node's SetActiveInSceneMgr
//--------------------------------------------------------------------
void scObject::SetActiveInSceneMgr(bool i_bRenderable)
{
	m_pBase->SetActiveInSceneMgr(i_bRenderable);
}
bool scObject::GetActiveInSceneMgr() const
{
	return m_pBase->GetActiveInSceneMgr();
}

//--------------------------------------------------------------------
//	SetFogged is really just a helper that sets all the nodes of this
//	object to render without fog
//--------------------------------------------------------------------
void scObject::SetFogged( bool i_bFog )
{
	set_fogged( i_bFog, m_pBase );
}

//--------------------------------------------------------------------
//	call SetCastsShadow to make the scObject cast a shadow or not.
//--------------------------------------------------------------------
void scObject::SetCastsShadow(bool i_Cast)
{
	//	default action is do nothing
}


//--------------------------------------------------------------------
//  AddControlAnimation - adds animation controller, this object
//		will own the control animation
//--------------------------------------------------------------------
void scObject::AddControlAnimation( scControlAnim* i_pAnim )
{
	m_ControlAnims.push_back(i_pAnim);
}

//--------------------------------------------------------------------
//  RemoveControlAnimation - removes animation controller,
//		the caller is responsible for deleting the control animation
//--------------------------------------------------------------------
void scObject::RemoveControlAnimation( scControlAnim* i_pAnim )
{
	envSTLHelpers::RemoveOneValue(m_ControlAnims, i_pAnim);
}


//--------------------------------------------------------------------
//	Inserts node into hierarchy to allow control animation
//	to alter matrix
//--------------------------------------------------------------------
g3dSceneNode* scObject::InsertControlNode(const char* i_Name)
{
	// Insert control node between the attachment point and its
	// shapes and skins. This requires some manuevering of flags
	// and pointers in order to get the new node to influence
	// the shapes correctly.

	g3dSceneNode *pAttachmentNode = GetBase()->GetNamedNode(i_Name);
	DBG_ASSERT(pAttachmentNode, "Can't find node to attach");
	if (!pAttachmentNode) return NULL;

	// Create new node to hold the control animation
	g3dSceneNode *node = new g3dSceneNode();
	node->SetSkipAnim(true);

	// Handle joint influences
	if (pAttachmentNode->GetIsJoint())
	{
		// If attachment node was a joint, the inserted control node is a joint also
		node->SetIsJoint(true);
		node->SetInvBindPose( pAttachmentNode->GetInvBindPose() );

		// In order to insert the control node between the joint skin,
		// we transfer the influence responsibility to the new joint.
		node->SetSkipInfluence( pAttachmentNode->GetSkipInfluence() );
		pAttachmentNode->SetSkipInfluence(true);
	}

	// In order to insert the control node between the attachment and its shape,
	// move the fragment to the new node
	if (pAttachmentNode->GetFragment())
	{
		node->SetFragment( pAttachmentNode->GetFragment() );
		pAttachmentNode->SetFragment( NULL );

		// Do we need to move the material or environment info to the new node also?
	}

	// Insert our new node between attachment node and its children and shape
	const int num_kids = pAttachmentNode->GetNumChildren();
	for (int ni=0; ni<num_kids; ni++)
	{
		// always remove "0" because we are removing the nodes that have been processed
		g3dSceneNode *child = pAttachmentNode->GetChild(0); 
		pAttachmentNode->RemoveChild(child);
		node->AddChild(child);
	}
	pAttachmentNode->AddChild(node);

	return node;
}

//--------------------------------------------------------------------
// Remove Control Node that was added earlier
//--------------------------------------------------------------------
void scObject::RemoveControlNode(g3dSceneNode* i_pNode)
{
	g3dSceneNode *parent = i_pNode->GetParent();
	DBG_ASSERT(parent, "Need parent node to detach");
	if (!parent)
		return;
	
	// Remove control node between parent node and its children
	parent->RemoveChild(i_pNode);

	// Transfer influence responsibility back to parent node
	if (parent->GetIsJoint())
		parent->SetSkipInfluence(i_pNode->GetSkipInfluence());

	// Move the fragment back to the parent node
	if (i_pNode->GetFragment())
	{
		parent->SetFragment( i_pNode->GetFragment() );
		i_pNode->SetFragment( NULL );

		// Do we need to move the material or environment info also?
	}

	// Move children back from inserted node to parent.
	const int num_kids = i_pNode->GetNumChildren();
	for (int ni=0; ni<num_kids; ni++)
	{
		g3dSceneNode *child = i_pNode->GetChild(0);
		i_pNode->RemoveChild(child);
		parent->AddChild(child);
	}

	// Note: the node is not deleted, the caller needs to delete it.
}

//--------------------------------------------------------------------
// If implementations have low resolution models, return true here.
//	Default returns false.
//--------------------------------------------------------------------
bool scObject::HasLowResolutionModel() const
{
	return false;
}

//--------------------------------------------------------------------
// Static access to the algorithm used to compute 
//	full transformation.
//--------------------------------------------------------------------
//static 
void scObject::ComputeFullTransformation(const maPoint3d& i_Pivot,
										 const maVector3d& i_PivotCompensation,
										 const maPoint3d& i_Position,
										 const maPoint3d& i_Scale,
										 const maRotation& i_Orientation,
										 maMatrix4x4 &o_Transformation)
{
	// First translate pivot point to origin (negated pivot vector)
	maMatrix4x4 pivot;
	pivot.MakeTranslate(-i_Pivot.m_X, -i_Pivot.m_Y, -i_Pivot.m_Z);

	// Then combine our normal transformation vector (SRT) with the pivot offset
	// to return it to position
	maMatrix4x4 xform;
	xform(3, 0) = i_Position.m_X + i_Pivot.m_X + i_PivotCompensation.m_X;
	xform(3, 1) = i_Position.m_Y + i_Pivot.m_Y + i_PivotCompensation.m_Y;
	xform(3, 2) = i_Position.m_Z + i_Pivot.m_Z + i_PivotCompensation.m_Z;

	maMatrix3x3 rot_mat = i_Orientation.GetMatrix3x3();
	xform(0, 0) = rot_mat(0, 0) * i_Scale.m_X;
	xform(0, 1) = rot_mat(0, 1) * i_Scale.m_X;
	xform(0, 2) = rot_mat(0, 2) * i_Scale.m_X;
	xform(1, 0) = rot_mat(1, 0) * i_Scale.m_Y;
	xform(1, 1) = rot_mat(1, 1) * i_Scale.m_Y;
	xform(1, 2) = rot_mat(1, 2) * i_Scale.m_Y;
	xform(2, 0) = rot_mat(2, 0) * i_Scale.m_Z;
	xform(2, 1) = rot_mat(2, 1) * i_Scale.m_Z;
	xform(2, 2) = rot_mat(2, 2) * i_Scale.m_Z;

	o_Transformation = pivot * xform;
}

//--------------------------------------------------------------------
// Recompute the transformation using all info including pivot.
//--------------------------------------------------------------------
void scObject::update_full_transformation()
{
	maMatrix4x4 total_xform;

	scObject::ComputeFullTransformation(m_Pivot, m_PivotCompensation,
										m_Position, m_Scale, m_Orientation,
										total_xform);

	// Set combined matrix into scene node
	m_pBase->SetTransform( total_xform );
}