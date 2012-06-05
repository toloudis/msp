/****************************************************************************\
**	mdlNodeInfo.hpp
**
**		Structure representing a transformation node in a scene graph
**	of a hierarchical object.
**		By using shared_ptr here, the node info represents a tree hierarchy
**	that will delete itself and its fragment info when the root is deleted.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_NODEINFO_HPP
#error mdlNodeInfo.hpp multiply included
#endif
#define MDL_NODEINFO_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef MA_MATRIX4X4_HPP
#include "Core/Ma/maMatrix4x4.hpp"
#endif 
#ifndef MDL_PATHREFERENCE_HPP
#include "Graphics/mdl/mdlPathReference.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class mdlFragInfo;
class mdlSubdivInfo;
struct smdlCharacterSkin;
struct mdlHairInfo;


//============================================================================
//============================================================================
class mdlNodeInfo
{	
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	mdlNodeInfo();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	mdlNodeInfo(bool i_bIsJoint);

public:
	bool m_bIsJoint;
	std::string m_NodeName;
	maMatrix4x4 m_Transform;

	// Pivot information
	maVector3d m_RotatePivot;
	maVector3d m_ScalePivot;
	maVector3d m_RotatePivotTranslation;
	maVector3d m_ScalePivotTranslation;

	maVector3d m_JointOrientation;
	maVector3d m_JointScaleOrientation;
	maMatrix4x4 m_InverseBindPose;

	shared_ptr<mdlFragInfo> m_MeshInfo;
	shared_ptr<mdlSubdivInfo> m_SubdivInfo;
	shared_ptr<mdlHairInfo> m_HairInfo;
	shared_ptr<smdlCharacterSkin> m_SkinInfo;
	shared_ptr<mdlPathReference> m_InstanceInfo;

	std::vector< shared_ptr<mdlNodeInfo> > m_Children;
};



//============================================================================
//============================================================================
class  mdlNodeInfoProxy : public mdlPathReference 
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	mdlNodeInfoProxy(){}
	//I should be an iterator of class 'string'
	template< class I >
		mdlNodeInfoProxy( I b, I e):
			mdlPathReference( b, e ){}
};


