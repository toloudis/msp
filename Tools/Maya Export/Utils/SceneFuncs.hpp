/*****************************************************************************
**  SceneFuncs.hpp
**
**		Namespace for writing meshes and hierarchies to file.    
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SCENEFUNCS_HPP
#error SceneFuncs.hpp multiply included
#endif
#define SCENEFUNCS_HPP

class chWriter;
class MMatrix;
class MObject;
class MFnMesh;
class MFnTransform;
class gfFileBin;
class MaterialData;
class MaterialTable;

namespace SceneFuncs
{
	//========================================================================
	//	WriteExporterVersionStamp - write version, date, time
	//	to chunk writer
	//========================================================================
	void WriteExporterVersionStamp(chWriter &o_Writer);

	//========================================================================
	//	WriteExclusiveMatrix - write the matrix that applies to the root 
	//  node of our skeleton. Usually a global scaling.
	//========================================================================
	void WriteExclusiveMatrix(chWriter &o_Writer,
							  const MMatrix& i_Matrix);

	//========================================================================
	// write MATR chunk to file
	//========================================================================
	void WriteMaterial(MaterialData *material, chWriter &o_Writer);

	//========================================================================
	//	WriteBRepToFile - write boundary representation (mesh) to file.
	//========================================================================
	void WriteBRepToFile(MFnMesh &mesh, chWriter &o_Writer,
						 bool i_bWorldSpace = false);
			
	//========================================================================
	// Gather warning messages about flag states for this shape
	//========================================================================
	void GatherWarningMessages(MFnMesh &mesh, MString& o_Message);

	//========================================================================
	//	WriteSingleMatBRep - write mesh to file, using given material data
	//========================================================================
	void WriteSingleMatBRep(MFnMesh &mesh, MaterialData *material, chWriter &o_Writer,
						 MMatrix &matrix);

	//========================================================================
	// write transformation for this node and recurse on children
	//========================================================================
	int WriteTransform(MFnTransform &transform, 
								chWriter &o_Writer, 
								MString &o_Message);

	//========================================================================
	//write animation for this node and recurse on children
	//========================================================================
	//void WriteAnimTransform(MFnTransform &transform, 
	//						chWriter &o_Writer, 
	//						bool i_bSinglePose = false);

	//========================================================================
	//	WriteHierarchy - write hierarchy of transforms and meshes to file.
	//		Returns number of meshes written.
	//========================================================================
	int WriteHierarchy(MObject &obj, 
					   gfFileBin &file, 
					   MString &o_Message);

	//========================================================================
	//	WriteAnimHierarchy - write animation channels from scene graph 
	//========================================================================
	//void WriteAnimHierarchy(MObject &obj, gfFileBin &file);

	//========================================================================
	//	GatherSharedMaterials - gathers materials from this
	//	object or its children into shared material table, adding
	//	to values already in shared table.  Only a call to 
	//  ClearSharedMaterialTable will clear the materials.
	//========================================================================
	void GatherSharedMaterials(MObject &obj);

	//========================================================================
	//  ClearSharedMaterialTable empties shared material table
	//	in order to start new group of materials, or to 
	//	cause the materials to be written directly into the
	//	fragment chunks.
	//========================================================================
	void ClearSharedMaterialTable();

	//========================================================================
	//	WriteSharedMaterialTable - write table of shared materials to file.
	//========================================================================
	void WriteSharedMaterialTable(chWriter &o_Writer);

	//========================================================================
	// Access needed for subdiv functions, can this get organized into 
	//	MaterialUtil?
	//========================================================================
	MaterialTable& SharedMaterialTable();
}
