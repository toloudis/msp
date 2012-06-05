/*****************************************************************************
**	smdlBoneDisplay.hpp
**
**		smdlBoneDisplay inserts fragments into the hierarchy to represent
**	the joints in a character skeleton.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_BONEDISPLAY_HPP
#error smdlBoneDisplay.hpp multiply included
#endif
#define SMDL_BONEDISPLAY_HPP

#ifndef SMDL_SURFACE_HPP
#include "Graphics/smdl/private/smdlSurface.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class g3dSceneNode;


//============================================================================
//============================================================================
class smdlBoneDisplay : public smdlSurface
{
	public:
		//--------------------------------------------------------------------
		//	smdlBoneDisplay requires the root scene node of the skeleton.
		//	It will traverse the hierarchy and insert nodes to hold the
		//	bone fragments which will display the joints.
		//--------------------------------------------------------------------
		smdlBoneDisplay( g3dSceneNode *i_pNode );

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		~smdlBoneDisplay();

		//--------------------------------------------------------------------
		//	Visible - set/get whether the given surface is renderable
		//--------------------------------------------------------------------
		virtual void SetVisible(bool i_bVisible);
		virtual bool GetVisible() const;

	private:
		std::vector<g3dSceneNode*>	m_InsertedNodes;
		bool m_bVisible;
};
