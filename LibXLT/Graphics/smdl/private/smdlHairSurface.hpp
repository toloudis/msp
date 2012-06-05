/*****************************************************************************
**	smdlHairSurface.hpp
**
**		smdlHairSurface handles a set of hair strands within a model
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_HAIRSURFACE_HPP
#error smdlHairSurface.hpp multiply included
#endif
#define SMDL_HAIRSURFACE_HPP

#ifndef SMDL_SURFACE_HPP
#include "Graphics/smdl/private/smdlSurface.hpp"
#endif


//============================================================================
//============================================================================
class g3dFragment;
class g3dSceneNode;
class matMaterial;
struct mdlHairInfo;


//============================================================================
//============================================================================
class smdlHairSurface  : public smdlSurface
{
	public:
		//--------------------------------------------------------------------
		//	smdlHairSurface requires a hair info structure. It creates a
		//	hair fragment for it and assumes ownership of the fragment. 
		//--------------------------------------------------------------------
		smdlHairSurface( const mdlHairInfo& i_HairInfo,
						 matMaterial *i_pMaterial);

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~smdlHairSurface();

		//--------------------------------------------------------------------
		// Return node that contains the fragments in this model in a small
		//	sub-scene graph
		//--------------------------------------------------------------------
		g3dSceneNode* RootNode();

		//--------------------------------------------------------------------
		// Name for hair surface, used to look for baked vertex animation
		//--------------------------------------------------------------------
		inline const std::string& GetHairName() const;

		//--------------------------------------------------------------------
		//	Visible - set/get whether the given surface is renderable
		//--------------------------------------------------------------------
		virtual void SetVisible(bool i_bVisible);
		virtual bool GetVisible() const;

		//--------------------------------------------------------------------
		// CheckAnimation checks compatibility of this animation 
		// with the model. It will return true if the animation can
		// be played on this model.
		//--------------------------------------------------------------------
		bool CheckAnimation( const smdlVertexAnimKeys &i_VertKeys ) const;

		//--------------------------------------------------------------------
		//	VertexTransformHair - use baked vertex animation to transform
		//		the strands in this group.
		//--------------------------------------------------------------------
		void VertexTransformHair(float i_CurrentFrame,
								 const smdlVertexAnimKeys &i_VertKeys);

		//--------------------------------------------------------------------
		// Animation data is giving us the bounding box for the surface 
		//	before the actual animation is done. Set this bounding box 
		//	into the fragment.
		//--------------------------------------------------------------------
		void BBoxTransform(const maAxisBox &i_BBox);

	private:
		std::string		m_HairName;
		g3dSceneNode*	m_pNode;
		g3dFragment*	m_pFragment;
		matMaterial*	m_pInternalHairShader;	
		int				m_NumOriginalVertices;
		std::vector<maPoint3d> m_VertCache;
};

//--------------------------------------------------------------------
// Name for hair surface, used to look for baked vertex animation
//--------------------------------------------------------------------
inline const std::string& smdlHairSurface::GetHairName() const
{
	return m_HairName;
}
