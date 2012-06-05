/*****************************************************************************
**	smdlSubdivCharacter.hpp
**
**		smdlSubdivCharacter represents an object which is made up of
**	multiple fragments all controlled by a single jointed skeleton.
**	Each fragment has morph targets that control states for the skin
**	before the skeleton deforms it.
**
**		This subdivision version of the character model alters the
**	base mesh with the jointed animation and morpth targets and
**	then updates and renders a subdivided level of the mesh.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_SUBDIVCHARACTER_HPP
#error smdlSubdivCharacter.hpp multiply included
#endif
#define SMDL_SUBDIVCHARACTER_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef SMDL_SKELETONOBJECT_HPP
#include "Graphics/smdl/smdlSkeletonObject.hpp"
#endif

#include <map>
#include <vector>


//============================================================================
//============================================================================
class fsLocator;
class g3dFragment;
class matMaterial;
class mdlFragInfo;
class mdlSubdivInfo;
class smdlBoneDisplay;
class smdlEnhancedSurface;
class smdlHairSurface;
class smdlMeshAutoLowRes;
class smdlMeshSkinGroup;
class smdlMeshStaticGroup;
class smdlSkinnedMeshSurface;
class smdlSubdivNetwork;
struct mdlHairInfo;
struct mdlSkinInfo;
struct smdlCharacterSkin;
struct smdlSubdivNetworkSet;


//============================================================================
//============================================================================
class smdlSubdivCharacter : public smdlSkeletonObject
{
	public:
		enum JointDisplay
		{
			e_ModelOnly = 0,
			e_JointsOnly,
			e_JointsAndModel
		};

		enum SubdivMode
		{
			e_SubdivCatmullClark = 0,
			e_SubdivPNTriangle,
			e_SubdivPatch
		};

		//--------------------------------------------------------------------
		// Set and return tessellation method to use for subdivision surfaces
		//--------------------------------------------------------------------
		static SubdivMode GetSubdivMode();
		static void SetSubdivMode(SubdivMode i_SubdivMode);

		//--------------------------------------------------------------------
		// Set and return maximum level of subdivision for which the 
		//	subdivision networks should be created
		//--------------------------------------------------------------------
		static int GetMaxSubdivLevel();
		static void SetMaxSubdivLevel(int i_MaxSubdivLevel);
		
		//--------------------------------------------------------------------
		// Set and return initial level of subdivision at which the 
		//	subdivision networks should be created
		//--------------------------------------------------------------------
		static int GetInitialSubdivLevel();
		static void SetInitialSubdivLevel(int i_InitialSubdivLevel);

		//--------------------------------------------------------------------
		// Set whether a low resolution version should bo auto-generated.
		//--------------------------------------------------------------------
		static void SetAutoGenerateLowRes(bool i_bAutoGen);

		//--------------------------------------------------------------------
		//	smdlSubdivCharacter requires the joint and scene graph hierarchies,
		//	and the skin (influences and morph targets) definitions.  
		//	The scene graph hierarchy should be set into the i_pRootNode.
		//
		//	Assumes ownership of the joint hierarchy. The Skin data
		//	is not owned so that it can be shared.
		//--------------------------------------------------------------------
		smdlSubdivCharacter(	g3dSceneNode* i_pRootNode,
							const std::vector<mdlSkinInfo>& i_SkinnedSurfaces,
							const std::vector< shared_ptr<mdlHairInfo> >& i_HairSurfaces,
							const std::map<const matMaterial*,matMaterial*>& i_MaterialRemapping,
							const fsLocator& i_CharLocator);

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~smdlSubdivCharacter();

		//--------------------------------------------------------------------
		// Overrides scObject's function. Returns true if we have a 
		//	low-resolution variation of the model.
		//--------------------------------------------------------------------
		virtual bool HasLowResolutionModel() const;

		//--------------------------------------------------------------------
		//	Returns true if this model has subdivision surfaces. This model
		//	might just have skinned or baked vertex animation on polygons.
		//--------------------------------------------------------------------
		bool HasSubdivisionSurfaces() const;

		//--------------------------------------------------------------------
		// Return current subdivision level being used.
		//--------------------------------------------------------------------
		int GetCurrentSubdivLevel() const;

		//--------------------------------------------------------------------
		// Set the current subdivision level being used, this should be 
		// a level less than or equal to the return value of 
		// GetMaxSubdivLevel()
		//--------------------------------------------------------------------
		void SetCurrentSubdivLevel(int i_SubdivLevel);

		//--------------------------------------------------------------------
		// Return number of vertices in the subdivided model
		// at the current subdivision level, used for sizing
		// the array for GetSubdivVertices()
		//--------------------------------------------------------------------
		int GetNumFacesAtLevel(int i_SubdivLevel) const;

		//--------------------------------------------------------------------
		// CheckAnimation checks compatibility of this animation 
		// with the model.  It will return true if the animation can
		// be played on this model.
		//--------------------------------------------------------------------
		virtual bool CheckAnimation( const anFrameAnimation& i_GeoAnimation ) const;

		//--------------------------------------------------------------------
		//	Animate
		//--------------------------------------------------------------------
		virtual void Animate( float i_SimulationTime );

		//--------------------------------------------------------------------
		//	Animate only the scene graph nodes, 
		//  fragments should not be animated yet.
		//	This is for preparation for attachments that need the
		//	transformations in the nodes, but not the bounding boxes.
		//--------------------------------------------------------------------
		virtual void AnimateMatrices(float i_SimulationTime);

		//--------------------------------------------------------------------
		// Return number of skins in this model
		//--------------------------------------------------------------------
		int GetNumSkins();

		//--------------------------------------------------------------------
		// Return number of morph target for the skin with the given index
		//--------------------------------------------------------------------
		int GetNumMorphTargets(int i_SkinIndex);

		//--------------------------------------------------------------------
		// Return number of morph target for the skin with the given index
		//--------------------------------------------------------------------
		//std::string GetMorphTargetName(int i_SkinIndex, int i_MorphTargetIndex);

		//--------------------------------------------------------------------
		// Weight is number, usually from 0-1, that controls influence
		//	of the given morph target
		//--------------------------------------------------------------------
		float GetMorphTargetWeight(int i_SkinIndex, int i_MorphTargetIndex);
		void SetMorphTargetWeight(int i_SkinIndex, int i_MorphTargetIndex, float i_Weight);

		//--------------------------------------------------------------------
		// JointDisplay - control display of joints and model
		//--------------------------------------------------------------------
		void SetJointDisplay(JointDisplay i_Display);
		JointDisplay GetJointDisplay() const;

		//--------------------------------------------------------------------
		// GetSubdivSurfaces()
		//--------------------------------------------------------------------
		const std::vector<smdlEnhancedSurface*>& GetSubdivSurfaces();

		//--------------------------------------------------------------------
		// GetNumSubdivSurfaces()
		//--------------------------------------------------------------------
		int GetNumSubdivSurfaces();

		//--------------------------------------------------------------------
		// GetMeshGroups()
		//--------------------------------------------------------------------
		const std::vector<smdlMeshSkinGroup*>& smdlSubdivCharacter::GetMeshGroups();

		//--------------------------------------------------------------------
		// GetNumMeshGroups()
		//--------------------------------------------------------------------
		int smdlSubdivCharacter::GetNumMeshGroups();

	private:
		//--------------------------------------------------------------------
		// private function supporting Animate()
		// Gather the weights for each morph target using animation data.
		//--------------------------------------------------------------------
		void gather_morph_weights(float i_SimulationTime,
								  std::vector<std::vector<float> > &o_Weights);

	private:
		//std::vector<smdlSubdivNetwork*> m_Subdivs;
	
		smdlSubdivNetworkSet* m_pSharedNetworkSet;
		std::vector<smdlEnhancedSurface*> m_SubdivSurfaces;
		std::vector<smdlSkinnedMeshSurface*> m_SingleSkins;
		std::vector<smdlMeshSkinGroup*> m_MeshGroups;
		std::vector<smdlHairSurface*> m_HairSurfaces;
		smdlBoneDisplay*		m_pBoneDisplay;
		smdlMeshAutoLowRes*		m_pAutoLowRes;
		std::vector<g3dFragment*> m_Fragments;
		std::map<std::string, g3dSceneNode*> m_SkinNodes;

		//const std::vector<smdlCharacterSkin>& m_Skins;
		std::vector<std::vector<float> > m_Weights;		// weights set from API
		std::vector<std::vector<float> > m_AnimWeights;	// weights set from animation data, combined with m_Weights
		bool m_bMorphDirty;
		bool m_bHighResDirty;
		int m_CurSubdivLevel;
		int m_BaseChildOffset;
		JointDisplay m_JointDisplay;
};

