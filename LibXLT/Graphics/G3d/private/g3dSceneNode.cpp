/****************************************************************************\
**	g3dSceneNode.cpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dSceneNode.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/G3d/g3dFragmentCreate.hpp"

#include <string>
#include <vector>

//============================================================================
//============================================================================
namespace
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void update_child_transform(g3dSceneNode* i_Child)
	{
		DBG_ASSERT(i_Child, "Node pointer is NULL");
		if (!i_Child)
			return;

		DBG_ASSERT(i_Child->GetParent(), "Node does not have parent");
		if (i_Child->GetParent())
			i_Child->SetTotalTransform(i_Child->GetTransform() * i_Child->GetParent()->GetTotalTransform());

		int i;
		int num = i_Child->GetNumChildren();
		for( i = 0 ; i < num ; ++i )
		{
			update_child_transform(i_Child->GetChild(i));
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void update_world_bbox( g3dSceneNode* i_pNode )
	{
		maAxisBox world_box;

		// Update the children
		std::vector<g3dSceneNode*>& children = i_pNode->GetChildren();
		std::vector<g3dSceneNode*>::iterator it = children.begin(), end = children.end();
		for( ; it != end; ++it )
		{
			g3dSceneNode* child = (*it);
			update_world_bbox( child );
			if (!child->GetWorldBox().IsEmpty())
				world_box.Union( child->GetWorldBox() );
		}

		// Check if it has geometry
		const g3dFragment* pFrag = i_pNode->GetFragment();
		if( pFrag )
		{
			// Convert box to world space
			if( pFrag->IsModelSpaceBox() )
			{
				if (!pFrag->GetBoundingBox().IsEmpty())
				{
					const maMatrix4x4& total_transform = i_pNode->GetTotalTransform();

					maPoint3d box_points[8];
					pFrag->GetBoundingBox().GetBoxPoints( box_points );

					// Transform the points to world space
					for( int i = 0; i < 8; ++i )
					{
						total_transform.Transform( box_points[i] );
						world_box.Union( box_points[i] );
					}
				}
			}
			// Already in world space
			else
			{
				world_box.Union( pFrag->GetBoundingBox() );
			}
		}

		i_pNode->SetWorldBox( world_box );
	}
}

//============================================================================
//============================================================================
int g3dSceneNode::m_Count = 0;


//--------------------------------------------------------------------
//  Constructor
//--------------------------------------------------------------------
g3dSceneNode::g3dSceneNode()
:	m_pParent(NULL),
	m_pOrientation(NULL),
	m_pScaleOrientation(NULL),
	m_pFragment(NULL),
	m_subFragment(0),
	m_pMaterial(NULL),
	m_pRenderState(NULL),
	m_pAmbientEnv(NULL),
	m_bIsJoint(false),
	m_DrawStyle(e_Inherit),
	m_ContentResolution(e_Mixed),
	m_bHavePivots(false),
	m_PickMask(0)
{
	m_Flags.m_bRenderable = true;
	m_Flags.m_bActiveInRenderLayer = true;
	m_Flags.m_bActiveInSceneMgr = true;
	m_Flags.m_bIgnoreParentTransform = false;
	m_Flags.m_bFog = true;
	m_Flags.m_bCastsShadow = true;
//	m_Flags.m_bReceivesShadow = true;
	m_Flags.m_bSkipAnim = false;
	m_Flags.m_bSkipInfluence = false;
	m_Flags.m_bForceLowRes = false;
	m_Flags.m_bGPUPickable = true;
	m_Flags.m_bPickHull = false;
	m_Flags.m_bRenderableInPlanarReflection = true;
	m_Flags.m_bRenderableInCubeMapReflection = true;

#ifdef _DEBUG
	m_allocID = m_Count;
#endif
	++m_Count;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dSceneNode::g3dSceneNode(g3dFragment* i_Fragment)
:	m_pParent(NULL),
	m_pOrientation(NULL),
	m_pScaleOrientation(NULL),
	m_pFragment(i_Fragment),
	m_subFragment(0),
	m_pMaterial(NULL),
	m_pRenderState(NULL),
	m_pAmbientEnv(NULL),
	m_bIsJoint(false),
	m_DrawStyle(e_Inherit),
	m_ContentResolution(e_Mixed),
	m_bHavePivots(false),
	m_PickMask(0)
{
	m_Flags.m_bRenderable = true;
	m_Flags.m_bActiveInRenderLayer = true;
	m_Flags.m_bActiveInSceneMgr = true;
	m_Flags.m_bIgnoreParentTransform = false;
	m_Flags.m_bFog = true;
	m_Flags.m_bCastsShadow = true;
//	m_Flags.m_bReceivesShadow = true;
	m_Flags.m_bSkipAnim = false;
	m_Flags.m_bSkipInfluence = false;
	m_Flags.m_bForceLowRes = false;
	m_Flags.m_bGPUPickable = true;
	m_Flags.m_bPickHull = false;
	m_Flags.m_bRenderableInPlanarReflection = true;
	m_Flags.m_bRenderableInCubeMapReflection = true;

#ifdef _DEBUG
	m_allocID = m_Count;
#endif
	++m_Count;

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dSceneNode::g3dSceneNode(g3dFragment* i_Fragment, const maMatrix4x4 &i_Matx)
:	m_pParent(NULL),
	m_pOrientation(NULL),
	m_pScaleOrientation(NULL),
	m_pFragment(i_Fragment),
	m_subFragment(0),
	m_pMaterial(NULL),
	m_pRenderState(NULL),
	m_pAmbientEnv(NULL),
	m_bIsJoint(false),
	m_Transform(i_Matx),
	m_DrawStyle(e_Inherit),
	m_ContentResolution(e_Mixed),
	m_bHavePivots(false),
	m_PickMask(0)
{
	m_Flags.m_bRenderable = true;
	m_Flags.m_bActiveInRenderLayer = true;
	m_Flags.m_bActiveInSceneMgr = true;
	m_Flags.m_bIgnoreParentTransform = false;
	m_Flags.m_bFog = true;
	m_Flags.m_bCastsShadow = true;
//	m_Flags.m_bReceivesShadow = true;
	m_Flags.m_bSkipAnim = false;
	m_Flags.m_bSkipInfluence = false;
	m_Flags.m_bForceLowRes = false;
	m_Flags.m_bGPUPickable = true;
	m_Flags.m_bPickHull = false;
	m_Flags.m_bRenderableInPlanarReflection = true;
	m_Flags.m_bRenderableInCubeMapReflection = true;

#ifdef _DEBUG
	m_allocID = m_Count;
#endif
	++m_Count;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dSceneNode::g3dSceneNode(g3dFragment* i_Fragment, const maMatrix4x4 &i_Matx, int i_subFrag)
:	m_pParent(NULL),
	m_pOrientation(NULL),
	m_pScaleOrientation(NULL),
	m_pFragment(i_Fragment),
	m_subFragment(i_subFrag),
	m_pMaterial(NULL),
	m_pRenderState(NULL),
	m_pAmbientEnv(NULL),
	m_bIsJoint(false),
	m_Transform(i_Matx),
	m_DrawStyle(e_Inherit),
	m_ContentResolution(e_Mixed),
	m_bHavePivots(false),
	m_PickMask(0)
{
	m_Flags.m_bRenderable = true;
	m_Flags.m_bActiveInRenderLayer = true;
	m_Flags.m_bActiveInSceneMgr = true;
	m_Flags.m_bIgnoreParentTransform = false;
	m_Flags.m_bFog = true;
	m_Flags.m_bCastsShadow = true;
//	m_Flags.m_bReceivesShadow = true;
	m_Flags.m_bSkipAnim = false;
	m_Flags.m_bSkipInfluence = false;
	m_Flags.m_bForceLowRes = false;
	m_Flags.m_bGPUPickable = true;
	m_Flags.m_bPickHull = false;
	m_Flags.m_bRenderableInPlanarReflection = true;
	m_Flags.m_bRenderableInCubeMapReflection = true;

#ifdef _DEBUG
	m_allocID = m_Count;
#endif
	++m_Count;

}

//--------------------------------------------------------------------
//  Destructor
//--------------------------------------------------------------------
g3dSceneNode::~g3dSceneNode()
{
//#ifdef _DEBUG
//	if ( m_pParent != NULL )
//	{
//		DBG_LOG1( "Children of %x", m_pParent );
//		std::vector<g3dSceneNode*>& children = m_pParent->GetChildren();
//		std::vector<g3dSceneNode*>::iterator it = children.begin(), end = children.end();
//		for( ; it != end; ++it )
//		{
//			g3dSceneNode* child = (*it);
//			DBG_LOG1("   %x", (child) );
//		}
//	}
//#endif

	if( m_pOrientation )	//	normally we wouldn't need this test but envPool doesn't check for NULL
	{
		delete m_pOrientation;
	}
	if (m_pScaleOrientation)
	{
		delete m_pScaleOrientation;
	}

	envSTLHelpers::DeleteContainer(m_Children);
	--m_Count;
}

//--------------------------------------------------------------------
//  AddChild
//--------------------------------------------------------------------
void g3dSceneNode::AddChild(g3dSceneNode* i_pNode)
{
#ifdef _DEBUG
	DBG_ASSERT(m_Children.end() == std::find(m_Children.begin(), m_Children.end(), i_pNode),
		"duplicate g3dSceneNode trying to be added");
	if (m_Children.end() != std::find(m_Children.begin(), m_Children.end(), i_pNode))
		return;
#endif

	m_Children.push_back(i_pNode);
	i_pNode->SetParent(this);

	//DBG_LOG2("%x <+ %x", this, i_pNode );
	//if ( this == (g3dSceneNode*)0x05dfc894 )
	//{
	//	int x = 0;
	//}
}

//--------------------------------------------------------------------
//	RemoveChild just removes the child without deleting it.
//--------------------------------------------------------------------
void g3dSceneNode::RemoveChild(g3dSceneNode* i_Child)
{
	std::vector<g3dSceneNode*>::iterator it = std::find(m_Children.begin(), m_Children.end(), i_Child);
	if (it != m_Children.end())
	{
		i_Child->SetParent(NULL);
		m_Children.erase(it);

		//DBG_LOG2("%x <- %x", this, i_Child );
		//if ( this == (g3dSceneNode*)0x5df5c54 )
		//{
		//	int x = 0;
		//}
	}
}

//--------------------------------------------------------------------
//	HasChild returns true if the given node is a child of this node.
//--------------------------------------------------------------------
bool g3dSceneNode::HasChild(g3dSceneNode* i_Child)
{
	std::vector<g3dSceneNode*>::iterator it = std::find(m_Children.begin(), m_Children.end(), i_Child);
	return (it != m_Children.end());
}

//--------------------------------------------------------------------
//	These Destroy functions remove the child from the list and delete
//	it.
//--------------------------------------------------------------------
void g3dSceneNode::DestroyChild(g3dSceneNode* i_Child)
{
	std::vector<g3dSceneNode*>::iterator it = std::find(m_Children.begin(), m_Children.end(), i_Child);
	if( it != m_Children.end() )
	{
		delete *it;
		m_Children.erase(it);
	}
}

void g3dSceneNode::DestroyChildren()
{
	envSTLHelpers::DeleteContainer(m_Children);
}


//--------------------------------------------------------------------
// Remove old child node and put new child in its place
//	Returns true if successful.
//--------------------------------------------------------------------
bool g3dSceneNode::SwapChild(g3dSceneNode* i_OldChild, g3dSceneNode* i_NewChild)
{
	for (int i=0; i<m_Children.size(); i++)
	{
		if (m_Children[i] == i_OldChild)
		{
			m_Children[i] = i_NewChild;
			//DBG_LOG2("SwapChild: Setting '%s' parent to NULL, was %s", i_OldChild->GetName(), (i_OldChild->GetParent()) ? i_OldChild->GetParent()->GetName() : "NULL");
			i_OldChild->SetParent(NULL);
			//DBG_LOG3("SwapChild: Setting '%s' parent to (%s), was %s", i_NewChild->GetName(), this->GetName(), (i_NewChild->GetParent()) ? i_NewChild->GetParent()->GetName() : "NULL");
			i_NewChild->SetParent(this);
			return true;
		}
	}
	DBG_ASSERT(false, "Child not found in SwapChild");
	return false;
}

//--------------------------------------------------------------------
//	SetTransform sets the transform to the parent space (or
//	world space if there is no parent).
//--------------------------------------------------------------------
void g3dSceneNode::SetTransform(const maMatrix4x4& i_Transform)
{
	m_Transform = i_Transform;
}

//--------------------------------------------------------------------
//	SetTotalTransform sets the transform to world space (assuming
//	it has been updated properly)
//--------------------------------------------------------------------
void g3dSceneNode::SetTotalTransform(const maMatrix4x4& i_Transform)
{
	m_TotalTransform = i_Transform;
}

//--------------------------------------------------------------------
//	UpdateTotalTransform can be used to ensure that the total
//	matrices of the scene are updated.  Usually the renderer does
//	this automatically during the render but in some cases it may
//	be necessary to refresh a node manually.
//--------------------------------------------------------------------
void g3dSceneNode::UpdateTotalTransform()
{
	m_TotalTransform = m_Transform;

	if( !GetIgnoreParentTransform() )
	{
		g3dSceneNode* cur_node = m_pParent;
		while( cur_node )
		{
			m_TotalTransform *= cur_node->m_Transform;
			if( cur_node->GetIgnoreParentTransform() )
			{
				break;
			}

			cur_node = cur_node->m_pParent;
		}
	}

	envSTLHelpers::ForAll( m_Children, update_child_transform );

	update_world_bbox( this );
}

//----------------------------------------------------------------------------
//	Name used to be limited, now is std::string
//----------------------------------------------------------------------------
void g3dSceneNode::SetName(const char* i_Name)
{
	m_Name = i_Name;
}

//----------------------------------------------------------------------------
//	Name for baking texture
//----------------------------------------------------------------------------
const char* g3dSceneNode::GetBakeName() const
{
	return m_BakeName.c_str();
}

//----------------------------------------------------------------------------
//	Name for baking texture
//----------------------------------------------------------------------------
void g3dSceneNode::SetBakeName(const char* i_Name)
{
	m_BakeName = i_Name;
}

//----------------------------------------------------------------------------
//	Safe comparison of names, so we can change length and case-sensitivity
//----------------------------------------------------------------------------
bool g3dSceneNode::CompareName(const char* i_Name)
{
	return (m_Name == i_Name);
//	return ( strncmp(m_Name, i_Name, 16) == 0 );
}

//--------------------------------------------------------------------
//  GetNamedNode
//--------------------------------------------------------------------
const g3dSceneNode* g3dSceneNode::GetNamedNode(const char* i_Name) const
{
	//if( strncmp(m_Name, i_Name, 16) == 0 )
	if (m_Name == i_Name)
	{
		return this;
	}
	else
	{
		int i;
		int num = m_Children.size();
		for( i = 0 ; i < num ; ++i )
		{
			const g3dSceneNode* node = m_Children[i]->GetNamedNode(i_Name);
			if( node )
				return node;
		}
	}

	return NULL;
}


//--------------------------------------------------------------------
//  GetNamedNode
//--------------------------------------------------------------------
g3dSceneNode* g3dSceneNode::GetNamedNode(const char* i_Name)
{
	//if( strncmp(m_Name, i_Name, 16) == 0 )
	if (m_Name == i_Name)
	{
		return this;
	}
	else
	{
		int i;
		int num = m_Children.size();
		for( i = 0 ; i < num ; ++i )
		{
			g3dSceneNode* node = m_Children[i]->GetNamedNode(i_Name);
			if( node )
				return node;
		}
	}

	return NULL;
}
//--------------------------------------------------------------------
//  Given a collection of path components
//  enclosed by the iterators b and e
//  follow the path from this node through the hierarchy.
//  return the descendent g3dSceneNode that matches the path
//--------------------------------------------------------------------
template< class TPtr >
TPtr g3dSceneNode::GetNamedNodeFromPathImpl( TPtr i_pNode,
											 std::deque< std::string >::const_iterator b, 
											 std::deque< std::string>::const_iterator e	) 
{
	TPtr retVal = NULL;
	if( b == e || i_pNode->m_Name != *b )
	{
		return retVal;
	}
	DBG_ASSERT( ( i_pNode->m_Name == *b ), "Not able to follow the path from the sceneroot" );
	//get the next path component
	b++;
	if ( b == e )
	{   //if this is the end of the path
		return i_pNode;
	}
	//more path components to process
	//so go into children
	int num = i_pNode->m_Children.size();
	for( int i = 0 ; i < num  ; ++i )
	{
		TPtr pChild = i_pNode->m_Children[i];
		retVal = pChild->GetNamedNodeFromPath( b, e);
		if ( NULL != retVal )
		{
			break;
		}
	}
	return retVal;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const g3dSceneNode* g3dSceneNode::GetNamedNodeFromPath( 
		std::deque< std::string >::const_iterator b, 
		std::deque< std::string>::const_iterator e	) const
{
	return GetNamedNodeFromPathImpl< const g3dSceneNode * > ( this, b, e );
	/*
	const g3dSceneNode *retVal = NULL;
	if( b == e || m_Name != *b )
	{
		return retVal;
	}
	DBG_ASSERT( (m_Name == *b ), "Not able to follow the path from the sceneroot" );
	//get the next path component
	b++;
	if ( b == e )
	{   //if this is the end of the path
		return this;
	}
	//more path components to process
	//so go into children
	int num = m_Children.size();
	for( i = 0 ; i < num  ; ++i )
	{
		const g3dSceneNode* pChild = m_Children[i];
		retVal = pChild->GetNamedNodeFromPath( b, e);
		if ( !retVal.empty() )
		{
			break;
		}
	}
	return retVal;
	*/
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dSceneNode* g3dSceneNode::GetNamedNodeFromPath( std::deque< std::string >::const_iterator b, 
												  std::deque< std::string>::const_iterator e ) 
{
	return GetNamedNodeFromPathImpl< g3dSceneNode * > ( this, b, e );
	/*
	const g3dSceneNode *retVal = NULL;
	if( b == e || m_Name != *b )
	{
		return retVal;
	}
	DBG_ASSERT( (m_Name == *b ), "Not able to follow the path from the sceneroot" );
	//get the next path component
	b++;
	if ( b == e )
	{   //if this is the end of the path
		return this;
	}
	//more path components to process
	//so go into children
	int num = m_Children.size();
	for( i = 0 ; i < num  ; ++i )
	{
		const g3dSceneNode* pChild = m_Children[i];
		retVal = pChild->GetNamedNodeFromPath( b, e);
		if ( !retVal.empty() )
		{
			break;
		}
	}
	return retVal;
	*/
}
//----------------------------------------------------------------------------
//	The orientation is an additional rotation which is compounded during
//	animation, representing a base position for the joint.
//	GetOrientationTransform could return NULL, in which case the node is
//	not considered to have this additional orientation information.
//	The SetOrientation uses 3 Euler angles for X, Y, and Z rotation.
//----------------------------------------------------------------------------
void g3dSceneNode::SetOrientation(float i_X, float i_Y, float i_Z)
{
	if( m_pOrientation == NULL )
		m_pOrientation = new maMatrix4x4;

	m_pOrientation->MakeRotateX(i_X);
	maMatrix4x4 ry;
	ry.MakeRotateY(i_Y);
	maMatrix4x4 rz;
	rz.MakeRotateZ(i_Z);
	(*m_pOrientation) *= ry;
	(*m_pOrientation) *= rz;
}
void g3dSceneNode::SetScaleOrientation(float i_X, float i_Y, float i_Z)
{
	if( m_pScaleOrientation == NULL )
		m_pScaleOrientation = new maMatrix4x4;

	m_pScaleOrientation->MakeRotateX(i_X);
	maMatrix4x4 ry;
	ry.MakeRotateY(i_Y);
	maMatrix4x4 rz;
	rz.MakeRotateZ(i_Z);
	(*m_pScaleOrientation) *= ry;
	(*m_pScaleOrientation) *= rz;
}


//----------------------------------------------------------------------------
// Other methods for setting the orientation. Direct matrix set and
// a function for clearing the orientation matrix back to NULL.
//----------------------------------------------------------------------------
void g3dSceneNode::SetOrientation(const maMatrix4x4& i_Matrix)
{
	if ( m_pOrientation == NULL )
		m_pOrientation = new maMatrix4x4;
	(*m_pOrientation) = i_Matrix;
}
void g3dSceneNode::ClearOrientation()
{
	if( m_pOrientation )	
	{
		delete m_pOrientation;
		m_pOrientation = NULL;
	}
}

//------------------------------------------------------------------------
// Rotate and scale pivot points for transformation.
//------------------------------------------------------------------------
void g3dSceneNode::SetPivotPoints(const maVector3d& i_RotatePivot,
								  const maVector3d& i_ScalePivot,
								  const maVector3d& i_RotatePivotTranslation,
								  const maVector3d& i_ScalePivotTranslation)
{
	m_RotatePivot = i_RotatePivot;
	m_ScalePivot = i_ScalePivot;
	m_RotatePivotTranslation = i_RotatePivotTranslation;
	m_ScalePivotTranslation = i_ScalePivotTranslation;
	m_bHavePivots = true;
}

//--------------------------------------------------------------------
//	DrawStyle controls how the shapes below this node will render.
//	The default e_Inherit means it will inherit the draw style of
//	the parent, but other styles can be used for wireframe.
//--------------------------------------------------------------------
void g3dSceneNode::SetDrawStyle(DrawStyle i_Style)
{
	m_DrawStyle = i_Style;
}

//--------------------------------------------------------------------
//	ContentResolution represents what resolution is contained
//	in this section of the scene graph. The default is e_Mixed.
//	When loading a file or creating content, please set this
//	flag in order to separate out the high and low res models.
//--------------------------------------------------------------------
void g3dSceneNode::SetContentResolution(Resolution i_Res)
{
	m_ContentResolution = i_Res;
}

//----------------------------------------------------------------------------
//	ForceLowResolution is used like DrawStyle to control how this object
//	should be rendered. If set true, then the renderer should choose the 
//	low-resolution models beneth this node. It does not represent the 
//	content like ContentResolution does, it is used to specify a 
//	rendering choice.
//----------------------------------------------------------------------------
void g3dSceneNode::SetForceLowResolution(bool i_bForce)
{
	m_Flags.m_bForceLowRes = i_bForce;
}

//----------------------------------------------------------------------------
// Set whether to consider this node and its children during the
// pick buffer render. This render assigns fragments color identifiers
// in order to see which fragment is closest to the camera.
//----------------------------------------------------------------------------
void g3dSceneNode::SetGPUPickable(bool i_bPickable)
{
	m_Flags.m_bGPUPickable = i_bPickable;
}

//----------------------------------------------------------------------------
// PickHull objects render to pick buffers even if Renderable flag is false
//----------------------------------------------------------------------------
void g3dSceneNode::SetPickHull(bool i_bPickHull)
{
	m_Flags.m_bPickHull = i_bPickHull;
}

//----------------------------------------------------------------------------
// PickMask is a user defined bit mask that can be used to filter
// the pickable objects. The default value of "0" means "do not filter".
//----------------------------------------------------------------------------
void g3dSceneNode::SetPickMask(envType::UInt32 i_PickMask)
{
	m_PickMask = i_PickMask;
}

//----------------------------------------------------------------------------
// Set ranges of pick codes for this node and its children in order
//	to locate the fragment that was picked.
//----------------------------------------------------------------------------
void g3dSceneNode::SetLowPickCode(envType::UInt32 i_PickCode)
{
	m_PickCodeLow = i_PickCode;
}
void g3dSceneNode::SetHighPickCode(envType::UInt32 i_PickCode)
{
	m_PickCodeHigh = i_PickCode;
}

//----------------------------------------------------------------------------
// Is the pick code given within the high and low pick codes assigned
// to this node?
//----------------------------------------------------------------------------
bool g3dSceneNode::ContainsPickCode(envType::UInt32 i_PickCode) const
{
	// We have to check the flags before cheking the pick code,
	// because our pick code was not updated if these
	// flags meant that the traversal didn't reach this node.
	if (!this->GetRenderable() && !this->GetPickHull())
		return false;
	if (!this->GetGPUPickable())
		return false;

	return ( (i_PickCode >= m_PickCodeLow) && (i_PickCode < m_PickCodeHigh) );
}

//--------------------------------------------------------------------
//	Returns the node with a fragment with the given pick code
//--------------------------------------------------------------------
const g3dSceneNode* g3dSceneNode::GetPickedNode(envType::UInt32 i_PickCode) const
{
	if (this->ContainsPickCode(i_PickCode))
	{
		// If we have a fragment and we match the low pick code, 
		// then it has to be our node.
		if ((this->m_pFragment != NULL) &&
			(i_PickCode == m_PickCodeLow) )
		{
			return this;
		}

		// Search for it in the children
		int num = m_Children.size();
		for (int i = 0; i < num; ++i )
		{
			const g3dSceneNode* node = m_Children[i]->GetPickedNode(i_PickCode);
			if ( node )
				return node;
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
//	DebugLogTree dumps a description of the heirarchy and children to the
//	debug log.
//----------------------------------------------------------------------------
void g3dSceneNode::DebugLogTree() const
{
	this->debug_log_tree(0);
}

//----------------------------------------------------------------------------
//  debug_log_tree
//----------------------------------------------------------------------------
void g3dSceneNode::debug_log_tree(int i_Tab) const
{
	std::string text;
	int i;
	for( i = 0 ; i < i_Tab ; ++i )
	{
		text += ' ';
		text += ' ';
	}

	text += m_Name;
	DBG_LOG(text.c_str());

	int num = m_Children.size();
	++i_Tab;
	for( i = 0 ; i < num ; ++i )
	{
		m_Children[i]->debug_log_tree(i_Tab);
	}
}

//----------------------------------------------------------------------------
//	Clone returns a copy of this, including children
//  Newly created fragments are returned in the o_NewFragments vector..
//----------------------------------------------------------------------------
g3dSceneNode* g3dSceneNode::Clone(std::vector<g3dFragment*>& o_NewFragments) const
{
	g3dSceneNode* ret_val = new g3dSceneNode(*this);
	//	this is a little tricky; we're using the compiler supplied copy constructor
	//	which doesn't increment m_Count, so we do it ourselves here
	++m_Count;

	ret_val->m_pParent = NULL;
	ret_val->m_Children.clear();

	if( m_pOrientation )
		ret_val->m_pOrientation = new maMatrix4x4(*m_pOrientation);
	if( m_pScaleOrientation )
		ret_val->m_pScaleOrientation = new maMatrix4x4(*m_pScaleOrientation);

	// Used to reuse the fragment pointer, but now that the
	// g3dFragment class has instance-specific info we can only share
	// the vertex buffers. By calling Clone(), the video memory buffers
	// will be shared.
	if (m_pFragment)
	{
		ret_val->m_pFragment = g3dFragmentCreate::CloneFragment(m_pFragment);
		if (ret_val->m_pFragment) o_NewFragments.push_back(ret_val->m_pFragment);
	}

	int i;
	int num = m_Children.size();
	for( i = 0 ; i < num ; ++i )
	{
		g3dSceneNode* cloned_child = m_Children[i]->Clone(o_NewFragments);
		ret_val->AddChild(cloned_child);
	}

	return ret_val;
}

//------------------------------------------------------------------------
// Clone all the children of the original scene node
// and add the cloned children to this node as children
// Also clone any fragments
//  Newly created fragments are returned in the o_NewFragments vector.
//----------------------------------------------------------------------------
void g3dSceneNode::CloneChildrenFrom( const g3dSceneNode *pOriginal,
									  std::vector<g3dFragment*>& o_NewFragments ) 
{
	if( NULL != pOriginal->GetFragment() )
	{
		const g3dFragment *pFragment = pOriginal->GetFragment();
		g3dFragment *pNewFragment = g3dFragmentCreate::CloneFragment( pFragment );
		SetFragment( pNewFragment );
		o_NewFragments.push_back( pNewFragment );
	}
	m_Children.clear();
	int nChildren = pOriginal->GetNumChildren();
	for( int i=0;  i < nChildren; ++i )
	{
		const g3dSceneNode *pChild = pOriginal->GetChild( i );
		g3dSceneNode *pNewChild = pChild->Clone( o_NewFragments );
		AddChild( pNewChild );
	}
}
//----------------------------------------------------------------------------
//	GetCount returns the number of g3dSceneNodes in existence, primarily for
//	leak checking.
//----------------------------------------------------------------------------
int g3dSceneNode::GetCount()
{
	return m_Count;
}