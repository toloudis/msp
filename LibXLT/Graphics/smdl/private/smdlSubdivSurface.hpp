/*****************************************************************************
**	smdlSubdivSurface.hpp
**
**		smdlSubdivSurface handles a subdivision surface that animates. 
**	It handles skinning, morph target animation and direct vertex animation.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_SUBDIVSURFACE_HPP
#error smdlSubdivSurface.hpp multiply included
#endif
#define SMDL_SUBDIVSURFACE_HPP

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
class g3dVertexBuffer;
class maMatrix4x4;
class matMaterial;
class mdlSubdivInfo;
class smdlSubdivNetwork;
struct smdlCharacterSkin;


//============================================================================
//============================================================================
class smdlSubdivSurface : public smdlEnhancedSurface
{
	public:
		//--------------------------------------------------------------------
		//	smdlSubdivSurface requires a subdivision network and the 
		//		character skin info with information about how it animates.
		//--------------------------------------------------------------------
		smdlSubdivSurface(	shared_ptr<mdlSubdivInfo> i_pSubdivInfo,
							shared_ptr<smdlSubdivNetwork> i_pSubdivNetwork,
							const std::map<const matMaterial*,matMaterial*>& i_MaterialRemapping,
							const smdlCharacterSkin& i_CharacterSkin );

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		~smdlSubdivSurface();

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
		// Access to vertices
		//--------------------------------------------------------------------
		std::vector<maPoint3d>* GetVertices();

		//--------------------------------------------------------------------
		// Access to fragment
		//--------------------------------------------------------------------
		std::vector<g3dFragment*>& GetFragments();

		//--------------------------------------------------------------------
		// GetNumFacesAtLevel
		//--------------------------------------------------------------------
		virtual int GetNumFacesAtLevel( int i_SubdivLevel ) const;

		//--------------------------------------------------------------------
		// Const access to subdiv network
		//--------------------------------------------------------------------
		inline const shared_ptr<smdlSubdivNetwork>& GetSubdivNetwork() const;

		//--------------------------------------------------------------------
		// Const access to subdiv info
		//--------------------------------------------------------------------
		inline const shared_ptr<mdlSubdivInfo>& GetSubdivInfo() const;

		//--------------------------------------------------------------------
		// Const access to character skin
		//--------------------------------------------------------------------
		inline virtual const smdlCharacterSkin& GetCharacterSkin() const;

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

	private:
		g3dSceneNode *m_pNode;
		std::string m_SubdivName;
		shared_ptr<smdlSubdivNetwork> m_pSubdivNetwork;
		shared_ptr<mdlSubdivInfo> m_pSubdivInfo;
		std::vector<maPoint3d> m_BasePositions;
		const smdlCharacterSkin& m_CharacterSkin;
		g3dVertexBuffer* m_pSharedVertexBuffer;
		std::vector<g3dFragment*> m_Fragments;
		std::vector<int> m_VertexRemapping;
		int m_NumOriginalVertices;
		std::vector<maPoint3d> m_VertCache;
};

//--------------------------------------------------------------------
// Name for subdiv, used to look for baked vertex animation
//--------------------------------------------------------------------
inline const std::string& smdlSubdivSurface::GetSubdivName() const
{
	return m_SubdivName;
}

//--------------------------------------------------------------------
// Const access to subdiv network
//--------------------------------------------------------------------
inline const shared_ptr<smdlSubdivNetwork>& smdlSubdivSurface::GetSubdivNetwork() const
{
	return m_pSubdivNetwork;
}

//--------------------------------------------------------------------
// Const access to character skin
//--------------------------------------------------------------------
inline const smdlCharacterSkin& smdlSubdivSurface::GetCharacterSkin() const
{
	return m_CharacterSkin;
}

//--------------------------------------------------------------------
// Const access to subdiv info
//--------------------------------------------------------------------
inline const shared_ptr<mdlSubdivInfo>& smdlSubdivSurface::GetSubdivInfo() const
{
	return m_pSubdivInfo;
}
