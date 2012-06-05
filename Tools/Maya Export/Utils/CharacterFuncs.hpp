/*****************************************************************************
**  CharacterFuncs.hpp
**
**   Namespace functions related to character output.
**	Characters consist of multiple meshes controlled by a joint
**	hierarchy and feature morph targets for emotions and phonemes.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CHARACTERFUNCS_HPP
#error CharacterFuncs.hpp multiply included
#endif
#define CHARACTERFUNCS_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <vector>

#include <maya/MIntArray.h>
#include <maya/MPointArray.h>

//class MFnSubd;
class MFnMesh;
class MObjectArray;
class chWriter;

namespace CharacterFuncs
{
	struct MorphTargetInfo
	{
		MString m_BlendShapeName;
		MString m_AliasName;
		MPointArray m_PositionVecs;
		bool m_bVecsAreDeltas;
		MIntArray m_SparseIndices;
	};

	//========================================================================
	//	DebugInputs - print out inputs that are used to 
	//	create this shape
	//========================================================================
	void DebugInputs(MFnMesh &mesh);

	//========================================================================
	//	DebugInputs - print out inputs that are used to 
	//	create this shape
	//========================================================================
	//void DebugInputs(MFnSubd &subdiv);

	//========================================================================
	// Get blend shapes (and other geometry filters) from given surface
	//========================================================================
	void GetGeometryFiltersFromInputs(MFnMesh &mesh, MObjectArray &objects);
	//void GetGeometryFiltersFromInputs(MFnSubd &subdiv, MObjectArray &objects);

	//========================================================================
	// Set boolean for whether to detect deforning animation ourselves, or to 
	// just look for Maya flag "sgpuDeformGeom"
	//========================================================================
	void SetAutoDetectDeformation(bool i_bEnable);
	bool GetAutoDetectDeformation();

	//========================================================================
	// Return true if the mesh is being influenced by a deformation that
	// we cannot handle - forces animation to baked vertex animation.
	//========================================================================
	bool IsDeformingGeometry(MFnMesh &mesh);

	//========================================================================
	// Get information about the blend shapes for this mesh.
	//========================================================================
	void GetMorphTargets(MFnMesh &mesh,
		std::vector< shared_ptr<MorphTargetInfo> >& o_MorphTargets,
		MString &o_Message);

	//========================================================================
	//	WriteMeshAndMorphTargets - gets base mesh and morph targets
	//		for this mesh by looking through inputs in the mesh's history.
	//		Writes the info for them to the writer.
	//========================================================================
	void WriteMeshAndMorphTargets(MFnMesh &mesh, 
								  chWriter &o_Writer, 
								  bool i_bWriteAsSubdiv);

	//========================================================================
	//	WriteSubdivAndMorphTargets - gets base subdiv and morph targets
	//		for this subdiv by looking through inputs in the subdiv's history.
	//		Writes the info for them to the writer.
	//========================================================================
	//void WriteSubdivAndMorphTargets(MFnSubd &subdiv, chWriter &o_Writer);

	//========================================================================
	//	WriteBlendShapeAnimation - writes blend shape animation
	//	for this subdivision.
	//  If i_bSinglePose is true, writes just position at current time.
	//========================================================================
	//void WriteBlendShapeAnimation(MFnSubd &subdiv, 
	//							  chWriter &o_Writer, 
	//							  bool i_bSinglePose);
	//========================================================================
	//========================================================================
	void WriteBlendShapeAnimation(MFnMesh &mesh, 
								  chWriter &o_Writer, 
								  double i_MinTime, 
								  double i_MaxTime,  
								  bool i_bSinglePose);
}


