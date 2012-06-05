/*****************************************************************************
**	mdlSkinInfo.hpp
**
**		mdlSkinInfo defines an associations between a surface, skinning 
**	information and a place in the scene graph for where the surface 
**	should be added.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_SKININFO_HPP
#error mdlSkinInfo.hpp multiply included
#endif
#define SMDL_SKININFO_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 


//============================================================================
//============================================================================
//class g3dSceneNode;
class mdlFragInfo;
class mdlSubdivInfo;
struct smdlCharacterSkin;


//============================================================================
//============================================================================
struct mdlSkinInfo
{
	// Only one of these pointers will be non-NULL
	shared_ptr<mdlFragInfo> m_MeshInfo;
	shared_ptr<mdlSubdivInfo> m_SubdivInfo;

	// skinning information
	shared_ptr<smdlCharacterSkin> m_SkinInfo;

	// Node in scene graph where the surface should be added.
	// If this is empty, then add the surface to the character's root node.
	//g3dSceneNode*	m_pSceneNode;
	std::string m_SceneNodeName;
};
