/*****************************************************************************
**	smdlHierarchyObject.cpp
**
**		smdlHierarchyObject represents an object which can be animated using
**	a hierarchical animation.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/smdlHierarchyObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/smdl/private/smdlMeshStaticGroup.hpp"
#include "Graphics/smdl/private/smdlSubAnimation.hpp"
#include "Graphics/smdl/smdlGeoFrameAnimation.hpp"


//============================================================================
//============================================================================
namespace
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	// Cubic blending code
	template<class V>
	V DoCubicBlend(const V& i_Val1, const V& i_Val2,
			   const V& i_Gradient1, const V& i_Gradient2,
			   float i_U)
	{
	float u2 = i_U*i_U;
	float u3 = u2*i_U;

	return (i_Val1 * (2*u3 - 3*u2 + 1) +
			i_Val2 * (3*u2 - 2*u3) +
			i_Gradient1 * (u3 - 2*u2 + i_U) +
			i_Gradient2 * (u3 - u2));
	}

	//----------------------------------------------------------------------------
	// Set values from rotation and translation into matrix in node
	//----------------------------------------------------------------------------
	void set_xform_values(g3dSceneNode* io_pSceneNode,
						  const maRotation &i_Rotate,
						  const maVector3d &i_Translate,
						  const maVector3d &i_Scale,
						  const bool bIgnoreJointOrientation,
						  const maVector3d &i_ParentScale,
						  const bool i_bInvertParentScale)
	{
		maMatrix4x4 transform;

		bool bHavePivots = io_pSceneNode->GetHasPivots();

		// Scale
		if (bHavePivots)
		{
			// We could probably set this all in one step faster
			const maVector3d& scale_pivot =  io_pSceneNode->GetScalePivot();
			const maVector3d& scale_pivot_trans =  io_pSceneNode->GetScalePivotTranslation();
			transform.MakeTranslate( -scale_pivot.m_X, -scale_pivot.m_Y, -scale_pivot.m_Z );
			transform.ScaleBy( i_Scale.m_X, i_Scale.m_Y, i_Scale.m_Z );
			transform.TranslateBy( scale_pivot.m_X + scale_pivot_trans.m_X, 
								   scale_pivot.m_Y + scale_pivot_trans.m_Y, 
								   scale_pivot.m_Z + scale_pivot_trans.m_Z );
		}
		else
		{
			transform.MakeScale( i_Scale.m_X, i_Scale.m_Y, i_Scale.m_Z );
		}

		const maMatrix4x4* pScaleOrient = io_pSceneNode->GetScaleOrientation();

		// Check for HJoint, with scale anims, now we have to do scale orient also
		if (pScaleOrient && !bIgnoreJointOrientation)
		{
			transform *= *pScaleOrient;
		}

		// Orient
		if (bHavePivots)
		{
			// Rotate around a point that is not (0,0,0)
			const maVector3d& rot_pivot =  io_pSceneNode->GetRotatePivot();
			const maVector3d& rot_pivot_trans =  io_pSceneNode->GetRotatePivotTranslation();
			maMatrix4x4 pvt_mtx;
			//pvt_mtx.MakeTranslate( -rot_pivot.m_X, -rot_pivot.m_Y, -rot_pivot.m_Z );
			//transform *= pvt_mtx;
			transform.TranslateBy( -rot_pivot.m_X, -rot_pivot.m_Y, -rot_pivot.m_Z );
			transform *= i_Rotate.GetMatrix();
			//pvt_mtx.MakeTranslate( rot_pivot.m_X, rot_pivot.m_Y, rot_pivot.m_Z );
			//transform *= pvt_mtx;
			transform.TranslateBy( rot_pivot.m_X + rot_pivot_trans.m_X, 
								   rot_pivot.m_Y + rot_pivot_trans.m_Y, 
								   rot_pivot.m_Z + rot_pivot_trans.m_Z );
		}
		else
		{
			transform *= i_Rotate.GetMatrix();
		}

		const maMatrix4x4* pOrientation = io_pSceneNode->GetOrientation();

		// Check for HJoint, need to use orientation here
		if (pOrientation && !bIgnoreJointOrientation)
		{
			transform *= *pOrientation;
		}

		// Joints only apply scale to self and influences, so 
		// remove the parent scaling in this transform
		//if (i_bInvertParentScale)
		if (i_bInvertParentScale && io_pSceneNode->GetIsJoint())
		{
			maMatrix4x4 inv_ps;
			inv_ps.MakeScale( 1.0f/i_ParentScale.m_X, 
							  1.0f/i_ParentScale.m_Y, 
							  1.0f/i_ParentScale.m_Z );
			transform *= inv_ps;
		}

		transform.TranslateBy(i_Translate.m_X, i_Translate.m_Y, i_Translate.m_Z);

		io_pSceneNode->SetTransform(transform);
	}

	//----------------------------------------------------------------------------
	// combine sub animation influence with rotation and translation from base anim
	//----------------------------------------------------------------------------
	void apply_subanims(maRotation &io_Rotate,
						maVector3d &io_Translate,
						maVector3d &io_Scale,
						const std::vector<smdlSubAnimationIterator*>& i_SubAnims)
	{
		// combine all of the subanimation rotations and then combine
		// that with the base anim rotation at the end.
		maRotation sum_rotation, post_rotation;
		maVector3d post_translate(0,0,0);
		maVector3d post_scale(0,0,0);
		float sum_blend = 0.0f;
		bool have_blend = false, have_post = false;

		int subanim_count = i_SubAnims.size();

		std::vector<smdlSubAnimationIterator*>::const_iterator it, end = i_SubAnims.end();
		for (it = i_SubAnims.begin(); it != end; ++it)
		{
			const smdlSubAnimationIterator* subanim = (*it);
			// Check to see if subanim influences this section of hierarchy
			if (subanim->IsAttached())
			{
				const smdlGeoAnimKeys* instance = subanim->GetData();

				//	We should only change the matrix if there are actually animation channels
				//
				if( instance->HasAnimation() )
				{
					maVector3d translate = instance->GetTranslation(subanim->GetCurrentFrame());
					maRotation rotate = instance->GetRotation(subanim->GetCurrentFrame());
					maVector3d scale = instance->GetScale(subanim->GetCurrentFrame());

					float blend = subanim->GetBlend();
					if (blend > 0.0f)
					{
						if (subanim->GetAdditive())
						{
							// Additive blend, just add rotation and translation in 
							// afterwards.
							have_post = true;
							rotate.ScaleAngle(blend);
							post_rotation *= rotate;
							post_translate += (translate * blend);
							
							// Have to expand out component multiply
							//post_scale *= (scale * blend);
							post_scale.m_X *= (scale.m_X * blend);
							post_scale.m_Y *= (scale.m_Y * blend);
							post_scale.m_Z *= (scale.m_Z * blend);
						}
						else
						{
							// Not additive blend, meaning remove some of the old animation
							// when adding new animation in.
							io_Translate = io_Translate + (translate - io_Translate) * blend;
							io_Scale = io_Scale + (scale - io_Scale) * blend;

							// First style of blending is just lerp/slerp. But, there should be a way to
							// have 2 expressions near 1.0 and have the result blend between them
							//io_Rotate.Slerp(io_Rotate, rotate, blend);

							// Trying to slerp all of the subanimations together and then combine with
							// the base anim at the end.
							sum_blend += blend;
							if (!have_blend)
							{
								// first rotation is just set, not blended
								sum_rotation = rotate;
								have_blend = true;
							}
							else
							{
								sum_rotation.Slerp(sum_rotation, rotate, blend / sum_blend);
							}
						}
					}
				}
			}
		}

		// Now blend with base anim
		if (have_blend)
		{
			// it's hard to say how to handle the base blend with the sum_blend.
			// This clamp means as soon as multiple subanims sum up to 1.0, then
			// the base animation is no longer visible. An alternative would
			// be to average the blend amounts (i.e. sum_blends/num_blends)
			if (sum_blend >= 1.0f)
			{
				io_Rotate = sum_rotation;
			}
			else
			{
				io_Rotate.Slerp(io_Rotate, sum_rotation, sum_blend);
			}
		}

		// Now add in additive translation and rotation
		if (have_post)
		{
			io_Rotate *= post_rotation;
			io_Translate += post_translate;
			io_Scale += post_scale;
		}
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void subanim_move_child(int i_ChildIndex,
							g3dSceneNode* i_ChildNode,
							std::vector<smdlSubAnimationIterator*>& io_SubAnims)
	{
		std::vector<smdlSubAnimationIterator*>::iterator it, end = io_SubAnims.end();
		for (it = io_SubAnims.begin(); it != end; ++it)
		{
			(*it)->MoveToChild(i_ChildIndex, *i_ChildNode);
		}
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void subanim_move_parent(std::vector<smdlSubAnimationIterator*>& io_SubAnims)
	{
		std::vector<smdlSubAnimationIterator*>::iterator it, end = io_SubAnims.end();
		for (it = io_SubAnims.begin(); it != end; ++it)
		{
			(*it)->MoveToParent();
		}
	}

	//----------------------------------------------------------------------------
	//	get children of scene node, checking for SkipAnim flag
	//----------------------------------------------------------------------------
	void get_child_nodes(g3dSceneNode* io_pSceneNode,
						 std::vector<g3dSceneNode*> &o_Nodes)
	{
		int num_children = io_pSceneNode->GetNumChildren();
		for (int i = 0 ; i < num_children ; i++ )
		{
			g3dSceneNode* child_node = io_pSceneNode->GetChild(i);
			if (child_node->GetSkipAnim())
			{
				get_child_nodes(child_node, o_Nodes);
			}
			else
			{
				o_Nodes.push_back(child_node);
			}
		}
	}

	//----------------------------------------------------------------------------
	//	one animation, no blend version
	//----------------------------------------------------------------------------
	void animate_level(	g3dSceneNode* io_pSceneNode,
						smdlTree<smdlGeoAnimKeys>::const_iterator& io_Instance,
						float i_Time,
						std::vector<smdlSubAnimationIterator*>& i_SubAnims,
						const bool bIgnoreJointOrientation,
						const maVector3d &i_ParentScale,
						const bool i_bInvertParentScale)
	{
		//	produce a matrix from these animation channels
		const smdlGeoAnimKeys& instance = io_Instance.GetData();

		//	We should only change the matrix if there are actually animation channels
		//
		maVector3d cur_scale(1,1,1);
		if( instance.HasAnimation() )
		{
			maVector3d translate = instance.GetTranslation(i_Time);
			maRotation rotate = instance.GetRotation(i_Time);
			maVector3d scale = instance.GetScale(i_Time);

			apply_subanims(rotate, translate, scale, i_SubAnims);
			set_xform_values(io_pSceneNode, 
				rotate, translate, scale, 
				bIgnoreJointOrientation, i_ParentScale, i_bInvertParentScale);

			if (instance.GetVisible() != NULL)
			{
				// set visibility
				io_pSceneNode->SetRenderable( instance.GetVisible(i_Time) );
			}

			// track scale in animation in order to invert it in joints
			cur_scale = scale;
		}

		//	fine...now animate the child nodes
		std::vector<g3dSceneNode*> children;
		children.reserve(io_pSceneNode->GetNumChildren());
		get_child_nodes(io_pSceneNode, children);

		// Allow animation tree to be shorter than geometry tree.
		// This allows connections between animating parts
		// and subparts
		//
		int anim_num = io_Instance.NumChildren();
		int child_num = children.size();
		//DBG_ASSERT(io_Instance.NumChildren() <= children.size(), "Model hierarchy not analogous to animation hierarchy (%s)", io_pSceneNode->GetName());
		
		// Choose fewest number of children to avoid crash.
		// However, if the hierarchies don't match, the results won't be right.
		int num_children = maFunctions::Lowest(io_Instance.NumChildren(), (int)children.size());
		// Joints apply their scale only to skinning matrices, not to child nodes.
		const bool bInvertParentScale = io_pSceneNode->GetIsJoint();
		for (int i = 0 ; i < num_children ; i++ )
		{
			io_Instance.MoveToChild(i);
			g3dSceneNode* child_frag = children[i];
			subanim_move_child(i, child_frag, i_SubAnims);
			animate_level(child_frag, io_Instance, i_Time, i_SubAnims, 
				bIgnoreJointOrientation, cur_scale, bInvertParentScale);
			subanim_move_parent(i_SubAnims);
			io_Instance.MoveToParent();
		}
	}

	//----------------------------------------------------------------------------
	// no blend, multiple anim trees version
	//----------------------------------------------------------------------------
	void animate_level(	std::vector<g3dSceneNode*> io_RootNodes,
						const smdlKeyRootMap& io_KeyRoots,
						float i_Time,
						std::vector<smdlSubAnimationIterator*>& i_SubAnims,
						const bool i_bIgnoreJointOrientation,
						const maVector3d &i_ParentScale,
						const bool i_bInvertParentScale)
	{			
		DBG_ASSERT(io_RootNodes.size() == io_KeyRoots.size(), "Have wrong number of root nodes; " << io_RootNodes.size() << " != " << io_KeyRoots.size());
		if (io_RootNodes.size() != io_KeyRoots.size())
			return;
		int ind = 0;
		smdlKeyRootMap::const_iterator it;
		for (it = io_KeyRoots.begin(); it != io_KeyRoots.end(); ++it, ++ind)
		{
			const smdlTree<smdlGeoAnimKeys> &keys = (*it->second);
			animate_level(io_RootNodes[ind], keys.GetIterator(), 
							i_Time, i_SubAnims, i_bIgnoreJointOrientation, 
							i_ParentScale, i_bInvertParentScale);
		}
	}

	//----------------------------------------------------------------------------
	//	blend version
	//----------------------------------------------------------------------------
	void animate_level(	g3dSceneNode* io_pSceneNode,
						smdlTree<smdlGeoAnimKeys>::const_iterator& io_Instance1,
						smdlTree<smdlGeoAnimKeys>::const_iterator& io_Instance2,
						float i_CurTime,
						float i_BlendTime,
						float i_BlendParam,
						bool i_bDoCubicBlend,
						float i_EaseOutScale,
						float i_EaseInScale,
						std::vector<smdlSubAnimationIterator*>& i_SubAnims,
						const bool bIgnoreJointOrientation,
						const maVector3d &i_ParentScale,
						const bool i_bInvertParentScale )
	{
		//	produce a matrix from these animation channels
		const smdlGeoAnimKeys& instance1 = io_Instance1.GetData();
		const smdlGeoAnimKeys& instance2 = io_Instance2.GetData();

		//	We should only change the matrix if there are actually animation channels
		//
		maVector3d cur_scale(1,1,1);
		if( instance1.HasAnimation() || instance2.HasAnimation())
		{
			maVector3d translate1 = instance1.GetTranslation(i_CurTime);
			maRotation rotate1 = instance1.GetRotation(i_CurTime);
			maVector3d scale1 = instance1.GetScale(i_CurTime);

			maVector3d translate2 = instance2.GetTranslation(i_BlendTime);
			maRotation rotate2 = instance2.GetRotation(i_BlendTime);
			maVector3d scale2 = instance2.GetScale(i_BlendTime);

			maVector3d translate, scale;
			maRotation rotate;

			if (i_bDoCubicBlend)
			{
				const float c_DeltaTime = 1.0f; // go back one frame

				maVector3d tgrad1 = translate1 - instance1.GetTranslation(i_CurTime - c_DeltaTime);
				tgrad1 *= i_EaseOutScale;
				maVector3d tgrad2 = instance2.GetTranslation(i_BlendTime + c_DeltaTime) - translate2;
				tgrad2 *= i_EaseInScale;
				translate = DoCubicBlend(translate1, translate2, tgrad1, tgrad2, i_BlendParam);

				maVector3d sgrad1 = scale1 - instance1.GetScale(i_CurTime - c_DeltaTime);
				sgrad1 *= i_EaseOutScale;
				maVector3d sgrad2 = instance2.GetScale(i_BlendTime + c_DeltaTime) - scale2;
				sgrad2 *= i_EaseInScale;
				scale = DoCubicBlend(scale1, scale2, sgrad1, sgrad2, i_BlendParam);

				// For rotation, do slerp, but with cubic alpha value?
				float cubic_alpha = 3*i_BlendParam*i_BlendParam - 2*i_BlendParam*i_BlendParam*i_BlendParam;
				maFunctions::Clamp(cubic_alpha, 0.0f, 1.0f);
				rotate.Slerp(rotate1, rotate2, cubic_alpha);
			}
			else
			{
				//	lerp the translate values and slerp the rotations
				//
				translate = translate1 + (translate2 - translate1) * i_BlendParam;
				rotate.Slerp(rotate1, rotate2, i_BlendParam);
				scale = scale1 + (scale2 - scale1) * i_BlendParam;
			}

			apply_subanims(rotate, translate, scale, i_SubAnims);
			set_xform_values(io_pSceneNode, rotate, translate, scale, 
				bIgnoreJointOrientation, i_ParentScale, i_bInvertParentScale);

			if (instance1.GetVisible() != NULL)
			{
				// set visibility
				io_pSceneNode->SetRenderable( instance1.GetVisible(i_CurTime) );
			}

			// track scale in animation in order to invert it in joints
			cur_scale = scale;
		}

		//	fine...now animate the child nodes
		std::vector<g3dSceneNode*> children;
		children.reserve(io_pSceneNode->GetNumChildren());
		get_child_nodes(io_pSceneNode, children);

		//DBG_ASSERT(num_children <= io_pSceneNode->GetNumChildren(), "Model hierarchy not analogous to animation hierarchy (%s)", io_pSceneNode->GetName());

		int num_children = maFunctions::Lowest(io_Instance1.NumChildren(), io_Instance2.NumChildren(), (int)children.size());
		const bool bInvertParentScale = io_pSceneNode->GetIsJoint();
		for( int i = 0 ; i < num_children ; i++ )
		{
			io_Instance1.MoveToChild(i);
			io_Instance2.MoveToChild(i);
			g3dSceneNode* child_frag = children[i];
			subanim_move_child(i, child_frag, i_SubAnims);
			animate_level(child_frag, io_Instance1, io_Instance2,
				i_CurTime, i_BlendTime, i_BlendParam, 
				i_bDoCubicBlend, i_EaseOutScale, i_EaseInScale, i_SubAnims, 
				bIgnoreJointOrientation, cur_scale, bInvertParentScale);
			subanim_move_parent(i_SubAnims);
			io_Instance1.MoveToParent();
			io_Instance2.MoveToParent();
		}
	}

	//----------------------------------------------------------------------------
	//	blend, multiple anim trees  version
	//----------------------------------------------------------------------------
	void animate_level(	std::vector<g3dSceneNode*> io_RootNodes,
						const smdlKeyRootMap& io_KeyRoots1,
						const smdlKeyRootMap& io_KeyRoots2,
						float i_CurTime,
						float i_BlendTime,
						float i_BlendParam,
						bool i_bDoCubicBlend,
						float i_EaseOutScale,
						float i_EaseInScale,
						std::vector<smdlSubAnimationIterator*>& i_SubAnims,
						const bool i_bIgnoreJointOrientation,
						const maVector3d &i_ParentScale,
						const bool i_bInvertParentScale )
	{
		DBG_ASSERT(io_RootNodes.size() == io_KeyRoots1.size(), "Have wrong number of root nodes; " << io_RootNodes.size() << " != " << io_KeyRoots1.size());
		if (io_RootNodes.size() != io_KeyRoots1.size())
			return;
		DBG_ASSERT(io_RootNodes.size() == io_KeyRoots2.size(), "Have wrong number of root nodes; " << io_RootNodes.size() << " != " << io_KeyRoots2.size());
		if (io_RootNodes.size() != io_KeyRoots2.size())
			return;
		
		int ind = 0;
		smdlKeyRootMap::const_iterator it1, it2;
		for (it1 = io_KeyRoots1.begin(), it2 = io_KeyRoots2.begin(); it1 != io_KeyRoots1.end(); ++it1, ++it2, ++ind)
		{
			const smdlTree<smdlGeoAnimKeys> &keys1 = (*it1->second);
			const smdlTree<smdlGeoAnimKeys> &keys2 = (*it2->second);
			animate_level(io_RootNodes[ind], keys1.GetIterator(), keys2.GetIterator(), 
							i_CurTime, i_BlendTime,i_BlendParam,
							i_bDoCubicBlend, i_EaseOutScale, i_EaseInScale,
							i_SubAnims, i_bIgnoreJointOrientation, 
							i_ParentScale, i_bInvertParentScale);
		}
	}

	//--------------------------------------------------------------------
	// find first child of node that is a joint
	//--------------------------------------------------------------------
	g3dSceneNode* find_first_joint(g3dSceneNode *i_pHierarchyRoot)
	{
		if (i_pHierarchyRoot->GetIsJoint())
			return i_pHierarchyRoot;

		for (int i=0; i<i_pHierarchyRoot->GetNumChildren(); i++)
		{
			if (g3dSceneNode *pNode = find_first_joint(i_pHierarchyRoot->GetChild(i)))
				return pNode;
		}
		return NULL;
	}

	//--------------------------------------------------------------------
	// get root nodes of given animation
	//--------------------------------------------------------------------
	void get_roots_of_animation(const smdlGeoFrameAnimInstance* i_pCurAnim,
								g3dSceneNode *i_pHierarchyRoot,
								std::vector<g3dSceneNode*>& o_RootNodes)
	{
		if (i_pCurAnim)
		{
			int num_roots = i_pCurAnim->GetAnim().GetKeyRoots().size();
			if ((num_roots == 1) && (i_pCurAnim->GetAnim().GetAttachToRootJoint()))
			{
				g3dSceneNode *joint = find_first_joint(i_pHierarchyRoot);
				if (joint)
					o_RootNodes.push_back( joint );
				else
					o_RootNodes.push_back( i_pHierarchyRoot );

			}
			else
			{

				const smdlKeyRootMap& key_roots = 
					i_pCurAnim->GetAnim().GetKeyRoots();

				// Get root node for each separate tree of animation data
				smdlKeyRootMap::const_iterator it;
				for (it = key_roots.begin(); it != key_roots.end(); ++it)
				{
					g3dSceneNode *node = NULL;
					if (!it->first.empty())
					{
						node = i_pHierarchyRoot->GetNamedNode(it->first.c_str());
					}
					if (node)
						o_RootNodes.push_back( node );
					else
						o_RootNodes.push_back( i_pHierarchyRoot );
				}
			}

			// We will later assume that the lengths match, so assert it here
			DBG_ASSERT(o_RootNodes.size() == num_roots, "Produced wrong number of root nodes; " << o_RootNodes.size() << " != " << num_roots);
		}
	}
}

//--------------------------------------------------------------------
//	The object will initially
//	be placed at the origin with unit scale and no rotation.
//--------------------------------------------------------------------
smdlHierarchyObject::smdlHierarchyObject( g3dSceneNode* i_pHierarchyRoot )
:	m_pHierarchy( i_pHierarchyRoot )
//	m_bLocalScaling(false)
{
	GetBase()->AddChild( i_pHierarchyRoot );

	// Gather up the static fragments within the hierarchy
	m_pStaticMeshes = new smdlMeshStaticGroup( i_pHierarchyRoot );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlHierarchyObject::~smdlHierarchyObject()
{
	delete m_pStaticMeshes;
}

//--------------------------------------------------------------------
// Overrides scObject's function. Returns true if we have a 
//	low-resolution variation of the model.
//--------------------------------------------------------------------
//virtual 
bool smdlHierarchyObject::HasLowResolutionModel() const
{
	return m_pStaticMeshes->HasLowResolution();
}

//--------------------------------------------------------------------
//	Animate
//--------------------------------------------------------------------
void smdlHierarchyObject::Animate(float i_SimulationTime)
{
	const smdlGeoFrameAnimInstance* cur_anim = this->GetCurAnimInstance();
	if( cur_anim )
	{
		float cur_frame = cur_anim->ComputeFrame(i_SimulationTime);
		bool bIgnoreJointOrientation = cur_anim->GetAnim().GetIgnoreJointOrientation();
		const maVector3d root_scale(1,1,1);
		//bool bInvertParentScale = this->m_bLocalScaling;
		bool bInvertParentScale = false; // we now determine this at each node based on the GetIsJoint() flag

		// Prepare subanimation iterators
		int num_subanims = this->GetNumSubAnims();
		std::vector<smdlSubAnimationIterator*> subanims;
		for (int i=0; i<num_subanims; i++)
		{
			const smdlSubAnimation *pSubAnim = this->GetSubAnimation(i);
			float current_frame = pSubAnim->GetAnimInstance()->ComputeFrame(i_SimulationTime);

			const smdlKeyRootMap& key_roots = 
				pSubAnim->GetAnimInstance()->GetKeyRoots();

			// Create subanimation iterator for each separate tree of animation data
			smdlKeyRootMap::const_iterator it;
			for (it = key_roots.begin(); it != key_roots.end(); ++it)
			{
				subanims.push_back( new smdlSubAnimationIterator(it->first, *it->second, 
																 current_frame, m_pHierarchy,
																 pSubAnim->GetBlend(),
																 pSubAnim->GetAdditive()) );
			}
		}

		// Check for subanim as base anim first. We cannot blend between anims that
		// have different root nodes.
		//
		std::vector<g3dSceneNode*> root_nodes;
		get_roots_of_animation(cur_anim, m_pHierarchy, root_nodes);

		const smdlGeoFrameAnimInstance* blend_anim = GetBlendAnimInstance();
		std::vector<g3dSceneNode*> blend_nodes;
		if (blend_anim)
			get_roots_of_animation(blend_anim, m_pHierarchy, blend_nodes);

		// Can only blend if the animation trees match up 
		if ( blend_anim && (root_nodes == blend_nodes))
		{
			float blend_frame = blend_anim->ComputeFrame(i_SimulationTime);
			float blend_param = this->GetBlendRatio(i_SimulationTime);

			if( blend_param <= 0.0f )
			{
				//	no real blend, just use cur anim
				//
				animate_level(root_nodes, cur_anim->GetKeyRoots(), 
					cur_frame, subanims, bIgnoreJointOrientation, 
					root_scale, bInvertParentScale);
			}
			else if( blend_param >= 1.0f )
			{
				//	only blend anim
				//
				animate_level(blend_nodes, blend_anim->GetKeyRoots(), 
					blend_frame, subanims, bIgnoreJointOrientation, 
					root_scale, bInvertParentScale);

				//	fully blended - get rid of cur anim
				//
				this->SwitchToBlend(i_SimulationTime);
			}
			else
			{
				float EaseOutScale = cur_anim->GetFrameRate() * this->GetBlendLength();
				float EaseInScale = blend_anim->GetFrameRate() * this->GetBlendLength();

				EaseOutScale *= this->GetEaseOutWeight();
				EaseInScale *= this->GetEaseInWeight();

				//	have to do a real blend
				//
				animate_level(	root_nodes, 
								cur_anim->GetKeyRoots(),
								blend_anim->GetKeyRoots(),
								cur_frame,
								blend_frame,
								blend_param, 
								this->IsSmoothBlend(),
								EaseOutScale,
								EaseInScale,
								subanims,
								bIgnoreJointOrientation,
								root_scale,
								bInvertParentScale);
			}
		}
		else
		{
			//	no blend or not compatible roots, just use cur anim
			//
			animate_level(root_nodes, cur_anim->GetKeyRoots(), cur_frame, 
				subanims, bIgnoreJointOrientation, root_scale, bInvertParentScale);
		}

		envSTLHelpers::DeleteContainer(subanims);
	}

	// Do base (animation controls) after the bones have moved from
	// keyframe animations
	scAnimatableObject::Animate( i_SimulationTime );
}

//--------------------------------------------------------------------
// In joint skeletons, scale values should only affect the
//	current joint and its influences. Mark this true to
//	keep scaling from propagating to the child nodes.
//--------------------------------------------------------------------
//void smdlHierarchyObject::SetLocalScaling(bool i_bLocal)
//{
//	this->m_bLocalScaling = i_bLocal;
//}

