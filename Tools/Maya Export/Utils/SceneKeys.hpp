/*****************************************************************************
**  SceneKeys.hpp
**
**   Namespace for baking animations in transformation hierarchies   
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef SCENE_KEYS_HPP
#error SceneKeys.hpp multiply included
#endif
#define SCENE_KEYS_HPP

#ifndef ANIM_KEYS_HPP
#include "AnimKeys.hpp"
#endif

#include <maya/MDagPath.h>
#include <maya/MFnTransform.h>

#include <list>

class MDagPath;

namespace SceneKeys
{
//============================================================================
// Supporting data structures
//============================================================================

	struct PathKeys
	{
		AnimKeys::TransformKeys	m_TransformKeys;
		MDagPath m_DagPath;
		//MFnTransform m_Transform;
	};

	struct SurfaceVisKeys
	{
		MDagPath m_DagPath;
		//MFnTransform m_Transform;
		AnimKeys::SurfaceKeys m_Keys;
	};

//============================================================================
// Joint baking
//============================================================================

	//========================================================================
	// Gets traverses hierarchy and gathers a list of key structures
	//	in order to bake animation later.
	//========================================================================
	//void GatherHierarchy(MFnTransform &i_Transform,
	//					 std::list<PathKeys> &o_Keys);

	//========================================================================
	// Gets current state of transforms and puts the data into the
	//	list data structure to represent the state at the given
	//	current time.
	//========================================================================
	void GatherKeys(std::list<PathKeys> &io_Keys, 
					float i_CurrentTime);

	//========================================================================
	//	WriteAnimation - write animation based on the given keys
	//		that were gathered earlier.
	//========================================================================
	//void WriteAnimation(chWriter &o_Writer,
	//					 MFnTransform &i_Transform,
	//					 const std::list<PathKeys> &i_Keys,
	//					 float i_TimeOffset);
	

//============================================================================
// Surface Visibility
//============================================================================

	//========================================================================
	// Gets mesh out of DAG path to surface and checks for
	//	a visibility animation flag.
	//========================================================================
	void GatherSurfaceVisibility(MDagPath &i_DagPath,
								 std::list<SurfaceVisKeys> &o_Keys,
								 const std::string &i_Name);
	
	//========================================================================
	// Gets current state of visibility and puts the data into the
	//	list data structure to represent the state at the given
	//	current time.
	//========================================================================
	void GatherKeys(std::list<SurfaceVisKeys> &io_Keys, 
					float i_CurrentTime);

	//========================================================================
	//	WriteAnimation - write animation based on the given keys
	//		that were gathered earlier.
	//========================================================================
	void WriteAnimation(chWriter &o_Writer,
						 const std::list<SurfaceVisKeys> &i_Keys,
						 float i_TimeOffset);
}
