/*****************************************************************************
**	smdlPatchSurface.hpp
**
**		smdlPatchSurface handles a PN Triangle approximation to a Catmull-Clark Subdivision surface. 
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_PATCHSURFACE_HPP
#error smdlPatchSurface.hpp multiply included
#endif
#define SMDL_PATCHSURFACE_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef SMDL_SMDLPATCHTESSELLATOR_HPP
#include "Graphics/smdl/private/smdlPatchTessellator.hpp"
#endif
#ifndef SMDL_SURFACE_HPP
#include "Graphics/smdl/private/smdlSurface.hpp"
#endif
#ifndef SMDL_VERTEXANIMKEYS_HPP
#include "Graphics/smdl/smdlVertexAnimKeys.hpp"
#endif 

#include <vector>


//============================================================================
//============================================================================
class g3dFragment;
class g3dSceneNode;
class maMatrix4x4;
class matMaterial;
class mdlSubdivInfo;
class smdlSubdivNetwork;
struct smdlCharacterSkin;


//============================================================================
//============================================================================
class smdlPatchSurface : public smdlEnhancedSurface
{
	public:
		//--------------------------------------------------------------------
		//	smdlPatchSurface requires a subdivision network and the 
		//		character skin info with information about how it animates.
		//--------------------------------------------------------------------
		smdlPatchSurface(	const mdlSubdivInfo& i_SubdivInfo,
							shared_ptr<smdlSubdivNetwork> i_pSubdivNetwork,
							matMaterial* i_pMaterial,
							const smdlCharacterSkin& i_CharacterSkin );

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		~smdlPatchSurface();

		//--------------------------------------------------------------------
		// Return node that contains the fragments in this model in a small
		//	sub-scene graph
		//--------------------------------------------------------------------
		g3dSceneNode* RootNode();

		//--------------------------------------------------------------------
		//	Visible - set/get whether the given surface is renderable
		//--------------------------------------------------------------------
		virtual void SetVisible(bool i_bVisible);
		virtual bool GetVisible() const;

		//--------------------------------------------------------------------
		// Name for subdiv, used to look for baked vertex animation
		//--------------------------------------------------------------------
		inline const std::string& GetSubdivName() const;

		//--------------------------------------------------------------------
		// Set the current subdivision level for the surface.
		//--------------------------------------------------------------------
		void SetCurrentSubdivLevel(int i_SubdivLevel);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline virtual int GetNumFacesAtLevel( int i_SubdivLevel ) const;

		//--------------------------------------------------------------------
		// Const access to subdiv network
		//--------------------------------------------------------------------
		inline const smdlPatchTessellator& GetPatchTessellator() const;

		//--------------------------------------------------------------------
		// Const access to character skin
		//--------------------------------------------------------------------
		inline const smdlCharacterSkin& GetCharacterSkin() const;

		//--------------------------------------------------------------------
		//	TransformSubdiv - given the joint matrices and weights for the
		//		influence of the morph targets, transform the
		//		base mesh based on the vertex influences. Then
		//		propagate the base mesh changes through to the current
		//		subdivision level.
		//--------------------------------------------------------------------
		virtual void TransformSubdiv( const maMatrix4x4* i_BoneMatrices,
							  const std::vector<float>& i_Weights );

		//--------------------------------------------------------------------
		// CheckAnimation checks compatibility of this animation 
		// with the model. It will return true if the animation can
		// be played on this model.
		//--------------------------------------------------------------------
		virtual bool CheckAnimation( const smdlVertexAnimKeys &i_VertKeys ) const;

		//--------------------------------------------------------------------
		//	VertexTransformSubdiv - use baked vertex animation to transform
		//		the base mesh of the subdivision surface.
		//--------------------------------------------------------------------
		virtual void VertexTransformSubdiv(float i_CurrentFrame,
								   const smdlVertexAnimKeys &i_VertKeys);		
		
		//--------------------------------------------------------------------
		// Animation data is giving us the bounding box for the surface 
		//	before the actual animation is done. Set this bounding box 
		//	into the fragment.
		//--------------------------------------------------------------------
		virtual void BBoxTransform(const maAxisBox &i_BBox);

	private:
		//--------------------------------------------------------------------
		// Update our fragment from the current state of the base mesh
		//	in the subdivision network. Used in animation.
		//--------------------------------------------------------------------
		void update_fragment();
		void CreateFragment();	//creates either a base or patch fragment (based on hardware flag)

	private:
		shared_ptr<smdlSubdivNetwork> m_spSubdivNetwork;

		g3dSceneNode *m_pNode;
		std::string m_SubdivName;
		std::vector<maPoint3d> m_BasePositions;
		std::vector<maVector3d> m_BaseNormals;
		const smdlCharacterSkin& m_CharacterSkin;
		g3dFragment* m_pFragment;
		matMaterial* m_pMaterial;
		std::vector<int> m_VertexRemapping;
		int m_NumOriginalVertices;
		std::vector<maPoint3d> m_VertCache;
		std::vector<maVector3d> m_NormCache;
		smdlPatchTessellator m_PatchTessellator;
		int m_nCurrentSubdivLevel;
};

//--------------------------------------------------------------------
// Name for subdiv, used to look for baked vertex animation
//--------------------------------------------------------------------
inline const std::string& smdlPatchSurface::GetSubdivName() const
{
	return m_SubdivName;
}

//--------------------------------------------------------------------
// Const access to tessellation system
//--------------------------------------------------------------------
inline const smdlPatchTessellator& smdlPatchSurface::GetPatchTessellator() const
{
	return m_PatchTessellator;
}

//--------------------------------------------------------------------
// Const access to character skin
//--------------------------------------------------------------------
inline const smdlCharacterSkin& smdlPatchSurface::GetCharacterSkin() const
{
	return m_CharacterSkin;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline int smdlPatchSurface::GetNumFacesAtLevel( int i_SubdivLevel ) const
{
	return GetPatchTessellator().GetNumFacesAtLevel( i_SubdivLevel );
}

