/*****************************************************************************
**  SubdivFuncs.hpp
**
**   Namespace for subdivision surface related functions.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef SUBDIVFUNCS_HPP
#error SubdivFuncs.hpp multiply included
#endif
#define SUBDIVFUNCS_HPP

class MFnSubd;
class MPointArray;
class chWriter;

namespace SubdivFuncs
{
	//========================================================================
	//========================================================================
	void DebugSubdivision(MFnSubd subdiv);

	//========================================================================
	//	GetPositions - get base mesh vertices of the given subdivision
	//========================================================================
	MStatus GetPositions(MFnSubd &subdiv,  MPointArray &o_Positions);
		
	//========================================================================
	//	GetPositions - get mesh vertices of the given subdivision
	//	with the given subdivision depth.
	//========================================================================
	MStatus GetPositions(MFnSubd &subdiv, int depth, MPointArray &o_Positions);

	//========================================================================
	//	DebugSkinClusters - look for skin clusters in given depth's 
	//		tesselation
	//========================================================================
	//MStatus DebugSkinClusters(MFnSubd &subdiv, int depth);

	//========================================================================
	// Return number of materials in this subdivision. The Write function
	//	below assumes that there is only one material per subdivision surface.
	//========================================================================
	int CountMaterials(MFnSubd &subdiv);

	//========================================================================
	//	WriteSubdivMeshToFile - write mesh of subdivision at 
	//	given depth to file.
	//
	//  Note: this function tesellates the subdivision into a mesh
	//	at the given depth and then exports mesh info.
	//========================================================================
	void WriteSubdivMeshToFile(MFnSubd &subdiv, int depth, int sample, chWriter &o_Writer,
						 MMatrix &matrix);

	//========================================================================
	//	WriteSubdivToFile - write subdivision surface info to file.
	//
	//  Note: this function exports subdivision info itself, including
	//	creasing info.
	//========================================================================
	void WriteSubdivToFile(MFnSubd &subdiv, chWriter &o_Writer);

	//========================================================================
	//	WriteMeshAsSubdiv - export a mesh as the control structure
	//		for a subdivision surface
	//========================================================================
	void WriteMeshAsSubdiv(MFnMesh &mesh, chWriter &o_Writer,
								 bool i_bWorldSpace = false);

	//========================================================================
	// Gather warning messages about flag states for this subdivision
	//========================================================================
	void GatherWarningMessages(MFnSubd &subdiv, MString& o_Message);
}


