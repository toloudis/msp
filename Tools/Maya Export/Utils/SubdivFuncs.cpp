/*****************************************************************************
**  SubdivFuncs.cpp
**
**   Namespace for subdivision surface related functions. 
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <SceneFuncs.hpp>
#include <SubdivFuncs.hpp>
#include <MayaFlagUtil.hpp>
#include <JointFuncs.hpp>
#include <MaterialUtil.hpp>

#include <maya/MDoubleArray.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnSubd.h>
#include <maya/MFnSubdNames.h>
#include <maya/MIntArray.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPointArray.h>
#include <maya/MUint64Array.h>

#include "Core/ch/chDefs.hpp"
#include "Core/ch/chBinWriter.hpp"

#include <vector>
#include <math.h>

namespace
{
	const float c_fEpsilon = 0.00001f;

	struct UVCoord 
	{ 
		float u,v;
	};


	const chDefs::Name c_SUBD = chDefs::MakeName('S', 'U', 'B', 'D');
	const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
	const chDefs::Name c_GVER = chDefs::MakeName('G', 'V', 'E', 'R');
	const chDefs::Name c_GFAC = chDefs::MakeName('G', 'F', 'A', 'C');
	const chDefs::Name c_MTID = chDefs::MakeName('M', 'T', 'I', 'D');
	const chDefs::Name c_TVER = chDefs::MakeName('T', 'V', 'E', 'R');
	const chDefs::Name c_TIND = chDefs::MakeName('T', 'I', 'N', 'D');
	const chDefs::Name c_CRSV = chDefs::MakeName('C', 'R', 'S', 'V');
	const chDefs::Name c_CRSE = chDefs::MakeName('C', 'R', 'S', 'E');
	const chDefs::Name c_FLAG = chDefs::MakeName('F', 'L', 'A', 'G');


	//========================================================================
	// Write flags for shape
	//========================================================================
	void write_flags(MFnDagNode &surface, chWriter &o_Writer)
	{
			// Flags
		cout << "Flag Tests" << endl;
		int doubleSided = MayaFlagUtil::GetEngineFlag(surface, "doubleSided");
		cout << "doubleSided = " << doubleSided << endl;
		int triangleSort = MayaFlagUtil::GetEngineFlag(surface, "triangleSort");
		cout << "triangleSort = " << triangleSort << endl;
		int castsShadows = MayaFlagUtil::GetEngineFlag(surface, "castsShadows");
		cout << "castsShadows = " << castsShadows << endl;
		int receiveShadows = MayaFlagUtil::GetEngineFlag(surface, "receiveShadows");
		cout << "receiveShadows = " << receiveShadows << endl;
		int shadowHull = MayaFlagUtil::GetEngineFlag(surface, "shadowHull");
		cout << "shadowHull = " << shadowHull << endl;
		//int primaryVisibility = MayaFlagUtil::GetEngineFlag(surface, "primaryVisibility");
		//cout << "primaryVisibility = " << primaryVisibility << endl;

		// Only write the flag chunk if the values are non-default
		if (doubleSided || triangleSort || shadowHull || !castsShadows || !receiveShadows)
		{
			// Note: need to increment version when adding flags
			o_Writer.WriteChunkHeader(c_FLAG, 2, false);
			// Version 0 has 2 flags, doubleSided and triangleSort:
			o_Writer.Write(envType::Int8(doubleSided));
			o_Writer.Write(envType::Int8(triangleSort));
			// Version 1 skipped in order to get in sync with Mesh export flags
			// Version 2 adds 3 flags, castsShadows, receiveShadows and shadowHull:
			o_Writer.Write(envType::Int8(castsShadows));
			o_Writer.Write(envType::Int8(receiveShadows));
			o_Writer.Write(envType::Int8(shadowHull));
			o_Writer.FinishChunk(); // c_FLAG
		}


	}

}	// end of namespace


//========================================================================
//========================================================================
void SubdivFuncs::DebugSubdivision(MFnSubd subdiv)
{
	cout << "** DebugSubdivision, named " << subdiv.name()<< endl;

	int num_levels = subdiv.levelMaxCurrent();
	cout << "num levels: " << num_levels << endl;
	for (int l=0; l<num_levels; l++)
	{
		cout << "  vertex count: (" << l << ") " << subdiv.vertexCount(l) << endl;
	}

	MUint64Array vertexIds,  edgeIds;
	subdiv.creasesGetAll ( vertexIds, edgeIds );
	cout << "  vertexIds length: " << vertexIds.length() << endl;
	cout << "  edgeIds length: " << edgeIds.length() << endl;
	for (int i=0; i<edgeIds.length(); i++)
	{
		MUint64 edgeId =  edgeIds[i];
		cout << " base: " << MFnSubdNames::base(edgeId) 
			<< " level: " << MFnSubdNames::level(edgeId) 
			<< " first: " << MFnSubdNames::first(edgeId) 
			<< " path: " << MFnSubdNames::path(edgeId) << endl;
	}


/*	cout << "  vertex count: " << subdiv.vertexCount() << endl;

	MPointArray positions;
	SubdivFuncs::GetPositions(subdiv, 1, positions);
	cout << "  position length: " << positions.length() << endl;
	for (int i=0; i<positions.length(); i++)
	{
		cout << i << ": " << positions[i][0] << "," << positions[i][1] 
			<< "," << positions[i][2] << endl;
	}

	// Get shaders for subdiv
	MObjectArray shaders;
	MUint64Array faces;
	MIntArray indices;
	subdiv.getConnectedShaders(0, shaders, faces, indices);  //hmmm, how to handle instances (using 0 for now)

	MaterialTable table;
	cout << "num shaders: " << shaders.length() << endl;
	for(i = 0; i < shaders.length(); i++) {
		cout << "----------------------" << endl;

		int index = MaterialUtil::GetMaterialData(shaders[i], table);
		cout << "Table index: " << index << endl;
	}
	*/
}

//========================================================================
//	GetPositions - get base mesh vertices of the given subdivision
//========================================================================
MStatus SubdivFuncs::GetPositions(MFnSubd &subdiv, MPointArray &o_Positions)
{
	cout << "GetPositions, node named " << subdiv.name() << endl;
	MStatus status;

	status = subdiv.vertexBaseMeshGet( o_Positions, MSpace::kObject );
	if (status != MS::kSuccess) 
		cout << "Error getting vertex positions from subdivision" << endl;
	return status;
}

//========================================================================
//	GetPositions - get mesh vertices of the given subdivision
//	with the given subdivision depth.
//========================================================================
MStatus SubdivFuncs::GetPositions(MFnSubd &subdiv, int depth, MPointArray &o_Positions)
{
	cout << "GetPositions, node named " << subdiv.name()<< endl;
	MStatus status;

	bool uniform = true;
	int sample = 2;
	MFnMeshData holder;
	MObject holder_obj = holder.create( &status);
	if (status != MS::kSuccess) 
	{
		cout << "Error creating MeshData" << endl;
		return status;
	}

	MObject obj = subdiv.tesselate (uniform, depth, sample, holder_obj, &status) ;
	if (status == MS::kSuccess) 
	{
		MFnMesh mesh(obj, &status);
		if (status == MS::kSuccess) 
		{
			cout << "Got mesh from subdivision!" << endl;
			status = mesh.getPoints(o_Positions);
		}
	}
	return status;
}



//========================================================================
//	DebugSkinClusters - look for skin clusters in given depth's 
//		tesselation
//========================================================================
//MStatus SubdivFuncs::DebugSkinClusters(MFnSubd &subdiv, int depth)
//{
//	cout << "DebugSkinClusters, node named " << subdiv.name()<< endl;
//	MStatus status;
//
//	bool uniform = true;
//	int sample = 2;
//	MFnMeshData holder;
//	MObject holder_obj = holder.create( &status);
//	if (status != MS::kSuccess) 
//	{
//		cout << "Error creating MeshData" << endl;
//		return status;
//	}
//
//	MObject obj = subdiv.tesselate (uniform, depth, sample, holder_obj, &status) ;
//	if (status == MS::kSuccess) 
//	{
//		MFnMesh mesh(obj, &status);
//		if (status == MS::kSuccess) 
//		{
//			cout << "Got mesh from subdivision!" << endl;
//			//status = mesh.getPoints(o_Positions);
//			JointFuncs::ParseSkinClusters(mesh);
//		}
//	}
//	return status;
//}


//========================================================================
// Return number of materials in this subdivision. The Write function
//	below assumes that there is only one material per subdivision surface.
//========================================================================
int SubdivFuncs::CountMaterials(MFnSubd &subdiv)
{
	MObjectArray shaders;
	MUint64Array faces;
	MIntArray indices;
	subdiv.getConnectedShaders(0, shaders, faces, indices);  //hmmm, how to handle instances (using 0 for now)
	return shaders.length();
}


//========================================================================
//	WriteSubdivMeshToFile - write mesh of subdivision at given 
//	 depth to file.
//
//  Note: this function tesellates the subdivision into a mesh
//	at the given depth and then exports mesh info.
//========================================================================
void SubdivFuncs::WriteSubdivMeshToFile(MFnSubd &subdiv, int depth, int sample, chWriter &o_Writer, MMatrix &matrix)
{
	MStatus status;

	bool uniform = true;
	MFnMeshData holder;
	MObject holder_obj = holder.create( &status);
	if (status != MS::kSuccess) 
	{
		cout << "Error creating MeshData" << endl;
	}
	else
	{
		MObject obj = subdiv.tesselate (uniform, depth, sample, holder_obj, &status) ;
		if (status == MS::kSuccess) 
		{
			MFnMesh mesh(obj, &status);
			if (status == MS::kSuccess) 
			{
				cout << "Got mesh from subdivision!" << endl;
				//status = mesh.getPoints(o_Positions);
				//SceneFuncs::WriteBRepToFile(mesh, o_Writer);

				// Get shaders for subdiv
				MObjectArray shaders;
				MUint64Array faces;
				MIntArray indices;
				subdiv.getConnectedShaders(0, shaders, faces, indices);  //hmmm, how to handle instances (using 0 for now)

				MaterialTable table;
				cout << "num shaders: " << shaders.length() << endl;

				// Assuming single shader for subdivisions, it is checked earlier
				int index = MaterialUtil::GetMaterialData(shaders[0], table);
				if (index >= 0)
				{
					SceneFuncs::WriteSingleMatBRep(mesh, table.entries[index], o_Writer, matrix);
				}

				// Delete the mesh node we just created
				/*MDGModifier dgModifier;
				status = dgModifier.deleteNode(obj); 
				if (status == MS::kSuccess) 
				{
					status = dgModifier.doIt(); 
					if (status == MS::kSuccess) 
					{
						cout << "Deleted tesselated mesh!" << endl;
					}
				}*/
		
			}
		}
	}

}

//========================================================================
//	WriteSubdivToFile - write subdivision surface info to file.
//
//  Note: this function exports subdivision info itself, including
//	creasing info.
//========================================================================
void SubdivFuncs::WriteSubdivToFile(MFnSubd &subdiv, chWriter &o_Writer)
{
	cout << "WriteSubdivToFile " << endl;

	MStatus status;
	int i;

	MPointArray positions;
	status = subdiv.vertexBaseMeshGet( positions );
	int nVerts = positions.length();
	cout << "nVerts: " << nVerts << endl;

	if (nVerts > 0xffff) // MAX UINT16
	{
		MString str;
		if (nVerts > 0xffff) str = "Too many vertices (";
		str += nVerts;
		str += ") in subdiv ";
		str += subdiv.name();
		MayaUtil::DisplayError(str);
		return;
	}

	int nPolys = subdiv.polygonCount();
	cout << "nPolys: " << nPolys << endl;

	// Count up number of indices needed
	// (sum of the number of vertices per polygon)
	int nIndices = 0, pc = 0;
	MUint64 polyId;
	bool bHasTexCoords = false;
	for (pc = 0; pc < nPolys; pc++) 
	{ 
		polyId = MFnSubdNames::baseFaceIdFromIndex(pc);
		nIndices += subdiv.polygonVertexCount( polyId );

		// find if we have texture coordinates
		bHasTexCoords |= subdiv.polygonHasVertexUVs( polyId );
	}


	bool bUsingSharedMaterials = (!SceneFuncs::SharedMaterialTable().IsEmpty());
	cout << "Using shared Materials: " << bUsingSharedMaterials << endl;

	MaterialTable uniqueTable;
	MaterialTable &materialTable = (bUsingSharedMaterials) ? SceneFuncs::SharedMaterialTable() : uniqueTable;

	// Get material info
	MObjectArray shaders;
	MUint64Array faces;
	MIntArray indices;
	subdiv.getConnectedShaders(0, shaders, faces, indices);  //hmmm, how to handle instances (using 0 for now)

	cout << "num shaders: " << shaders.length() << endl;

	// Assuming single shader for subdivisions?
	MaterialData *material = NULL;
	int index = MaterialUtil::GetMaterialData(shaders[0], materialTable);
	if (index >= 0)
	{
		material = materialTable.entries[index];
	}
	else
	{
		cout << "No attached materials found, so adding grey one." << endl;
		
		material = new MaterialData;
		materialTable.entries.push_back(material); // necessary?
		material->name = "Gray";
		material->diffuse[0] = material->diffuse[1] = material->diffuse[2] = 0.5f;
		material->ambient[0] = material->ambient[1] = material->ambient[2] = 0.5f;
	}

	std::vector<UVCoord> tex_coords;
	std::vector<unsigned short> tex_inds;
	if (bHasTexCoords)
	{
		tex_coords.reserve(nIndices);
		tex_inds.resize(nIndices);

		// Create an array of UV data. Maya stores it per polygon vertex,
		// we need it as an array of values with indices
		cout << "Gathering tex coords, " << nIndices << endl;
		UVCoord tc;
		int ind = 0, tci = 0;
		for (pc = 0; pc < nPolys; pc++) 
		{ 
			polyId = MFnSubdNames::baseFaceIdFromIndex(pc);	
			MDoubleArray uValues, vValues;	
			subdiv.polygonGetVertexUVs( polyId, uValues, vValues );
			const int num_poly_verts = uValues.length();
			for (int pv=0; pv<num_poly_verts; pv++)
			{
				tc.u = (float) uValues[pv];
				tc.v = (float) vValues[pv];

				// find is this uv coord is already used
				const int ntc = tex_coords.size();
				for (tci=0; tci<ntc; tci++)
				{
					if ( (tex_coords[tci].u == tc.u)
						&& (tex_coords[tci].v == tc.v) )
					//if ((fabs(tex_coords[tci].u - tc.u) < c_fEpsilon)
					//	&& (fabs(tex_coords[tci].v - tc.v) < c_fEpsilon))
					{
						// have a match
						break;
					}
				}
				if (tci == ntc) // didn't find a match
				{
					tex_coords.push_back(tc);
				}
				//cout << "TC (" << tc.u << "," << tc.v << ") Index " << ind << " is " << tci << endl;
				tex_inds[ind++] = tci;
			}
		}

		cout << tex_coords.size() << " unique tex coords found." << endl;
	}

	// Get creasing information
	MUint64Array vertexCreaseIds, edgeCreaseIds;
	subdiv.creasesGetAll( vertexCreaseIds, edgeCreaseIds );
	int nVertexCreases = vertexCreaseIds.length();
	int nEdgeCreases = edgeCreaseIds.length();
	cout << "Num creases, vertex: " << nVertexCreases << " edges: " << nEdgeCreases << endl;
	// The crease functions used above return the creases up to
	// the max level currently subdivided. After that, the
	// child levels inherit the crease info from the parent.
	int maxCreaseLevel = subdiv.levelMaxCurrent();
	cout << "Max Crease Level: " << maxCreaseLevel << endl;


#if 1
	// Subdivision info chunk
	o_Writer.WriteChunkHeader(c_SUBD, 1, true);

	// Subdiv name
	o_Writer.WriteChunkHeader(c_NNAM, 0, false);
	o_Writer.Write(MayaUtil::PrepareName(subdiv.name()).asUTF8());
	o_Writer.FinishChunk();

	// Vertices
	o_Writer.WriteChunkHeader(c_GVER, 0, false);
	o_Writer.Write(envType::Int16(nVerts));
	MPoint point;
	for(i = 0; i < nVerts; i++) 
	{
		point = positions[i];
		//cout << "pt (" << i << ") " << point[0] << " " << point[1] << " " << point[2] << endl;
		o_Writer.Write(float(point[0]));
		o_Writer.Write(float(point[1]));
		o_Writer.Write(float(point[2]));
	}
	o_Writer.FinishChunk();

/*
	// Normals
	o_Writer.WriteChunkHeader(c_NVER, 0, false);
	o_Writer.Write(envType::Int16(nNormals));
	MFloatVectorArray normalArray;
	mesh.getNormals(normalArray);
	for (i = 0; i < nNormals; i++) 
	{
		MFloatVector normal = normalArray[i];
		o_Writer.Write(float(normal[0]));
		o_Writer.Write(float(normal[1]));
		o_Writer.Write(float(normal[2]));
	}
	o_Writer.FinishChunk();
*/
	
	if (bHasTexCoords)
	{
		// Texture coordinates
		int nUVs = tex_coords.size();
		o_Writer.WriteChunkHeader(c_TVER, 0, false);
		o_Writer.Write(envType::Int16(nUVs));
		for (i = 0; i < nUVs; i++) 
		{
			o_Writer.Write(tex_coords[i].u);
			o_Writer.Write(tex_coords[i].v);
		}
		o_Writer.FinishChunk();
	}

	// skipping vertex colors

	// write single material info
	if (bUsingSharedMaterials)
	{
		o_Writer.WriteChunkHeader(c_MTID, 0, false);
		o_Writer.Write(material->name.asUTF8());
		o_Writer.FinishChunk();
	}
	else
	{
		SceneFuncs::WriteMaterial(material, o_Writer);
	}
	

	// writing face info: first a number of vertices 
	// in face, then vertex indices
	o_Writer.WriteChunkHeader(c_GFAC, 0, false);
	o_Writer.Write(envType::Int16(nPolys));
	o_Writer.Write(envType::Int16(nIndices+nPolys));
	for (pc = 0; pc < nPolys; pc++) 
	{ 
		polyId = MFnSubdNames::baseFaceIdFromIndex(pc);
		
		int nPolyVerts = subdiv.polygonVertexCount( polyId );
		o_Writer.Write(envType::Int8(nPolyVerts));

		MUint64Array vertexIds;
		subdiv.polygonVertices( polyId,  vertexIds );
		for (int v=0; v<nPolyVerts; v++)
		{
			int vid = subdiv.vertexBaseIndexFromVertexId(vertexIds[v]);
			o_Writer.Write(envType::Int16(vid));
		}
	}
	o_Writer.FinishChunk();

/*
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
*/
	
	if (bHasTexCoords)
	{
		o_Writer.WriteChunkHeader(c_TIND, 0, false);
		o_Writer.Write(envType::Int16(nPolys));
		o_Writer.Write(envType::Int16(nIndices+nPolys));
		int npv = 0;
		int ind = 0;
		for (pc = 0; pc < nPolys; pc++) 
		{
			polyId = MFnSubdNames::baseFaceIdFromIndex(pc);
			npv = subdiv.polygonVertexCount( polyId );
			o_Writer.Write(envType::Int8(npv));
			for (int v = 0; v < npv; v++) 
			{
				o_Writer.Write(envType::Int16(tex_inds[ind++]));

			}
		}
		o_Writer.FinishChunk();
	}

	// Creasing information
	MUint64 creaseId = 0;
	int base, first, level, path, corner;

	if (nVertexCreases > 0)
	{
		// Write vertex creasing information
		o_Writer.WriteChunkHeader(c_CRSV, 0, false);
		o_Writer.Write(envType::Int16(maxCreaseLevel));
		const int nvi = vertexCreaseIds.length();
		o_Writer.Write(envType::Int16(nvi));
		for (i=0; i<nvi; i++)
		{
			creaseId = vertexCreaseIds[i];
			MFnSubdNames::fromMUint64( creaseId, base, first, level, path, corner );

			// Some of these items need many bits, some just a few
			o_Writer.Write(envType::Int32(base));
			o_Writer.Write(envType::Int16(first));
			o_Writer.Write(envType::Int16(level));
			o_Writer.Write(envType::Int32(path));
			o_Writer.Write(envType::Int32(corner));
		}
		o_Writer.FinishChunk();
	}

	if (nEdgeCreases > 0)
	{
		// Write edge creasing information
		o_Writer.WriteChunkHeader(c_CRSE, 0, false);
		const int nei = edgeCreaseIds.length();
		o_Writer.Write(envType::Int16(maxCreaseLevel));
		o_Writer.Write(envType::Int16(nei));
		for (i=0; i<nei; i++)
		{
			creaseId = edgeCreaseIds[i];
			MFnSubdNames::fromMUint64( creaseId, base, first, level, path, corner );

			// Some of these items need many bits, some just a few
			o_Writer.Write(envType::Int32(base));
			o_Writer.Write(envType::Int16(first));
			o_Writer.Write(envType::Int16(level));
			o_Writer.Write(envType::Int32(path));
			o_Writer.Write(envType::Int32(corner));
		}
		o_Writer.FinishChunk();
	}

	// Flags
	write_flags(subdiv, o_Writer);

	o_Writer.FinishChunk(); // c_SUBD
			
#endif

}

//========================================================================
//	WriteMeshAsSubdiv - export a mesh as the control structure
//		for a subdivision surface
//========================================================================
void SubdivFuncs::WriteMeshAsSubdiv(MFnMesh &mesh, chWriter &o_Writer,
								 bool i_bWorldSpace)
{
	MStatus status;
	int i;

	int nVerts = mesh.numVertices(&status);
	int nUVs = mesh.numUVs(&status);
	cout << "nVerts: " << nVerts << endl;
	cout << "nUVs: " << nUVs << endl;

	if (nVerts > 0xffff) // MAX UINT16
	{
		MString str;
		if (nVerts > 0xffff) str = "Too many vertices (";
		str += nVerts;
		str += ") in mesh (as subdiv) ";
		str += mesh.name();
		MayaUtil::DisplayError(str);
		return;
	}

	int nPolys = mesh.numPolygons(&status);
	cout << "nPolys: " << nPolys << endl;

	// Count up number of indices needed
	// (sum of the number of vertices per polygon)
	int nIndices = 0, pc = 0;
	for (pc = 0; pc < nPolys; pc++) 
	{ 
		nIndices += mesh.polygonVertexCount( pc );
	}

	bool bUsingSharedMaterials = (!SceneFuncs::SharedMaterialTable().IsEmpty());
	cout << "Using shared Materials: " << bUsingSharedMaterials << endl;

	MaterialTable uniqueTable;
	MaterialTable &materialTable = (bUsingSharedMaterials) ? SceneFuncs::SharedMaterialTable() : uniqueTable;

	// Get material info
	MObjectArray shaders;
	MIntArray indices;
	mesh.getConnectedShaders(0, shaders, indices);  //hmmm, how to handle instances (using 0 for now)

	cout << "num shaders: " << shaders.length() << endl;

	// Assuming single shader for subdivisions?
	MaterialData *material = NULL;
	int index = MaterialUtil::GetMaterialData(shaders[0], materialTable);
	if (index >= 0)
	{
		material = materialTable.entries[index];
	}
	else
	{
		cout << "No attached materials found, so adding grey one." << endl;
		
		material = new MaterialData;
		materialTable.entries.push_back(material); // necessary?
		material->name = "Gray";
		material->diffuse[0] = material->diffuse[1] = material->diffuse[2] = 0.5f;
		material->ambient[0] = material->ambient[1] = material->ambient[2] = 0.5f;
	}

	
#if 1
	// Geometry fragment chunk
	// Raised to version 1 when changing to 32-bit num verts and indices
	o_Writer.WriteChunkHeader(c_SUBD, 1, true);

	// Mesh name
	o_Writer.WriteChunkHeader(c_NNAM, 0, false);
	o_Writer.Write(MayaUtil::PrepareName(mesh.name()).asUTF8());
	o_Writer.FinishChunk();

	// Vertices
	o_Writer.WriteChunkHeader(c_GVER, 1, false);
	o_Writer.Write(envType::Int16(nVerts));
	for(i = 0; i < nVerts; i++) 
	{
		MPoint point;
		mesh.getPoint(i, point, (i_bWorldSpace) ? MSpace::kWorld : MSpace::kObject);
		o_Writer.Write(float(point[0]));
		o_Writer.Write(float(point[1]));
		o_Writer.Write(float(point[2]));
	}
	o_Writer.FinishChunk();

	// Texture coordinates
	o_Writer.WriteChunkHeader(c_TVER, 1, false);
	o_Writer.Write(envType::Int16(nUVs));
	for (i = 0; i < nUVs; i++) 
	{
		float u, v;
		mesh.getUV(i, u, v);
		o_Writer.Write(u);
		o_Writer.Write(v);
	}
	o_Writer.FinishChunk();

	// Single material
	if (bUsingSharedMaterials)
	{
		o_Writer.WriteChunkHeader(c_MTID, 0, false);
		o_Writer.Write(material->name.asUTF8());
		o_Writer.FinishChunk();
	}
	else
	{
		SceneFuncs::WriteMaterial(material, o_Writer);
	}

	// writing face info: first a number of vertices 
	// in face, then vertex indices
	o_Writer.WriteChunkHeader(c_GFAC, 0, false);
	o_Writer.Write(envType::Int16(nPolys));
	o_Writer.Write(envType::Int16(nIndices+nPolys));
	for (pc = 0; pc < nPolys; pc++) 
	{ 
		MIntArray vertexIds;
		mesh.getPolygonVertices( pc, vertexIds );

		int nPolyVerts = vertexIds.length();
		o_Writer.Write(envType::Int8(nPolyVerts));
		for (int v=0; v<nPolyVerts; v++)
		{
			o_Writer.Write(envType::Int16(vertexIds[v]));
		}
	}
	o_Writer.FinishChunk();

	o_Writer.WriteChunkHeader(c_TIND, 0, false);
	o_Writer.Write(envType::Int16(nPolys));
	o_Writer.Write(envType::Int16(nIndices+nPolys));
	for (pc = 0; pc < nPolys; pc++) 
	{
		MIntArray vertexIds;
		mesh.getPolygonVertices( pc, vertexIds );

		int nPolyVerts = vertexIds.length();
		o_Writer.Write(envType::Int8(nPolyVerts));
		int ti = 0;
		for (int v=0; v<nPolyVerts; v++)
		{
			mesh.getPolygonUVid(pc, v, ti);
			o_Writer.Write(envType::Int16(ti));
		}
	}
	o_Writer.FinishChunk();

	// Flags
	write_flags(mesh, o_Writer);

	o_Writer.FinishChunk(); // c_GFRG
			
#endif

}

//========================================================================
// Gather warning messages about flag states for this subdivision
//========================================================================
void SubdivFuncs::GatherWarningMessages(MFnSubd &subdiv, MString& o_Message)
{
	// Check for efficiencies, gather a message to 
	// be reported in dialog
	int doubleSided = MayaFlagUtil::GetEngineFlag(subdiv, "doubleSided");
	if (doubleSided) 
		o_Message += (subdiv.name() + " is marked double-sided. \\n");

	int triangleSort = MayaFlagUtil::GetEngineFlag(subdiv, "triangleSort");
	if (triangleSort) 
		o_Message += (subdiv.name() + " is marked triangle-sort. \\n");

	int shadowHull = MayaFlagUtil::GetEngineFlag(subdiv, "shadowHull");
	if (shadowHull) 
		o_Message += (subdiv.name() + " is marked shadow hull. \\n");

	int castsShadows = MayaFlagUtil::GetEngineFlag(subdiv, "castsShadows");
	if (!castsShadows) 
		o_Message += (subdiv.name() + " is marked to NOT cast shadows. \\n");

	int receiveShadows = MayaFlagUtil::GetEngineFlag(subdiv, "receiveShadows");
	if (!receiveShadows) 
		o_Message += (subdiv.name() + " is marked to NOT receive shadows. \\n");
}
