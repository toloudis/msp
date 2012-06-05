/*****************************************************************************
**  SceneFuncs.cpp
**
**		Namespace for writing meshes and hierarchies to file.      
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <SceneFuncs.hpp>
//#include <MeshUtil.hpp>
#include <AnimFuncs.hpp>
#include <MaterialUtil.hpp>
#include <MayaFlagUtil.hpp>

#include <maya/MItMeshPolygon.h>
#include <maya/MFloatMatrix.h>
#include <maya/MFloatVector.h>
#include <maya/MFnIkJoint.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnMesh.h>
#include <maya/MFnSet.h>
#include <maya/MFnSubd.h>
#include <maya/MFnTransform.h>
#include <maya/MGlobal.h>
#include <maya/MIntArray.h>
#include <maya/MMatrix.h>
#include <maya/MObjectArray.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MSelectionList.h>
#include <maya/MStatus.h>
#include <maya/MUint64Array.h>

#include "Core/ch/chDefs.hpp"
#include "Core/ch/chBinWriter.hpp"

#include <time.h>
#include <vector>

namespace
{

bool	bWriteMeshDetails = false;


//=============================================================================
//	Chunk types
//=============================================================================
const chDefs::Name c_EXPV = chDefs::MakeName('E', 'X', 'P', 'V');
const chDefs::Name c_GFRG = chDefs::MakeName('G', 'F', 'R', 'G');
const chDefs::Name c_GVER = chDefs::MakeName('G', 'V', 'E', 'R');
const chDefs::Name c_NVER = chDefs::MakeName('N', 'V', 'E', 'R');
const chDefs::Name c_TVER = chDefs::MakeName('T', 'V', 'E', 'R');
const chDefs::Name c_VCOL = chDefs::MakeName('V', 'C', 'O', 'L');
const chDefs::Name c_SWLD = chDefs::MakeName('S', 'W', 'L', 'D'); // Shadow welding triangles
const chDefs::Name c_FLAG = chDefs::MakeName('F', 'L', 'A', 'G'); // Set of boolean flags
const chDefs::Name c_MTBL = chDefs::MakeName('M', 'T', 'B', 'L');
const chDefs::Name c_MTID = chDefs::MakeName('M', 'T', 'I', 'D');
const chDefs::Name c_MNAM = chDefs::MakeName('M', 'N', 'A', 'M');
const chDefs::Name c_MATR = chDefs::MakeName('M', 'A', 'T', 'R');
const chDefs::Name c_MBAS = chDefs::MakeName('M', 'B', 'A', 'S');
const chDefs::Name c_MEFF = chDefs::MakeName('M', 'E', 'F', 'F');
const chDefs::Name c_GIND = chDefs::MakeName('G', 'I', 'N', 'D');
const chDefs::Name c_NIND = chDefs::MakeName('N', 'I', 'N', 'D');
const chDefs::Name c_TIND = chDefs::MakeName('T', 'I', 'N', 'D');
const chDefs::Name c_CIND = chDefs::MakeName('C', 'I', 'N', 'D');
//const chDefs::Name c_GLMP = chDefs::MakeName('G', 'L', 'M', 'P');
const chDefs::Name c_HLEV = chDefs::MakeName('H', 'L', 'E', 'V');
const chDefs::Name c_HJNT = chDefs::MakeName('H', 'J', 'N', 'T');
const chDefs::Name c_HTRN = chDefs::MakeName('H', 'T', 'R', 'N');
const chDefs::Name c_HJOR = chDefs::MakeName('H', 'J', 'O', 'R');
//const chDefs::Name c_HRPV = chDefs::MakeName('H', 'R', 'P', 'V');
const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
const chDefs::Name c_MHIE = chDefs::MakeName('M', 'H', 'I', 'E');
const chDefs::Name c_ANIH = chDefs::MakeName('A', 'N', 'I', 'H');
const chDefs::Name c_ANLV = chDefs::MakeName('A', 'N', 'L', 'V');
const chDefs::Name c_EXMT = chDefs::MakeName('E', 'X', 'M', 'T');



//========================================================================
// Local material table to gather materials from many meshes together

MaterialTable	l_SharedMaterialTable;

//========================================================================
//========================================================================
void write_color(chWriter &o_Writer, float color[3])
{
	o_Writer.Write(color[0]);
	o_Writer.Write(color[1]);
	o_Writer.Write(color[2]);
	o_Writer.Write(1.0f);
}

//========================================================================
//========================================================================
inline void write_index(chWriter &o_Writer, int i_Index, bool i_bNeed32Bit)
{
	if (i_bNeed32Bit)
		o_Writer.Write(envType::Int32(i_Index));
	else
		o_Writer.Write(envType::Int16(i_Index));
}
				

//========================================================================
// write number of bytes used for an index (16->2, 32->4)
//========================================================================
void write_index_size(chWriter &o_Writer, bool i_bNeed32Bit)
{
	if (i_bNeed32Bit)
		o_Writer.Write(envType::Int16(4));
	else
		o_Writer.Write(envType::Int16(2));
}

//========================================================================
// Gather material information into material table.
// indexTable will map from the Maya material index into the material
//	index in our MaterialTable. Some entries in the indexTable may be -1.
// Returns number of true materials (number of indices >= 0 in indexTable)
//========================================================================
int process_materials(MFnMesh &mesh, MaterialTable &table, 
					   MIntArray &indexTable)
{
	cout << "---process_materials---" << endl;

	MObjectArray shaders;
	MIntArray indices;
	int index, i;

	if(indexTable.length() > 0)
		indexTable.clear();

	mesh.getConnectedShaders(0, shaders, indices);  //hmmm, how to handle instances (using 0 for now)

	int num_shaders = shaders.length();
	if (bWriteMeshDetails)
	{
		cout << "num shaders: " << num_shaders << endl;
	}
	if (num_shaders == 0)
	{
		return 0;
	}

	int nPolys = mesh.numPolygons();
	int nIndices = indices.length();

	// count up polygons per shader
	MIntArray polyCounts(num_shaders, 0);
	for (i = 0; i < nPolys; i++) {
		int ind = indices[i];
		if (ind >= 0 && ind < num_shaders) {
			polyCounts[ind]++;
		}
	}

	MStatus status;
	int mat_count = 0;
	for(i = 0; i < shaders.length(); i++) {
		// if no polygons use this shader, continue
		if (polyCounts[i] > 0) 
		{
			cout << "----------------------" << endl;
			index = MaterialUtil::GetMaterialData(shaders[i], table);
			indexTable.append(index);
			mat_count++;
		}
		else
		{
			if (bWriteMeshDetails)
			{
				cout << "----------------------" << endl;
				cout << "Skipping shader, no polygons" << endl;
			}

			indexTable.append(-1);
		}

	}
	return mat_count;
}

//========================================================================
// gather material information from mesh into shared material table
//========================================================================
void gather_shared_materials(MFnMesh &mesh)
{
	MIntArray indexTable; // not going to use this index table
	process_materials(mesh, l_SharedMaterialTable, indexTable);
}

//========================================================================
// gather material information from mesh into shared material table
//========================================================================
void gather_shared_materials(MFnSubd &subdiv)
{
	// Get material info
	MObjectArray shaders;
	MUint64Array faces;
	MIntArray indices;
	subdiv.getConnectedShaders(0, shaders, faces, indices);  //hmmm, how to handle instances (using 0 for now)

	MaterialTable table;
	//cout << "num shaders: " << shaders.length() << endl;

	// Assuming single shader for subdivisions?
	MaterialUtil::GetMaterialData(shaders[0], l_SharedMaterialTable);
}

//========================================================================
// gather material information from children of transform into 
// shared material table
//========================================================================
void gather_shared_materials(MFnTransform &transform)
{
	int numChildren = transform.childCount();

	std::vector<int> visible;
	int i;
	for (i = 0; i < numChildren; i++) 
		if (MayaUtil::HasTypeAsChild(transform.child(i), MFn::kMesh)) 
			visible.push_back(i);

	if (visible.size() > 0) 
	{
		for (i = 0; i < visible.size(); i++) 
		{
			MObject obj = transform.child(visible[i]);
			MStatus status;
			MFnTransform trans_child(obj, &status);
			if (status)
			{
				gather_shared_materials(trans_child);
			}
			else
			{			
				MFnMesh mesh(obj, &status);
				if (status == MS::kSuccess) 
				{
					gather_shared_materials(mesh);
				}
			}
		}
	}

}

//========================================================================
// find shared colors in color list and create indice list
//========================================================================
void process_colors(MColorArray &io_Colors, MIntArray &o_Indices)
{
	MColorArray new_colors;

	int num_cols = io_Colors.length();
	o_Indices.setLength(num_cols);
	for (int i=0; i<num_cols; i++)
	{
		MColor col = io_Colors[i];
		int c=0;
		for (c=0; c<new_colors.length(); c++)
		{
			if (col == new_colors[c])
				break;
		}
		if (c == new_colors.length())
		{
			// Add new color
			new_colors.append(col);
		}
		o_Indices[i] = c;
	}

	// now return new array in parameter
	io_Colors = new_colors;
}

//========================================================================
// Write flags for shape
//========================================================================
void write_flags(MFnMesh &mesh, chWriter &o_Writer)
{
	// Flags
	cout << "Primary Tests" << endl;
	int doubleSided = MayaFlagUtil::GetEngineFlag(mesh, "doubleSided");
	cout << "doubleSided = " << doubleSided << endl;
	int triangleSort = MayaFlagUtil::GetEngineFlag(mesh, "triangleSort");
	cout << "triangleSort = " << triangleSort << endl;
	int castsShadows = MayaFlagUtil::GetEngineFlag(mesh, "castsShadows");
	cout << "castsShadows = " << castsShadows << endl;
	int receiveShadows = MayaFlagUtil::GetEngineFlag(mesh, "receiveShadows");
	cout << "receiveShadows = " << receiveShadows << endl;
	//int primaryVisibility = MayaUtil::GetEngineFlag(mesh, "primaryVisibility");
	//cout << "primaryVisibility = " << primaryVisibility << endl;
	int shadowHull = MayaFlagUtil::GetEngineFlag(mesh, "shadowHull");
	cout << "shadowHull = " << shadowHull << endl;
	
	int lowRes = 0; // no resolution distinction
	if (MayaFlagUtil::EngineFlagExists(mesh, "LowRes"))
	{
		int lowResState = MayaFlagUtil::GetEngineFlag(mesh, "LowRes");
		if (lowResState)
			lowRes = 1;	// low-res
		else
			lowRes = 2;	// high-res
	}
	cout << "lowRes = " << lowRes << endl;

	int cloth = MayaFlagUtil::GetEngineFlag(mesh, "Cloth");
	cout << "cloth = " << cloth << endl;

	// Now, always write the flag information...

	// Only write the flag chunk if the values are non-default
	//if (doubleSided || triangleSort || shadowHull 
	//	|| !castsShadows || !receiveShadows || lowRes || cloth)
	{
		// Note: need to increment version when adding flags
		o_Writer.WriteChunkHeader(c_FLAG, 5, false);
		// Version 2 syncs with subdivisions to have 5 flags
		o_Writer.Write(envType::Int8(doubleSided));
		o_Writer.Write(envType::Int8(triangleSort));
		o_Writer.Write(envType::Int8(castsShadows));
		o_Writer.Write(envType::Int8(receiveShadows));
		o_Writer.Write(envType::Int8(shadowHull));
		// Version 3 adds "low-resolution" flag (Version 5 adds 3-way state)
		//  0 - appear in all resolutions
		//	1 - appear only in low resolutions
		//	2 - appear only in high resolutions
		o_Writer.Write(envType::Int8(lowRes));
		// Version 4 adds "cloth" flag
		o_Writer.Write(envType::Int8(cloth));
		o_Writer.FinishChunk(); // c_FLAG
	}
}

}	// end of namespace


//========================================================================
//	WriteExporterVersionStamp - write version, date, time
//	to chunk writer
//========================================================================
void SceneFuncs::WriteExporterVersionStamp(chWriter &o_Writer)
{
	char date_string[64];
	char time_string[64];
	_strdate(date_string);
	_strtime(time_string);

	// Write as strings so readable from bin viewer
	//
	o_Writer.WriteChunkHeader(c_EXPV, 0, false);
	o_Writer.Write(MayaUtil::GetExporterVersion());
	o_Writer.Write(date_string);
	o_Writer.Write(time_string);
	o_Writer.FinishChunk();

}

//========================================================================
//	WriteExclusiveMatrix - write the matrix that applies to the root 
//  node of our skeleton. Usually a global scaling.
//========================================================================
void SceneFuncs::WriteExclusiveMatrix(chWriter &o_Writer,
									  const MMatrix& i_Matrix)
{
	o_Writer.WriteChunkHeader(c_EXMT, 0, false);
	for (int row=0; row < 4; row++)
	{
		for (int col=0; col < 4; col++)
		{
			o_Writer.Write(float(i_Matrix(row,col)));
		}
	}
	o_Writer.FinishChunk();
}

//========================================================================
// write MATR chunk to file
//========================================================================
void SceneFuncs::WriteMaterial(MaterialData *material, chWriter &o_Writer)
{
	o_Writer.WriteChunkHeader(c_MATR, 0, true);

	// new material name chunk
	o_Writer.WriteChunkHeader(c_MNAM, 0, false);
	o_Writer.Write(material->name.asUTF8());
	o_Writer.FinishChunk();

	o_Writer.WriteChunkHeader(c_MBAS, 0, false);

	float white[3] = {1, 1, 1};
	float black[3] = {0, 0, 0};

	// For now, assume white material if texture on diffuse
	//
	if (material->texture == "") 
	{
		write_color(o_Writer, material->diffuse);
		write_color(o_Writer, material->ambient);
		write_color(o_Writer, material->specular);
		write_color(o_Writer, black); //emissive
		o_Writer.Write(material->power); 
		o_Writer.Write(envType::Int8(0));
	}
	else 
	{			
		write_color(o_Writer, white); //diffuse
		write_color(o_Writer, material->ambient);
		write_color(o_Writer, material->specular);
		write_color(o_Writer, black); //emissive
		o_Writer.Write(material->power); //power

		//cout << "Material Texture name: " << material->texture << endl;

		MStringArray subs;
		MString newTextureName;
		newTextureName = material->texture;

		//	check if the texture name has back-slashes instead of forward slashes
		//	for the directory breaks and split it up with the appropriate one.
		//
		MayaUtil::ConvertSlashes(newTextureName);
		newTextureName.split('/', subs);

		//	grab the filename only
		//
		MString &textureName = subs[subs.length() - 1];
		//cout << "2-Short Texture name: " << textureName << " splits " << subs.length() << endl;

		o_Writer.Write(envType::Int8(1));
		o_Writer.Write(textureName.asUTF8());
	}
	
	o_Writer.FinishChunk();

	if (material->effect != "")
	{
		o_Writer.WriteChunkHeader(c_MEFF, 0, false);

		//cout << "Full Effect name: " << material->effect << endl;

		MStringArray subs;
		material->effect.split('/', subs);
		MString &effectName = subs[subs.length() - 1];

		//cout << "Short Effect name: " << effectName << endl;

		o_Writer.Write(effectName.asUTF8());

		o_Writer.FinishChunk();
	}

	o_Writer.FinishChunk();
}

//========================================================================
//	WriteBRepToFile - write boundary representation (mesh) to file.
//========================================================================
void SceneFuncs::WriteBRepToFile(MFnMesh &mesh, chWriter &o_Writer,
								 bool i_bWorldSpace)
{
	MStatus status;

	int i;

	bool bUsingSharedMaterials = (!l_SharedMaterialTable.IsEmpty());

	MaterialTable uniqueTable;
	MaterialTable &materialTable = (bUsingSharedMaterials) ? l_SharedMaterialTable : uniqueTable;

	// Gather materials into our material table.
	// indexTable will map from Maya material index to our material index
	//	in the materialTable, but some entires may be -1
	MIntArray indexTable;
	process_materials(mesh, materialTable, indexTable);

	if (bWriteMeshDetails)
	{
		materialTable.Print();
		cout << materialTable.entries.size();
		cout << indexTable << endl;
	}

	// Check for no materials,
	// add one in 
	//
	if (materialTable.entries.size() == 0)
	{
		if (bWriteMeshDetails)
		{
			cout << "No attached materials found, so adding grey one." << endl;
		}
		
		MaterialData *material = new MaterialData;
		materialTable.entries.push_back(material);
		material->name = "Gray";
		material->diffuse[0] = material->diffuse[1] = material->diffuse[2] = 0.5f;
		material->ambient[0] = material->ambient[1] = material->ambient[2] = 0.5f;

		indexTable.append(0);
	}
	else if (indexTable.length() == 0)
	{
		// We have a material table but no materials for this mesh.
		// So give a warning message and return without writing anything.
		//MayaUtil::DisplayError(mesh.name() + " does not have a material attached.");
		cout << "No attached materials found, skipping mesh: " << mesh.name() << endl;
		return;
	}


	int nVerts = mesh.numVertices(&status);
	int nPolys = mesh.numPolygons(&status);
	int nUVs = mesh.numUVs(&status);
	int nNormals = mesh.numNormals(&status);

	cout << "nVerts: " << nVerts << endl;
	cout << "nPolys: " << nPolys << endl;
	cout << "nUVs: " << nUVs << endl;
	cout << "nNormals: " << nNormals << endl;

	bool bNeed32Bit =  (nVerts > 0xffff || nUVs > 0xffff || nNormals > 0xffff); // MAX UINT16

	// This check doesn't make much sense right now, because the
	// nVerts is stored as "int", but I'll leave it for future use
	// if int goes to 64 bit.
	if (nVerts > 0x7fffffff || nUVs > 0x7fffffff || nNormals > 0x7fffffff) // MAX INT32
	{
		MString str;
		if (nVerts > 0x7fffffff) str = "Too many vertices (";
		else if (nNormals > 0x7fffffff) str = "Too many normals (";
		else if (nUVs > 0x7fffffff) str = "Too many uvs (";
		str += nVerts;
		str += ") in mesh ";
		str += mesh.name();
		MayaUtil::DisplayError(str);
	}


	if (nNormals == 0) {
		cout << "******************************************************" << endl;
		cout << "******************************************************" << endl;
		cout << mesh.name() << endl;
		cout << "******************************************************" << endl;
		cout << "******************************************************" << endl;
	}


	// Vertex colors
	MColorArray  vcolors;
	mesh.getFaceVertexColors ( vcolors );
	int nColors = vcolors.length();
	bool bHaveVertexColors = false;
	if (nColors > 0)
	{
		bHaveVertexColors = true;
		for (int ci=0; ci<nColors; ci++)
		{
			MColor col = vcolors[ci];
			if (col[0] < 0.0f && col[1] < 0.0f && col[2] < 0.0f)
			{
				bHaveVertexColors = false;
				break;
			}
			//cout << "  vcolor[" << ci << " = " << col << endl; 
		}
	}
	cout << "Have vertex colors : " << bHaveVertexColors << endl; 

	MIntArray color_inds;
	if (bHaveVertexColors)
	{
		process_colors(vcolors, color_inds);
		nColors = vcolors.length();
		cout << "Reindexed Colors: " << nColors << endl;

		//for (ci=0; ci<nColors; ci++)
		//	cout << "  vcolor[" << ci << "] = " << vcolors[ci] << endl; 
	}


	MIntArray triCounts;
	int totalTris = MayaUtil::GetTriCount(mesh, indexTable, triCounts);
	
	if (bWriteMeshDetails)
	{
		cout << "triCounts array: " << triCounts << endl;
	}

	if (totalTris == 0)
	{
		cout << "No polygons, so aborting." << endl;
		//file.Write(nVerts);
		return;
	}

	MObjectArray shaders;
	MIntArray indices;

	mesh.getConnectedShaders(0, shaders, indices);  //hmmm, how to handle instances (using 0 for now)

	MIntArray polyCounts;
	
	MayaUtil::GetPolyCounts(indexTable.length(), indices, mesh.numPolygons(), polyCounts);

	if (bWriteMeshDetails)
	{
		cout << "polyCounts: " << polyCounts << endl;
	}

	MIntArray polyIndices;
	MayaUtil::SortByMaterials(mesh, indices, polyCounts, polyIndices);

	if (bWriteMeshDetails)
	{
		cout << "Done sort by materials." << endl;
		cout << "polyIndices:" << endl;
		cout << polyIndices << endl;

		cout << "Num. Normals = " << nNormals << endl;
	}

	//get vertex normals in index form (the API SUCKS here)
	MItMeshPolygon polyIter(mesh.object(), &status);

	if (status != MS::kSuccess) 
	{
		cout << "could not make poly iterator!" << endl;
	}
	else if (bWriteMeshDetails) 
	{
		cout << "made iterator" << endl;
	}

	std::vector<MIntArray> normalTable(nPolys);

	for (; !polyIter.isDone(); polyIter.next()) 
	{
		int numVerts = polyIter.polygonVertexCount();
	
		if (bWriteMeshDetails)
		{
			cout << "poly " << polyIter.index() << ":";
		}

		for (int v = 0; v < numVerts; v++) 
		{
			int normalIndex = polyIter.normalIndex(v);
			normalTable[polyIter.index()].append(normalIndex);

			if (bWriteMeshDetails)
			{
				cout << " " << normalIndex;
			}
		}
		
		if (bWriteMeshDetails)
		{
			cout << endl;
		}
	}
	
#if 1
	// Geometry fragment chunk
	// Raised to version 1 when changing to 32-bit num verts and indices
	o_Writer.WriteChunkHeader(c_GFRG, 1, true);

	// Mesh name
	o_Writer.WriteChunkHeader(c_NNAM, 0, false);
	o_Writer.Write(MayaUtil::PrepareName(mesh.name()).asUTF8());
	o_Writer.FinishChunk();

	// Vertices
	o_Writer.WriteChunkHeader(c_GVER, 1, false);
	o_Writer.Write(envType::Int32(nVerts));
	for(i = 0; i < nVerts; i++) 
	{
		MPoint point;
		mesh.getPoint(i, point, (i_bWorldSpace) ? MSpace::kWorld : MSpace::kObject);
		o_Writer.Write(float(point[0]));
		o_Writer.Write(float(point[1]));
		o_Writer.Write(float(point[2]));
	}
	o_Writer.FinishChunk();

	// Normals
	o_Writer.WriteChunkHeader(c_NVER, 1, false);
	o_Writer.Write(envType::Int32(nNormals));
	MFloatVectorArray normalArray;
	mesh.getNormals(normalArray, (i_bWorldSpace) ? MSpace::kWorld : MSpace::kObject);
	for (i = 0; i < nNormals; i++) 
	{
		MFloatVector normal = normalArray[i];
		o_Writer.Write(float(normal[0]));
		o_Writer.Write(float(normal[1]));
		o_Writer.Write(float(normal[2]));
	}
	o_Writer.FinishChunk();

	// Texture coordinates
	o_Writer.WriteChunkHeader(c_TVER, 1, false);
	o_Writer.Write(envType::Int32(nUVs));
	for (i = 0; i < nUVs; i++) 
	{
		float u, v;
		mesh.getUV(i, u, v);
		o_Writer.Write(u);
		o_Writer.Write(v);
	}
	o_Writer.FinishChunk();


	if (bHaveVertexColors)
	{
		// Vertex Colors 
		o_Writer.WriteChunkHeader(c_VCOL, 1, false);
		o_Writer.Write(envType::Int32(nColors));
		for (i = 0; i < nColors; i++) 
		{
			MColor col = vcolors[i];
			o_Writer.Write(float(col[0]));
			o_Writer.Write(float(col[1]));
			o_Writer.Write(float(col[2]));
		}
		o_Writer.FinishChunk();
	}


	int numMaterials = indexTable.length();
	for( i = 0; i < numMaterials; i++) 
	{
		int matIndex = indexTable[i];
		if (matIndex >= 0)
		{
			MaterialData *material = materialTable.entries[matIndex];

			if (bUsingSharedMaterials)
			{
				o_Writer.WriteChunkHeader(c_MTID, 0, false);
				o_Writer.Write(material->name.asUTF8());
				o_Writer.FinishChunk();
			}
			else
			{
				WriteMaterial(material, o_Writer);
			}
		}
	}

	// polyIndices is a single array packed in material order
	int pi = 0;
	int pc = 0;
	for (int mi = 0; mi < numMaterials; mi++) 
	{
		// If no triangles assigned to this material, skip it.
		if (triCounts[mi] == 0)
			continue;

		int cur_pi = pi;

		o_Writer.WriteChunkHeader(c_GIND, 1, false);
		o_Writer.Write(envType::Int32(triCounts[mi]));
		// write number of bytes used for an index 
		write_index_size(o_Writer, bNeed32Bit);
		for (pc = 0; pc < polyCounts[mi]; pc++) 
		{
			int polyIndex = polyIndices[pi];
			pi++;
			MIntArray vertList;
			mesh.getPolygonVertices(polyIndex, vertList);
			
			int num = vertList.length();
			for (int v = 2; v < num; v++) 
			{
				write_index(o_Writer, vertList[0], bNeed32Bit);
				write_index(o_Writer, vertList[v-1], bNeed32Bit);
				write_index(o_Writer, vertList[v], bNeed32Bit);
			}
		}
		o_Writer.FinishChunk();

		pi = cur_pi;
		o_Writer.WriteChunkHeader(c_NIND, 1, false);
		o_Writer.Write(envType::Int32(triCounts[mi]));
		write_index_size(o_Writer, bNeed32Bit);
		for (pc = 0; pc < polyCounts[mi]; pc++) 
		{
			int polyIndex = polyIndices[pi];
			pi++;
			MIntArray vertList;
			mesh.getPolygonVertices(polyIndex, vertList);
			
			int num = vertList.length();
			for (int v = 2; v < num; v++) 
			{
				write_index(o_Writer, normalTable[polyIndex][0], bNeed32Bit);
				write_index(o_Writer, normalTable[polyIndex][v-1], bNeed32Bit);
				write_index(o_Writer, normalTable[polyIndex][v], bNeed32Bit);
			}
		}
		o_Writer.FinishChunk();

		pi = cur_pi;
		o_Writer.WriteChunkHeader(c_TIND, 1, false);
		o_Writer.Write(envType::Int16(0));	// first texture layer
		o_Writer.Write(envType::Int32(triCounts[mi]));
		write_index_size(o_Writer, bNeed32Bit);
		for (pc = 0; pc < polyCounts[mi]; pc++) 
		{
			int polyIndex = polyIndices[pi];
			pi++;
			MIntArray vertList;
			mesh.getPolygonVertices(polyIndex, vertList);
			
			int num = vertList.length();
			int ti;
			for (int v = 2; v < num; v++) 
			{
				mesh.getPolygonUVid(polyIndex, 0, ti);
				write_index(o_Writer, ti, bNeed32Bit);
				mesh.getPolygonUVid(polyIndex, v-1, ti);
				write_index(o_Writer, ti, bNeed32Bit);
				mesh.getPolygonUVid(polyIndex, v, ti);
				write_index(o_Writer, ti, bNeed32Bit);
			}
		}
		o_Writer.FinishChunk();

		// Vertex color indices
		if (bHaveVertexColors)
		{
			pi = cur_pi;
			o_Writer.WriteChunkHeader(c_CIND, 1, false);
			o_Writer.Write(envType::Int32(triCounts[mi]));
			write_index_size(o_Writer, bNeed32Bit);
			for (pc = 0; pc < polyCounts[mi]; pc++) 
			{
				int polyIndex = polyIndices[pi];
				pi++;
				MIntArray vertList;
				mesh.getPolygonVertices(polyIndex, vertList);
				
				int num = vertList.length();
				for (int v = 2; v < num; v++) 
				{
					int ci1 = 0, ci2 = 0, ci3 = 0;
					mesh.getFaceVertexColorIndex(polyIndex,0, ci1);
					mesh.getFaceVertexColorIndex(polyIndex,v-1, ci2);
					mesh.getFaceVertexColorIndex(polyIndex,v, ci3);

					//cout << "Color inds: " << ci1 << "-" << color_inds[ci1] << ",";
					//cout << ci2 << "-" << color_inds[ci2] << ",";
					//cout << ci3 << "-" << color_inds[ci3] << endl;

					write_index(o_Writer, color_inds[ci1], bNeed32Bit);
					write_index(o_Writer, color_inds[ci2], bNeed32Bit);
					write_index(o_Writer, color_inds[ci3], bNeed32Bit);
				}
			}
			o_Writer.FinishChunk();
		}
	}

	// write casts shadow, double-sided, etc. flags
	write_flags(mesh, o_Writer);

	// Debug attributes
	//cout << "ATTRIBUTES" << endl;
	//MayaUtil::PrintAttributeTypes(mesh);
	// Plugs
	//cout << "PLUGS" << endl;
	//MayaUtil::PrintPlugs(mesh);
	//cout << "**" << endl;

/* No longer doing stencil shadows 

// Note [bga] - (this would need upgrade to 32-bit if reactivated)

	// If casting shadows, then output welding zero area triangles
	// to seal up sharp edges
	//
	std::vector<MeshUtil::IndexType> weld_geom, weld_norm, weld_uv;
	if (castsShadows)
	{
		MeshUtil::FillNonWeldedEdges(mesh, weld_geom, weld_norm, weld_uv); 

		int num_welds = weld_geom.size();
		//cout << "Num Welds: " << num_welds << endl;
		//
		//for (int wi=0; wi<num_welds; wi++)
		//	cout << " Weld (" << wi << ") : " << weld_geom[wi] << ", " << weld_norm[wi] << ", " << weld_uv[wi] << endl;
	
		o_Writer.WriteChunkHeader(c_SWLD, 0, false);

		o_Writer.Write(envType::Int16(num_welds));
		o_Writer.Write(envType::Int16(3)); // 3 sets of indices = geom, norm, tex

		int wi;
		for (wi=0; wi<num_welds; wi++)
			o_Writer.Write(envType::Int16(weld_geom[wi]));
		for (wi=0; wi<num_welds; wi++)
			o_Writer.Write(envType::Int16(weld_norm[wi]));
		for (wi=0; wi<num_welds; wi++)
			o_Writer.Write(envType::Int16(weld_uv[wi]));

		o_Writer.FinishChunk(); // c_SWLD
	}
*/
	o_Writer.FinishChunk(); // c_GFRG
			
#endif

}

//========================================================================
// Gather warning messages about flag states for this shape
//========================================================================
void SceneFuncs::GatherWarningMessages(MFnMesh &mesh, MString& o_Message)
{
	// Check for efficiencies, gather a message to 
	// be reported in dialog

	//bga - Nobody cares about double-sided apparently...
	//bool doubleSided = MayaFlagUtil::GetDoubleSidedFlag(mesh);
	//if (doubleSided) 
	//	o_Message += (mesh.name() + " is marked double-sided. \\n");

	bool triangleSort = MayaFlagUtil::GetTriangleSortFlag(mesh);
	if (triangleSort) 
		o_Message += (mesh.name() + " is marked triangle-sort. \\n");

	bool shadowHull = MayaFlagUtil::GetShadowHullFlag(mesh);
	if (shadowHull) 
		o_Message += (mesh.name() + " is marked shadow hull. \\n");

	bool castsShadows = MayaFlagUtil::GetCastsShadowsFlag(mesh);
	if (!castsShadows) 
		o_Message += (mesh.name() + " is marked to NOT cast shadows. \\n");

	bool receiveShadows = MayaFlagUtil::GetReceiveShadowsFlag(mesh);
	if (!receiveShadows) 
		o_Message += (mesh.name() + " is marked to NOT receive shadows. \\n");

	int low_res = MayaFlagUtil::GetResolutionLevel(mesh);
	bool bWriteSubdiv = MayaFlagUtil::GetExportAsSubdivFlag(mesh);
	if (bWriteSubdiv && (low_res == 1))
		o_Message += (mesh.name() + " cannot be both subdivision and low resolution. \\n");

}

//========================================================================
//	WriteSingleMatBRep - write mesh to file, using given material data
//========================================================================
void SceneFuncs::WriteSingleMatBRep(MFnMesh &mesh, MaterialData *material, chWriter &o_Writer,
						 MMatrix &matrix)
{
	cout << "WriteSingleMatBRep " << endl;

	MStatus status;
	int i;

	// need index table?

	// Check for no materials?


	int nVerts = mesh.numVertices(&status);
	int nPolys = mesh.numPolygons(&status);
	int nUVs = mesh.numUVs(&status);
	int nNormals = mesh.numNormals(&status);

	cout << "nVerts: " << nVerts << endl;
	cout << "nPolys: " << nPolys << endl;
	cout << "nUVs: " << nUVs << endl;
	cout << "nNormals: " << nNormals << endl;

	if (nVerts > 0xffff || nUVs > 0xffff || nNormals > 0xffff) // MAX UINT16
	{
		MString str;
		if (nVerts > 0xffff) str = "Too many vertices (";
		else if (nNormals > 0xffff) str = "Too many normals (";
		else if (nUVs > 0xffff) str = "Too many uvs (";
		str += nVerts;
		str += ") in mesh ";
		str += mesh.name();
		MayaUtil::DisplayError(str);
		return;
	}

	// How to handle ambient shading?


	// need to get a triangle count, not just polygon count
	int totalTris = MayaUtil::GetSimpleTriCount(mesh);
	cout << "Total tris: " << totalTris << endl;

	//get vertex normals in index form (the API SUCKS here)

	std::vector<MIntArray> normalTable(nPolys);
	MIntArray face_normals ;
	for (i=0; i<nPolys; i++) 
	{
		int numVerts = mesh.polygonVertexCount(i);
		mesh.getFaceNormalIds ( i, face_normals );
	
		if (bWriteMeshDetails) cout << "poly " << i << ":";

		for (int v = 0; v < numVerts; v++) 
		{
			int normalIndex = face_normals[v];
			normalTable[i].append(normalIndex);

			if (bWriteMeshDetails)	cout << " " << normalIndex;
		}
		
		if (bWriteMeshDetails)	cout << endl;
	}
	
#if 1
	// Geometry fragment chunk
	o_Writer.WriteChunkHeader(c_GFRG, 0, true);

	// Vertices
	o_Writer.WriteChunkHeader(c_GVER, 0, false);
	o_Writer.Write(envType::Int16(nVerts));
	MPoint point;
	for(i = 0; i < nVerts; i++) 
	{
		//mesh.getPoint(i, point, (i_bWorldSpace) ? MSpace::kWorld : MSpace::kObject);
		mesh.getPoint(i, point);
		point *= matrix;
		o_Writer.Write(float(point[0]));
		o_Writer.Write(float(point[1]));
		o_Writer.Write(float(point[2]));
	}
	o_Writer.FinishChunk();

	// Normals
	o_Writer.WriteChunkHeader(c_NVER, 0, false);
	o_Writer.Write(envType::Int16(nNormals));
	MFloatVectorArray normalArray;
	mesh.getNormals(normalArray);
	MFloatVector normal;
	MFloatMatrix fmatrix(matrix.matrix);
	for (i = 0; i < nNormals; i++) 
	{
		normal = normalArray[i];
		normal *= fmatrix;
		o_Writer.Write(float(normal[0]));
		o_Writer.Write(float(normal[1]));
		o_Writer.Write(float(normal[2]));
	}
	o_Writer.FinishChunk();

	// Texture coordinates
	o_Writer.WriteChunkHeader(c_TVER, 0, false);
	o_Writer.Write(envType::Int16(nUVs));
	for (i = 0; i < nUVs; i++) 
	{
		float u, v;
		mesh.getUV(i, u, v);
		o_Writer.Write(u);
		o_Writer.Write(v);
	}
	o_Writer.FinishChunk();

	// skipping vertex colors

	// write single material info
	if (false) //(bUsingSharedMaterials)
	{
		o_Writer.WriteChunkHeader(c_MTID, 0, false);
		o_Writer.Write(material->name.asUTF8());
		o_Writer.FinishChunk();
	}
	else
	{
		WriteMaterial(material, o_Writer);
	}
	

	// polyIndices is a single array packed in material order
	int pc = 0;
	
	o_Writer.WriteChunkHeader(c_GIND, 0, false);
	o_Writer.Write(envType::Int16(totalTris));
	for (pc = 0; pc < nPolys; pc++) 
	{
		MIntArray vertList;
		mesh.getPolygonVertices(pc, vertList);
		
		int num = vertList.length();
		for (int v = 2; v < num; v++) 
		{
			o_Writer.Write(envType::Int16(vertList[0]));
			o_Writer.Write(envType::Int16(vertList[v-1]));
			o_Writer.Write(envType::Int16(vertList[v]));
		}
	}
	o_Writer.FinishChunk();

	o_Writer.WriteChunkHeader(c_NIND, 0, false);
	o_Writer.Write(envType::Int16(totalTris));
	for (pc = 0; pc < nPolys; pc++) 
	{
		MIntArray vertList;
		mesh.getPolygonVertices(pc, vertList);
		
		int num = vertList.length();
		for (int v = 2; v < num; v++) 
		{
			o_Writer.Write(envType::Int16(normalTable[pc][0]));
			o_Writer.Write(envType::Int16(normalTable[pc][v-1]));
			o_Writer.Write(envType::Int16(normalTable[pc][v]));
		}
	}
	o_Writer.FinishChunk();

	o_Writer.WriteChunkHeader(c_TIND, 0, false);
	o_Writer.Write(envType::Int16(0));	// first texture layer
	o_Writer.Write(envType::Int16(totalTris));
	for (pc = 0; pc < nPolys; pc++) 
	{
		MIntArray vertList;
		mesh.getPolygonVertices(pc, vertList);
		
		int num = vertList.length();
		int ti;
		for (int v = 2; v < num; v++) 
		{
			mesh.getPolygonUVid(pc, 0, ti);
			o_Writer.Write(envType::Int16(ti));
			mesh.getPolygonUVid(pc, v-1, ti);
			o_Writer.Write(envType::Int16(ti));
			mesh.getPolygonUVid(pc, v, ti);
			o_Writer.Write(envType::Int16(ti));
		}
	}
	o_Writer.FinishChunk();

	// Skipping Vertex color indices

	// write casts shadow, double-sided, etc. flags
	//write_flags(mesh, o_Writer);

	// no stencil shadow welding anymore

	o_Writer.FinishChunk(); // c_GFRG
			
#endif

}

			

//========================================================================
// write transformation for this node and recurse on children
//========================================================================
int SceneFuncs::WriteTransform(MFnTransform &transform, 
								chWriter &o_Writer, 
								MString &o_Message)
{
	int mesh_count = 0;

	bool bJoint = false;
	MStatus status;
	MFnIkJoint joint(transform.object(), &status);
	if (status)
		bJoint = true;

	if (bJoint)
		o_Writer.WriteChunkHeader(c_HJNT, 0, true);
	else
		o_Writer.WriteChunkHeader(c_HLEV, 0, true);

	// write node name
	cout << "Writing xform: " << transform.name() << endl;
	o_Writer.WriteChunkHeader(c_NNAM, 0, false);
	o_Writer.Write(MayaUtil::PrepareName(transform.name()).asUTF8());
	o_Writer.FinishChunk();
	 
	
	// write transform matrix
	o_Writer.WriteChunkHeader(c_HTRN, 0, false);
	MMatrix matrix = transform.transformationMatrix();
	for (int row=0; row < 4; row++)
	{
		for (int col=0; col < 4; col++)
		{
			o_Writer.Write(float(matrix(row,col)));
		}
	}
	o_Writer.FinishChunk();

	if (bJoint)
	{
		o_Writer.WriteChunkHeader(c_HJOR, 0, false);

		double	threeDoubles[3];
		MTransformationMatrix::RotationOrder rOrder = MTransformationMatrix::kXYZ;
		joint.getOrientation(threeDoubles, rOrder);

		o_Writer.Write((float) threeDoubles[0]);
		o_Writer.Write((float) threeDoubles[1]);
		o_Writer.Write((float) threeDoubles[2]);

		o_Writer.FinishChunk();
	}

	int numChildren = transform.childCount();
	cout << "number of children: " << numChildren << endl;

	std::vector<int> visible;
	int i;
	for (i = 0; i < numChildren; i++) 
	{
		if (MayaUtil::HasTypeAsChild(transform.child(i), MFn::kMesh)) 
		{
			visible.push_back(i);
		}
	}

	if (visible.size() > 0) 
	{
		cout << "  ***  " << transform.name() << " has " << visible.size() << " visible" << endl;
		for (i = 0; i < visible.size(); i++) 
		{
			cout << "   .... child " << visible[i] << endl;
			
			MObject obj = transform.child(visible[i]);
			MFnTransform trans_child(obj, &status);
			if (status)
			{
				mesh_count += WriteTransform(trans_child, o_Writer, o_Message);
			}
			else
			{			
				MFnMesh mesh(obj, &status);
				if (status == MS::kSuccess) 
				{
					SceneFuncs::WriteBRepToFile(mesh, o_Writer);
					SceneFuncs::GatherWarningMessages(mesh, o_Message);
					mesh_count++;
				}
			}
		}
	}

	
	o_Writer.FinishChunk();	// c_HLEV

	return mesh_count;
}


//========================================================================
//write animation for this node and recurse on children
//========================================================================
//void SceneFuncs::WriteAnimTransform(MFnTransform &transform, 
//									chWriter &o_Writer, 
//									bool i_bSinglePose)
//{
//	o_Writer.WriteChunkHeader(c_ANLV, 1, true);
//
//	// Node name
//	o_Writer.WriteChunkHeader(c_NNAM, 0, false);
//	o_Writer.Write(MayaUtil::PrepareName(transform.name()).asUTF8());
//	o_Writer.FinishChunk();
//
//	AnimFuncs::WriteAnimation(o_Writer, transform, i_bSinglePose);
//
//	int numChildren = transform.childCount();
//	cout << "number of children: " << numChildren << endl;
//
//	std::vector<int> visible;
//	int i;
//	for (i = 0; i < numChildren; i++) 
//	{
//		// All joints should write out, but only transforms with mesh in
//		// their subgraph shuld write out 
//		if ( transform.child(i).hasFn(MFn::kJoint) ||
//			 MayaUtil::HasTypeAsChild(transform.child(i), MFn::kMesh) )
//		{
//			visible.push_back(i);
//		}
//	}
//
//	if (visible.size() > 0) 
//	{
//		cout << "  ***  " << transform.name() << " has " << numChildren << endl;
//		for (i = 0; i < visible.size(); i++) 
//		{
//			cout << "   .... child " << visible[i] << endl;
//			
//			MStatus status;
//			MObject obj = transform.child(visible[i]);
//			MFnTransform trans_child(obj, &status);
//			if (status)
//			{
//				WriteAnimTransform(trans_child, o_Writer, i_bSinglePose);
//			}
//		}
//	}
//
//	
//	o_Writer.FinishChunk();	// c_HLEV
//}
				
//========================================================================
//	WriteHierarchy - write hierarchy of transforms and meshes to file.
//========================================================================
int SceneFuncs::WriteHierarchy(MObject &obj, 
							   gfFileBin &file, 
							   MString &o_Message)
{
	int count = 0;
	if (obj.hasFn(MFn::kTransform)) 
	{	
		MStatus status;
		MFnTransform transform(obj, &status);
		if (status)
		{
			chBinWriter writer(file);
			writer.WriteChunkHeader(c_MHIE, 0, true);

			// Put shared material table here just inside Model chunk
			if (!l_SharedMaterialTable.IsEmpty())
				WriteSharedMaterialTable(writer);

			count = WriteTransform(transform, writer, o_Message);
	
			writer.FinishChunk();
		}
	}
	return count;
}

//========================================================================
//	WriteAnimHierarchy - write animation channels from scene graph 
//========================================================================
//void SceneFuncs::WriteAnimHierarchy(MObject &obj, gfFileBin &file)
//{
//	if (obj.hasFn(MFn::kTransform)) 
//	{	
//		MStatus status;
//		MFnTransform transform(obj, &status);
//		if (status)
//		{
//			chBinWriter writer(file);
//			writer.WriteChunkHeader(c_ANIH, 0, true);
//					
//			// Write in the frame rate in small chunk at top
//			AnimFuncs::WriteCurrentFrameRate(writer);
//
//			WriteAnimTransform(transform, writer);
//	
//			writer.FinishChunk();
//		}
//	}
//}


//========================================================================
//	GatherSharedMaterials - gathers materials from this
//	object or its children into shared material table, adding
//	to values already in shared table.  Only a call to 
//  ClearSharedMaterialTable will clear the materials.
//========================================================================
void SceneFuncs::GatherSharedMaterials(MObject &obj)
{
	MStatus status;
	if (obj.hasFn(MFn::kTransform)) 
	{	
		MFnTransform transform(obj, &status);
		if (status)	
		{
			gather_shared_materials(transform);
		}
	}
	else if (obj.hasFn(MFn::kMesh)) 
	{	
		MFnMesh mesh(obj, &status);
		cout << "Gather materials, mesh: " << mesh.name() << endl;
		if (status)	gather_shared_materials(mesh);
	}
	else if (obj.hasFn(MFn::kSubdiv)) 
	{	
		MFnSubd subdiv(obj, &status);
		cout << "Gather materials, subdiv: " << subdiv.name() << endl;
		if (status)	gather_shared_materials(subdiv);
	}
}

//========================================================================
//  ClearSharedMaterialTable empties shared material table
//	in order to start new group of materials, or to 
//	cause the materials to be written directly into the
//	fragment chunks.
//========================================================================
void SceneFuncs::ClearSharedMaterialTable()
{
	l_SharedMaterialTable.Clear();
}

//========================================================================
//	WriteSharedMaterialTable - write table of shared materials to file.
//========================================================================
void SceneFuncs::WriteSharedMaterialTable(chWriter &o_Writer)
{
	int num_mats = l_SharedMaterialTable.entries.size();
	if (num_mats == 0) return; // don't write chunk if no materials

	o_Writer.WriteChunkHeader(c_MTBL, 0, true);

	for (int i=0; i<num_mats; i++)
	{
		MaterialData *material = l_SharedMaterialTable.entries[i];
		WriteMaterial(material, o_Writer);
	}

	o_Writer.FinishChunk();
}


//========================================================================
// Access needed for subdiv functions, can this get organized into 
//	MaterialUtil?
//========================================================================
MaterialTable& SceneFuncs::SharedMaterialTable()
{
	return l_SharedMaterialTable;
}