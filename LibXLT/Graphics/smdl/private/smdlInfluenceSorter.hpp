/*****************************************************************************
**	smdlInfluenceSorter.hpp
**
**		smdlInfluenceSorter splits up skinned fragments in order to join
**	them together into static meshes to be used as a low-res model.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_INFLUENCESORTER_HPP
#error smdlInfluenceSorter.hpp multiply included
#endif
#define SMDL_INFLUENCESORTER_HPP

#ifndef MDL_FRAGINFO_HPP
#include "Graphics/mdl/mdlFragInfo.hpp"
#endif


//============================================================================
//============================================================================
class g3dFragment;
class g3dSceneNode;
struct mdlSplitFragInfo;
struct smdlCharacterSkin;


//============================================================================
//============================================================================
class smdlInfluenceSorter 
{
	public:
		//--------------------------------------------------------------------
		//	smdlInfluenceSorter requires the root scene node of the skeleton.
		//--------------------------------------------------------------------
		smdlInfluenceSorter(g3dSceneNode *i_pRootNode);

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		~smdlInfluenceSorter();

		//--------------------------------------------------------------------
		// Submit the given fragment and skin into the sorter
		//--------------------------------------------------------------------
		void Submit(const mdlFragInfo &i_FragInfo, 
					const smdlCharacterSkin &i_Skin);
		void Submit(const mdlSplitFragInfo &i_SplitFrag, 
					const smdlCharacterSkin &i_Skin);

		//--------------------------------------------------------------------
		// CreateFragments from the fragments that has been submitted
		//--------------------------------------------------------------------
		void CreateFragments(matMaterial *i_pLowResMat,
							 std::vector<g3dFragment*> &o_Fragments,
							 std::vector<g3dSceneNode*> &o_ParentNodes);

	private:
		//--------------------------------------------------------------------
		// Internal structure
		//--------------------------------------------------------------------
		struct sJointGather
		{
			g3dSceneNode* m_pJoint;
			int m_ParentIndex;
			std::vector<envType::UInt32> m_Indices;
		};

		//--------------------------------------------------------------------
		// internal routine for setting up structures for skeleton
		//--------------------------------------------------------------------
		void gather_joints(g3dSceneNode* i_pJoint,
					  std::vector<sJointGather> &o_Joints,
					  int i_ParentIndex);

		//--------------------------------------------------------------------
		// internal generic submit function
		//--------------------------------------------------------------------
		template<class T>
		void Submit(const T &i_Frag, 
					const smdlCharacterSkin &i_Skin);

	private:
		std::vector<sJointGather> m_Joints;
		mdlFragInfo m_JoinedFrag;
};
