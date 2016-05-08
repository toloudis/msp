/****************************************************************************\
**	g3dSceneNode.hpp
**
**	A g3dSceneNode is an element of the scene graph that describes the items
**	that are rendered in the psg package.  A scene node generally has a
**	parent and children, and stores the matrix transformation to the space
**	of its parent.  A scene node may also have a fragment associated with
**	it, which means that something is rendered for that node.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_SCENENODE_HPP
#error g3dSceneNode.hpp multiply included
#endif
#define G3D_SCENENODE_HPP

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef ENV_POOL_HPP
#include "Core/env/envPool.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif

#include <deque>
#include <string>
#include <vector>


//============================================================================
//	Forward References
//============================================================================
class g3dFragment;
class matMaterial;
struct g3dAmbientEnvState;
struct g3dRenderState;


//============================================================================
//============================================================================
class g3dSceneNode
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		enum DrawStyle
		{
			e_Inherit = -1,	// inherit value from parent node (the default)
			e_Solid = 0,	// draw lit, solid polygons (normal style)
			e_Wireframe,	// draw unlit wireframe
			e_LitWireframe	// draw wireframe with textures and lighting
		};
		enum Resolution
		{
			e_Mixed = -1,	// this node and its children contain both low-res and high-res content
			e_LowRes = 0,	// content beneath and including this node is all low-res
			e_HighRes		// content beneath and including this node is all high-res
		};

		//--------------------------------------------------------------------
		//  Constructor
		//--------------------------------------------------------------------
		g3dSceneNode();
		g3dSceneNode(g3dFragment* i_Fragment);
		g3dSceneNode(g3dFragment* i_Fragment, const maMatrix4x4 &i_Matx);
		g3dSceneNode(g3dFragment* i_Fragment, const maMatrix4x4 &i_Matx, int i_subFragIndex);

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~g3dSceneNode();

		//--------------------------------------------------------------------
		//	GetTransform returns the transform to the parent space (or
		//	world space if there is no parent).
		//--------------------------------------------------------------------
		inline const maMatrix4x4& GetTransform() const;
		//inline maMatrix4x4& GetTransform();
		void SetTransform(const maMatrix4x4& i_Transform);

		//--------------------------------------------------------------------
		//	GetTotalTransform returns the transform to world space (assuming
		//	it has been updated properly)
		//--------------------------------------------------------------------
		inline const maMatrix4x4& GetTotalTransform() const;
		//inline maMatrix4x4& GetTotalTransform();
		void SetTotalTransform(const maMatrix4x4& i_Transform);

		//--------------------------------------------------------------------
		//	UpdateTotalTransform can be used to ensure that the total
		//	matrices of the scene are updated.  Causes all children
		//  to be updated.
		//--------------------------------------------------------------------
		void UpdateTotalTransform();

		//--------------------------------------------------------------------
		//	Sets the parent of this object as i_pParent.  This does not
		//	change i_pParent.
		//--------------------------------------------------------------------
		inline void SetParent(g3dSceneNode* i_pParent);

		//--------------------------------------------------------------------
		//	Adds i_pNode as a child of this; also sets the parent of i_pNode.
		//	The g3dSceneNode owns its children.
		//--------------------------------------------------------------------
		void AddChild(g3dSceneNode* i_pNode);

		//--------------------------------------------------------------------
		//	RemoveChild just removes the child without deleting it.
		//--------------------------------------------------------------------
		void RemoveChild(g3dSceneNode* i_Child);

		//--------------------------------------------------------------------
		//	HasChild returns true if the given node is a child of this node.
		//--------------------------------------------------------------------
		bool HasChild(g3dSceneNode* i_Child);

		//--------------------------------------------------------------------
		//	These Destroy functions remove the child from the list and delete
		//	it.
		//--------------------------------------------------------------------
		void DestroyChild(g3dSceneNode* i_Child);
		void DestroyChildren();

		//--------------------------------------------------------------------
		// Remove old child node and put new child in its place.
		//	Returns true if successful.
		//--------------------------------------------------------------------
		bool SwapChild(g3dSceneNode* i_OldChild, g3dSceneNode* i_NewChild);

		//--------------------------------------------------------------------
		//	This could return NULL.
		//--------------------------------------------------------------------
		inline g3dSceneNode* GetParent();
		inline const g3dSceneNode* GetParent() const;

		//--------------------------------------------------------------------
		//  GetChild/Children
		//--------------------------------------------------------------------
		inline int GetNumChildren() const;
		inline g3dSceneNode* GetChild(int i_Num);
		inline const g3dSceneNode* GetChild(int i_Num) const;
		inline std::vector<g3dSceneNode*>& GetChildren();
		inline const std::vector<g3dSceneNode*>& GetChildren() const;

		//--------------------------------------------------------------------
		//	GetRenderable returns true if this node and all of its children
		//	should be rendered.
		//--------------------------------------------------------------------
		inline bool GetRenderable() const;
		inline void SetRenderable(bool i_bRenderable);

		//--------------------------------------------------------------------
		//	GetRenderable returns true if this node and all of its children
		//	should be rendered in render layer setting.
		//--------------------------------------------------------------------
		inline bool GetActiveInRenderLayer() const;
		inline void SetActiveInRenderLayer(bool i_bRenderable);

		//--------------------------------------------------------------------
		//	GetRenderable returns true if this node and all of its children
		//	should be rendered in render layer setting.
		//--------------------------------------------------------------------
		inline bool GetActiveInSceneMgr() const;
		inline void SetActiveInSceneMgr(bool i_bRenderable);

		//--------------------------------------------------------------------
		//	GetRenderableInPlanarReflection returns true if this node and all 
		//	of its children should be rendered to a reflection map.
		//--------------------------------------------------------------------
		inline bool GetRenderableInPlanarReflection() const;
		inline void SetRenderableInPlanarReflection(bool i_bRenderable);
		inline bool GetRenderableInCubeMapReflection() const;
		inline void SetRenderableInCubeMapReflection(bool i_bRenderable);

		//--------------------------------------------------------------------
		//	GetIgnoreParentTransform will return true if the node should be
		//	treated as a root of the world for purposes of positioning.
		//--------------------------------------------------------------------
		inline bool GetIgnoreParentTransform() const;
		inline void SetIgnoreParentTransform(bool i_bIgnore);

		//--------------------------------------------------------------------
		//	GetFogged returns true if the node should be rendered with fog.
		//	Rendering without fog is generally faster so it should be
		//	disabled where possible.
		//--------------------------------------------------------------------
		inline bool GetFogged() const;
		inline void SetFogged(bool i_bFog);

		//----------------------------------------------------------------------------
		//	If CastsShadow is false, then the depth map render pass will 
		//  not render this node and those beneath it.
		//----------------------------------------------------------------------------
		inline bool GetCastsShadow() const;
		inline void SetCastsShadow(bool i_bShadow);

		//----------------------------------------------------------------------------
		//	If ReceivesShadow is false, then this node and those beneath it
		//  will not receive light from shadow sources.
		//----------------------------------------------------------------------------
		//inline bool GetReceivesShadow() const;
		//inline void SetReceivesShadow(bool i_bShadow);

		//----------------------------------------------------------------------------
		//	If SkipAnim is true, then the animation code should skip this node
		//	because it is not part of the original hierarchy.
		//----------------------------------------------------------------------------
		inline bool GetSkipAnim() const;
		inline void SetSkipAnim(bool i_bSkip);

		//----------------------------------------------------------------------------
		//	If SkipInfluence is true, then the influence matrices code should 
		//	skip this joint because it is not part of the original hierarchy.
		//----------------------------------------------------------------------------
		inline bool GetSkipInfluence() const;
		inline void SetSkipInfluence(bool i_bSkip);

		//----------------------------------------------------------------------------
		//	Name used to be limited, now is std::string
		//----------------------------------------------------------------------------
		const char* GetName() const;
		void SetName(const char* i_Name);

		//----------------------------------------------------------------------------
		//	Name for baking texture
		//----------------------------------------------------------------------------
		const char* GetBakeName() const;
		void SetBakeName(const char* i_Name);

		//----------------------------------------------------------------------------
		//	Safe comparison of names, so we can change length and case-sensitivity
		//----------------------------------------------------------------------------
		bool CompareName(const char* i_Name);

		//--------------------------------------------------------------------
		//	returns a node with the given name which is either this or a
		//	recursive child of this, or NULL if no such node exists.
		//--------------------------------------------------------------------
		const g3dSceneNode* GetNamedNode(const char* i_Name) const;
		g3dSceneNode* GetNamedNode(const char* i_Name);
		//--------------------------------------------------------------------
		//  Given a collection of path components
		//  enclosed by the iterators b and e
		//  follow the path from this node through the hierarchy.
		//  return the descendent g3dSceneNode that matches the path
		//--------------------------------------------------------------------
		const g3dSceneNode* g3dSceneNode::GetNamedNodeFromPath( 
					std::deque< std::string >::const_iterator b, 
					std::deque< std::string>::const_iterator e) const;
		
		g3dSceneNode* g3dSceneNode::GetNamedNodeFromPath( 
					std::deque< std::string >::const_iterator b, 
					std::deque< std::string>::const_iterator e);
		//actual implementation
		template< class TPtr >
		static TPtr GetNamedNodeFromPathImpl( TPtr pNode,
			std::deque< std::string >::const_iterator b, 
			std::deque< std::string>::const_iterator e
			);
		//--------------------------------------------------------------------
		//	GetFragment returns the fragment at this node.  This
		//	could be NULL.
		//--------------------------------------------------------------------
		inline const g3dFragment* GetFragment() const;
		inline g3dFragment* GetFragment();

		int GetSubFragment() const {return m_subFragment;}

		//--------------------------------------------------------------------
		//	SetFragment sets the fragment at this node.
		//--------------------------------------------------------------------
		inline void SetFragment( g3dFragment* i_pFragment );

		//--------------------------------------------------------------------
		//	If the g3dSceneNode has a material set to a non-NULL color,
		//	that material will replace ("override") the fragment's material.
		//--------------------------------------------------------------------
		inline matMaterial* GetMaterial();
		inline const matMaterial* GetMaterial() const;
		inline void SetMaterial(matMaterial* i_pMaterial);

		//--------------------------------------------------------------------
		//	The render state is a struct of data the is used to render the
		//  scene node and its children.  For example lights.  The render
		//  state is NULL unless the render state is Set.
		//--------------------------------------------------------------------
		inline g3dRenderState* GetRenderState();
		inline const g3dRenderState* GetRenderState() const;
		inline void SetRenderState( g3dRenderState* i_pRenderState );

		//--------------------------------------------------------------------
		//	The environment state is a struct of data the is used to render 
		//  the scene node's ambient environment pass and its children.  
		//  The environment state is NULL unless the environment state is Set.
		//--------------------------------------------------------------------
		inline g3dAmbientEnvState* GetEnvironment();
		inline const g3dAmbientEnvState* GetEnvironment() const;
		inline void SetEnvironment( g3dAmbientEnvState* i_pAmbientEnv );

		//----------------------------------------------------------------------------
		//	The world box of the g3dSceneNode should contain all child nodes.
		//----------------------------------------------------------------------------
		inline const maAxisBox& GetWorldBox() const;
		inline void SetWorldBox(const maAxisBox& i_Box);

		//----------------------------------------------------------------------------
		//	The orientation is an additional rotation which is compounded during
		//	animation, representing a base position for the joint.
		//	GetOrientationTransform could return NULL, in which case the node is
		//	not considered to have this additional orientation information.
		//	The SetOrientation uses 3 Euler angles for X, Y, and Z rotation.
		//----------------------------------------------------------------------------
		void SetOrientation(float i_X, float i_Y, float i_Z);
		inline const maMatrix4x4* GetOrientation() const;
		void SetScaleOrientation(float i_X, float i_Y, float i_Z);
		inline const maMatrix4x4* GetScaleOrientation() const;

		//----------------------------------------------------------------------------
		// Other methods for setting the orientation. Direct matrix set and
		// a function for clearing the orientation matrix back to NULL.
		//----------------------------------------------------------------------------
		void SetOrientation(const maMatrix4x4& i_Matrix);
		void ClearOrientation();

		//------------------------------------------------------------------------
		// Rotate and scale pivot points for transformation.
		//------------------------------------------------------------------------
		void SetPivotPoints(const maVector3d& i_RotatePivot,
						    const maVector3d& i_ScalePivot, 
							const maVector3d& i_RotatePivotTranslation, 
							const maVector3d& i_ScalePivotTranslation);
		inline bool GetHasPivots() const;
		inline const maVector3d& GetRotatePivot() const;
		inline const maVector3d& GetScalePivot() const;
		inline const maVector3d& GetRotatePivotTranslation() const;
		inline const maVector3d& GetScalePivotTranslation() const;

		//--------------------------------------------------------------------
		// If the joint flag is set to true, then this node will be used
		//	to influence skinned vertices. The scale values for this node
		//	should be applied to the skinned matrices, but should not
		//	apply to the child nodes. The InvBindPose matrix
		//	should be set when using the joint flag. Default is false.
		//--------------------------------------------------------------------
		inline bool GetIsJoint() const;
		inline void SetIsJoint(bool i_bJoint);

		//--------------------------------------------------------------------
		//	GetInvBindPose returns the inverse of the bind pose matrix for
		//	the joint space.  The inverse of the bind pose matrix will
		//	transform a vertex in model space to joint space.
		//--------------------------------------------------------------------
		inline const maMatrix4x4& GetInvBindPose() const;
		inline void SetInvBindPose(const maMatrix4x4& i_InvBindPose);

		//----------------------------------------------------------------------------
		//	DrawStyle controls how the shapes below this node will render.
		//	The default e_Inherit means it will inherit the draw style of
		//	the parent, but other styles can be used for wireframe.
		//----------------------------------------------------------------------------
		inline DrawStyle GetDrawStyle() const;
		void SetDrawStyle(DrawStyle i_Style);

		//----------------------------------------------------------------------------
		//	ContentResolution represents what resolution is contained
		//	in this section of the scene graph. The default is e_Mixed.
		//	When loading a file or creating content, please set this
		//	flag in order to separate out the high and low res models.
		//----------------------------------------------------------------------------
		inline Resolution GetContentResolution() const;
		void SetContentResolution(Resolution i_Res);

		//----------------------------------------------------------------------------
		//	ForceLowResolution is used like DrawStyle to control how this object
		//	should be rendered. If set true, then the renderer should choose the 
		//	low-resolution models beneth this node. It does not represent the 
		//	content like ContentResolution does, it is used to specify a 
		//	rendering choice.
		//----------------------------------------------------------------------------
		inline bool GetForceLowResolution() const;
		void SetForceLowResolution(bool i_bForce);

		//----------------------------------------------------------------------------
		// Set whether to consider this node and its children during the
		// pick buffer render. This render assigns fragments color identifiers
		// in order to see which fragment is closest to the camera.
		//----------------------------------------------------------------------------
		inline bool GetGPUPickable() const;
		void SetGPUPickable(bool i_bPickable);

		//----------------------------------------------------------------------------
		// PickHull objects render to pick buffers even if Renderable flag is false
		//----------------------------------------------------------------------------
		inline bool GetPickHull() const;
		void SetPickHull(bool i_bPickHull);

		//----------------------------------------------------------------------------
		// PickMask is a user defined bit mask that can be used to filter
		// the pickable objects. The default value of "0" means "do not filter".
		//----------------------------------------------------------------------------
		inline envType::UInt32 GetPickMask() const;
		void SetPickMask(envType::UInt32 i_PickMask);

		//----------------------------------------------------------------------------
		// Set ranges of pick codes for this node and its children in order
		//	to locate the fragment that was picked.
		//----------------------------------------------------------------------------
		void SetLowPickCode(envType::UInt32 i_PickCode);
		void SetHighPickCode(envType::UInt32 i_PickCode);

		//----------------------------------------------------------------------------
		// Is the pick code given within the high and low pick codes assigned
		// to this node?
		//----------------------------------------------------------------------------
		bool ContainsPickCode(envType::UInt32 i_PickCode) const;

		//--------------------------------------------------------------------
		//	Returns the node with a fragment with the given pick code
		//--------------------------------------------------------------------
		const g3dSceneNode* GetPickedNode(envType::UInt32 i_PickCode) const;

		//----------------------------------------------------------------------------
		//	DebugLogTree dumps a description of the heirarchy and children to the
		//	debug log.
		//----------------------------------------------------------------------------
		void DebugLogTree() const;

		//----------------------------------------------------------------------------
		//	g3dSceneNode uses a pool allocator for its new and delete
		//----------------------------------------------------------------------------
//		void* operator new(size_t size);
//		void operator delete(void* i_Ptr);

		//----------------------------------------------------------------------------
		//	Clone returns a copy of this, including children.
		//  Newly created fragments are returned in the o_NewFragments vector.
		//----------------------------------------------------------------------------
		g3dSceneNode* Clone(std::vector<g3dFragment*>& o_NewFragments) const;

	   //----------------------------------------------------------------------------
		// Clone all the children of the original scene node
		// and add the cloned children to this node as children
		// Also clone any fragments
		//  Newly created fragments are returned in the o_NewFragments vector.
		//----------------------------------------------------------------------------
		void CloneChildrenFrom( const g3dSceneNode *pOriginal,
								std::vector<g3dFragment*>& o_NewFragments ) ;

		//----------------------------------------------------------------------------
		//	GetCount returns the number of g3dSceneNodes in existence, primarily for
		//	leak checking.
		//----------------------------------------------------------------------------
		static int GetCount();

	private:

		//----------------------------------------------------------------------------
		//  debug_log_tree
		//----------------------------------------------------------------------------
		void debug_log_tree(int i_Tab) const;

		static int m_Count;

		maMatrix4x4	m_Transform;
		maMatrix4x4	m_TotalTransform;
		maMatrix4x4* m_pOrientation;
		maMatrix4x4* m_pScaleOrientation;

		g3dSceneNode* m_pParent;
		std::vector<g3dSceneNode*> m_Children;

		g3dFragment* m_pFragment;
		int m_subFragment;
		maAxisBox m_WorldBox;
		matMaterial* m_pMaterial;
		g3dAmbientEnvState* m_pAmbientEnv;
		g3dRenderState* m_pRenderState;
		DrawStyle	m_DrawStyle;
		Resolution	m_ContentResolution;

		// Pivots
		bool m_bHavePivots;
		maVector3d m_RotatePivot, m_ScalePivot;
		maVector3d m_RotatePivotTranslation, m_ScalePivotTranslation;

		// Joint nodes
		bool m_bIsJoint;
		maMatrix4x4 m_InvBindPose;

		// GPU Picking info
		envType::UInt32 m_PickCodeLow, m_PickCodeHigh;
		envType::UInt32 m_PickMask;

		std::string m_Name;
		std::string m_BakeName;

#ifdef _DEBUG
		int m_allocID;
#endif

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		struct
		{
			bool	m_bRenderable : 1;
			bool	m_bActiveInRenderLayer : 1;
			bool	m_bActiveInSceneMgr : 1;
			bool	m_bIgnoreParentTransform : 1;
			bool	m_bFog : 1;
			bool	m_bCastsShadow : 1;		// casts shadows
			//bool	m_bReceivesShadow : 1;	// receives light from shadow sources
			bool	m_bSkipAnim : 1;		// inserted control nodes should be skipped when animating
			bool	m_bSkipInfluence : 1;		// inserted control joints should be skipped when influencing verts
			bool	m_bForceLowRes : 1;
			bool	m_bGPUPickable : 1;		// render when doing pick pass
			bool	m_bPickHull : 1;		// render in pick pass even if invisible
			bool	m_bRenderableInPlanarReflection : 1;
			bool	m_bRenderableInCubeMapReflection : 1;
		} m_Flags;
};


//--------------------------------------------------------------------
//  GetTransform
//--------------------------------------------------------------------
inline const maMatrix4x4& g3dSceneNode::GetTransform() const
{
	return m_Transform;
}

//inline maMatrix4x4& g3dSceneNode::GetTransform()
//{
//	return m_Transform;
//}

//--------------------------------------------------------------------
//  GetTotalTransform
//--------------------------------------------------------------------
inline const maMatrix4x4& g3dSceneNode::GetTotalTransform() const
{
	return m_TotalTransform;
}

//inline maMatrix4x4& g3dSceneNode::GetTotalTransform()
//{
//	return m_TotalTransform;
//}

//--------------------------------------------------------------------
//  SetParent
//--------------------------------------------------------------------
inline void g3dSceneNode::SetParent(g3dSceneNode* i_pParent)
{
	m_pParent = i_pParent;
}

//--------------------------------------------------------------------
//	GetParent - This could return NULL.
//--------------------------------------------------------------------
inline g3dSceneNode* g3dSceneNode::GetParent()
{
	return m_pParent;
}

inline const g3dSceneNode* g3dSceneNode::GetParent() const
{
	return m_pParent;
}

//--------------------------------------------------------------------
//  GetChild/Children
//--------------------------------------------------------------------
inline int g3dSceneNode::GetNumChildren() const
{
	return m_Children.size();
}

inline g3dSceneNode* g3dSceneNode::GetChild(int i_Num)
{
	DBG_ASSERT((i_Num >= 0) && (i_Num < m_Children.size()), "Invalid i_Num in GetChild");
	if (i_Num < 0 || i_Num >= m_Children.size())
		return NULL;
	return m_Children[i_Num];
}

inline const g3dSceneNode* g3dSceneNode::GetChild(int i_Num) const
{
	DBG_ASSERT((i_Num >= 0) && (i_Num < m_Children.size()), "Invalid i_Num in GetChild");
	if (i_Num < 0 || i_Num >= m_Children.size())
		return NULL;
	return m_Children[i_Num];
}

inline std::vector<g3dSceneNode*>& g3dSceneNode::GetChildren()
{
	return m_Children;
}

inline const std::vector<g3dSceneNode*>& g3dSceneNode::GetChildren() const
{
	return m_Children;
}

//--------------------------------------------------------------------
//  GetRenderable
//--------------------------------------------------------------------
inline bool g3dSceneNode::GetRenderable() const
{
	return m_Flags.m_bRenderable;
}

//--------------------------------------------------------------------
//  SetRenderable
//--------------------------------------------------------------------
inline void g3dSceneNode::SetRenderable(bool i_bRenderable)
{
	m_Flags.m_bRenderable = i_bRenderable;
}

//--------------------------------------------------------------------
//	GetRenderable returns true if this node and all of its children
//	should be rendered in render layer setting.
//--------------------------------------------------------------------
inline bool g3dSceneNode::GetActiveInRenderLayer() const
{
	return m_Flags.m_bActiveInRenderLayer;
}

inline void g3dSceneNode::SetActiveInRenderLayer(bool i_bRenderable)
{
	m_Flags.m_bActiveInRenderLayer = i_bRenderable;
}

//--------------------------------------------------------------------
//	GetRenderable returns true if this node and all of its children
//	should be rendered in render layer setting.
//--------------------------------------------------------------------
inline bool g3dSceneNode::GetActiveInSceneMgr() const
{
	return m_Flags.m_bActiveInSceneMgr;
}

inline void g3dSceneNode::SetActiveInSceneMgr(bool i_bRenderable)
{
	m_Flags.m_bActiveInSceneMgr = i_bRenderable;
}

//--------------------------------------------------------------------
//  GetRenderableInPlanarReflection
//--------------------------------------------------------------------
inline bool g3dSceneNode::GetRenderableInPlanarReflection() const
{
	return m_Flags.m_bRenderableInPlanarReflection;
}

//--------------------------------------------------------------------
//  SetRenderableInPlanarReflection
//--------------------------------------------------------------------
inline void g3dSceneNode::SetRenderableInPlanarReflection(bool i_bRenderable)
{
	m_Flags.m_bRenderableInPlanarReflection = i_bRenderable;
}
//--------------------------------------------------------------------
//  GetRenderableInCubeMapReflection
//--------------------------------------------------------------------
inline bool g3dSceneNode::GetRenderableInCubeMapReflection() const
{
	return m_Flags.m_bRenderableInCubeMapReflection;
}

//--------------------------------------------------------------------
//  SetRenderableInCubeMapReflection
//--------------------------------------------------------------------
inline void g3dSceneNode::SetRenderableInCubeMapReflection(bool i_bRenderable)
{
	m_Flags.m_bRenderableInCubeMapReflection = i_bRenderable;
}

//--------------------------------------------------------------------
//  GetIgnoreParentTransform
//--------------------------------------------------------------------
inline bool g3dSceneNode::GetIgnoreParentTransform() const
{
	return m_Flags.m_bIgnoreParentTransform;
}

//--------------------------------------------------------------------
//  SetIgnoreParentTransform
//--------------------------------------------------------------------
inline void g3dSceneNode::SetIgnoreParentTransform(bool i_bIgnoreParentTransform)
{
	m_Flags.m_bIgnoreParentTransform = i_bIgnoreParentTransform;
}

//--------------------------------------------------------------------
//	GetFogged - returns true if the node should be rendered with fog.
//	Rendering without fog is generally faster so it should be
//	disabled where possible.
//--------------------------------------------------------------------
inline bool g3dSceneNode::GetFogged() const
{
	return m_Flags.m_bFog;
}

inline void g3dSceneNode::SetFogged(bool i_bFog)
{
	m_Flags.m_bFog = i_bFog;
}

//----------------------------------------------------------------------------
//	If CastsShadow is false, then the depth map render pass will 
//  not render this node and those beneath it.
//----------------------------------------------------------------------------
inline bool g3dSceneNode::GetCastsShadow() const
{
	return m_Flags.m_bCastsShadow;
}

inline void g3dSceneNode::SetCastsShadow(bool i_bShadow)
{
	m_Flags.m_bCastsShadow = i_bShadow;
}

//----------------------------------------------------------------------------
//	If ReceivesShadow is false, then this node and those beneath it
//  will not receive light from shadow sources.
//----------------------------------------------------------------------------
//inline bool g3dSceneNode::GetReceivesShadow() const
//{
//	return m_Flags.m_bReceivesShadow;
//}
//
//inline void g3dSceneNode::SetReceivesShadow(bool i_bShadow)
//{
//	m_Flags.m_bReceivesShadow = i_bShadow;
//}

//----------------------------------------------------------------------------
//	If SkipAnim is true, then the animation code should skip this node
//	because it is not part of the original hierarchy.
//----------------------------------------------------------------------------
inline bool g3dSceneNode::GetSkipAnim() const
{
	return m_Flags.m_bSkipAnim;
}
inline void g3dSceneNode::SetSkipAnim(bool i_bSkip)
{
	m_Flags.m_bSkipAnim = i_bSkip;
}

//----------------------------------------------------------------------------
//	If SkipInfluence is true, then the influence matrices code should 
//	skip this joint because it is not part of the original hierarchy.
//----------------------------------------------------------------------------
inline bool g3dSceneNode::GetSkipInfluence() const
{
	return m_Flags.m_bSkipInfluence;
}
inline void g3dSceneNode::SetSkipInfluence(bool i_bSkip)
{
	m_Flags.m_bSkipInfluence = i_bSkip;
}

//----------------------------------------------------------------------------
//	Name used to be limited, now is std::string
//----------------------------------------------------------------------------
inline const char* g3dSceneNode::GetName() const
{
	return m_Name.c_str();
}

////----------------------------------------------------------------------------
////	Name for baking texture
////----------------------------------------------------------------------------
//inline const char* g3dSceneNode::GetBakeName() const
//{
//	return m_BakeName.c_str();
//}

//--------------------------------------------------------------------
//  GetFragment
//--------------------------------------------------------------------
inline const g3dFragment* g3dSceneNode::GetFragment() const
{
	return m_pFragment;
}

inline g3dFragment* g3dSceneNode::GetFragment()
{
	return m_pFragment;
}

//--------------------------------------------------------------------
//  SetFragment
//--------------------------------------------------------------------
inline void g3dSceneNode::SetFragment( g3dFragment* i_pFragment )
{
	m_pFragment = i_pFragment;
}

//--------------------------------------------------------------------
//	If the g3dSceneNode has a material set to a non-NULL color,
//	that material will replace ("override") the fragment's material.
//--------------------------------------------------------------------
inline matMaterial* g3dSceneNode::GetMaterial()
{
	return m_pMaterial;
}

inline const matMaterial* g3dSceneNode::GetMaterial() const
{
	return m_pMaterial;
}

inline void g3dSceneNode::SetMaterial(matMaterial* i_pMaterial)
{
	m_pMaterial = i_pMaterial;
}

//--------------------------------------------------------------------
//	The render state is a struct of data the is used to render the
//  scene node and its children.  For example lights.  The render
//  state is NULL unless the render state is Set.
//--------------------------------------------------------------------
inline g3dRenderState* g3dSceneNode::GetRenderState()
{
	return m_pRenderState;
}

inline const g3dRenderState* g3dSceneNode::GetRenderState() const
{
	return m_pRenderState;
}

inline void g3dSceneNode::SetRenderState( g3dRenderState* i_pRenderState )
{
	m_pRenderState = i_pRenderState;
}

//--------------------------------------------------------------------
//	The environment state is a struct of data the is used to render 
//  the scene node's ambient environment pass and its children.  
//  The environment state is NULL unless the environment state is Set.
//--------------------------------------------------------------------
inline g3dAmbientEnvState* g3dSceneNode::GetEnvironment()
{
	return m_pAmbientEnv;
}

inline const g3dAmbientEnvState* g3dSceneNode::GetEnvironment() const
{
	return m_pAmbientEnv;
}

inline void g3dSceneNode::SetEnvironment( g3dAmbientEnvState* i_pAmbientEnv )
{
	m_pAmbientEnv = i_pAmbientEnv;
}

//----------------------------------------------------------------------------
//	The world box of the g3dSceneNode should contain all child nodes.
//----------------------------------------------------------------------------
inline const maAxisBox& g3dSceneNode::GetWorldBox() const
{
	return m_WorldBox;
}

inline void g3dSceneNode::SetWorldBox(const maAxisBox& i_Box)
{
	m_WorldBox = i_Box;
}

//----------------------------------------------------------------------------
//	The orientation is an additional rotation which is compounded during
//	animation, representing a base position for the joint.
//	GetOrientation could return NULL, in which case the node is
//	not considered to have this additional orientation information.
//----------------------------------------------------------------------------
inline const maMatrix4x4* g3dSceneNode::GetOrientation() const
{
	return m_pOrientation;
}
inline const maMatrix4x4* g3dSceneNode::GetScaleOrientation() const
{
	return m_pScaleOrientation;
}

//------------------------------------------------------------------------
// Rotate and scale pivot points for transformation.
//------------------------------------------------------------------------
inline bool g3dSceneNode::GetHasPivots() const
{
	return m_bHavePivots;
}
inline const maVector3d& g3dSceneNode::GetRotatePivot() const
{
	return m_RotatePivot;
}
inline const maVector3d& g3dSceneNode::GetScalePivot() const
{
	return m_ScalePivot;
}
inline const maVector3d& g3dSceneNode::GetRotatePivotTranslation() const
{
	return m_RotatePivotTranslation;
}
inline const maVector3d& g3dSceneNode::GetScalePivotTranslation() const
{
	return m_ScalePivotTranslation;
}

//--------------------------------------------------------------------
// If the joint flag is set to true, then this node will be used
//	to influence skinned vertices. The scale values for this node
//	should be applied to the skinned matrices, but should not
//	apply to the child nodes. The InvBindPose matrix
//	should be set when using the joint flag. Default is false.
//--------------------------------------------------------------------
inline bool g3dSceneNode::GetIsJoint() const
{
	return m_bIsJoint;
}
inline void g3dSceneNode::SetIsJoint(bool i_bJoint)
{
	m_bIsJoint = i_bJoint;
}

//--------------------------------------------------------------------
//	GetInvBindPose returns the inverse of the bind pose matrix for
//	the joint space.  The inverse of the bind pose matrix will
//	transform a vertex in model space to joint space.
//--------------------------------------------------------------------
inline const maMatrix4x4& g3dSceneNode::GetInvBindPose() const
{
	return m_InvBindPose;
}
inline void g3dSceneNode::SetInvBindPose(const maMatrix4x4& i_InvBindPose)
{
	m_InvBindPose = i_InvBindPose;
}

//--------------------------------------------------------------------
//	DrawStyle controls how the shapes below this node will render.
//	The default e_Inherit means it will inherit the draw style of
//	the parent, but other styles can be used for wireframe.
//--------------------------------------------------------------------
inline g3dSceneNode::DrawStyle g3dSceneNode::GetDrawStyle() const
{
	return m_DrawStyle;
}

//--------------------------------------------------------------------
//	ContentResolution represents what resolution is contained
//	in this section of the scene graph. The default is e_Mixed.
//	When loading a file or creating content, please set this
//	flag in order to separate out the high and low res models.
//--------------------------------------------------------------------
inline g3dSceneNode::Resolution g3dSceneNode::GetContentResolution() const
{
	return m_ContentResolution;
}

//----------------------------------------------------------------------------
//	ForceLowResolution is used like DrawStyle to control how this object
//	should be rendered. If set true, then the renderer should choose the 
//	low-resolution models beneth this node. It does not represent the 
//	content like ContentResolution does, it is used to specify a 
//	rendering choice.
//----------------------------------------------------------------------------
inline bool g3dSceneNode::GetForceLowResolution() const
{
	return m_Flags.m_bForceLowRes;
}

//----------------------------------------------------------------------------
// Returs true if this node should be rendered in the pick buffer render
// and assigned pick codes based on what color it was assigned.
//----------------------------------------------------------------------------
inline bool g3dSceneNode::GetGPUPickable() const
{
	return m_Flags.m_bGPUPickable;
}

//----------------------------------------------------------------------------
// PickHull objects render to pick buffers even if Renderable flag is false
//----------------------------------------------------------------------------
inline bool g3dSceneNode::GetPickHull() const
{
	return m_Flags.m_bPickHull;
}

//----------------------------------------------------------------------------
// PickMask is a user defined bit mask that can be used to filter
// the pickable objects. The default value of "0" means "do not filter".
//----------------------------------------------------------------------------
inline envType::UInt32 g3dSceneNode::GetPickMask() const
{
	return m_PickMask;
}

