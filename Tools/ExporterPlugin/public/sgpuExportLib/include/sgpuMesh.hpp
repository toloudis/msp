/****************************************************************************\
**  sgpuMesh.hpp
**
**  sgpuMesh.hpp defines class for a indexed triangle mesh. A single
**	set of indices maps into vertex information which contains position,
**	normal and texture coordinates.
**	Please note that sgpuMesh is a handle to an internal representation
**	of the mesh.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_TRIMESH_HPP
#define SGPU_TRIMESH_HPP

#include "sgpuExportLib.hpp"
#include "sgpuVector.hpp"
#include "sgpuNodeContent.hpp"



//============================================================================
//============================================================================
struct sgpuMeshImpl;
class sgpuMaterial;
class sgpuNode;
class sgpuMeshConstructor;
class sgpuString;

//============================================================================
//============================================================================
class SGPUEXPORTLIB_API sgpuMesh : public sgpuNodeContent
{
public:
	sgpuMesh( const sgpuMesh &i_Other );
	sgpuMesh& operator= ( const sgpuMesh & i_Other );
	~sgpuMesh();
	//========================================================================
	// Set name of the polygon mesh
	//If the mesh name is made up of internationalized characters,
	// convert the name to a UTF-8 sequence and  make an std::string
	//out of the UTF-8 string
	//========================================================================

	void SetName( const sgpuString& i_Name );
	const sgpuString GetName() const;

	//========================================================================
	// A vertex in sgpuMesh is a 3 tuple of (position, normal and texture -cordinate).
	// Vertices are indexed such that there must be the same number of
	// positions, normals and texture coords. Call the SetNumVertices()
	// function first to allocate the arrays and then set individual
	// values for position, normal and texture coords.
	//========================================================================
	void SetNumVertices(int i_NumVertices);
	//i_PosIdx should be less than i_NumVertices
	void SetPosition(int i_VertIndex, float i_X, float i_Y, float i_Z);
	//i_NofrmalIdx should be less than i_NumVertices
	void SetNormal(int i_VertIndex, float i_X, float i_Y, float i_Z);
	//i_UVIdx should be less than i_NumVertices
	void SetTexCoord(int i_VertIndex, float i_U, float i_V);
	//========================================================================
	// Vertices are indexed such that there must be the same number of
	// positions, normals and texture coords. Call the GetNumVertices()
	// function first to get the number of positions or the number of normals 
	// or the number of texture coords
	//========================================================================
	int GetNumVertices() const;
	// i_PosIndex should be less than GetNumVertices()
	sgpuVector3 GetPosition( int i_PosIndex ) const;
	//i_NormalIndex should be less than GetNumVertices
	sgpuVector3 GetNormal( int i_NormalIndex ) const;
	//i_UVIndex should be les than GetNumVertices
	sgpuVector3 GetTexCoord( int i_UVIndex ) const ;
	//========================================================================
	// A vertex in sgpuMesh is a 3 tuple of (position, normal and texture -cordinate).
	// Consequently a vertex in the sgpuMesh can be different in identity from that
	// of the the position and normal and texture-coordinate in source data (eg:Maya, Max).
	// Eg: In the original source data a cube mesh may contain, 8 positions, 6 normals and 4 u-vs.
	// But when translated to sgpu-Vertices, this would result in 24 sgpu-Vertices
	// In order to preserve the identity
	// one can set the original number of vertices  in the source data and the mapping
	// from a vertex in sgpuMesh to that in the source data.
	//========================================================================
	void SetNumOriginalPositions( int i_NumOriginalPositions );
	void SetNumOriginalNormals( int i_NumOriginalNormals );
	int GetNumOriginalPositions(  ) const;
	int GetNumOriginalNormals(  ) const;	
	void SetVertexRemap( int i_VertexIdxInsgpuMesh, int i_OriginalPositionIdx, int i_OriginalNormalIdx );

	//Given a vertex index in the subdiv,
	//if the orginal remapping is present
	//return the original position index of this vertex
	//in the source mesh (eg: from Maya/Max)
	//otherwise return -1
	//i_VertIdxInMesh should be less than GetNumVertices	
	int GetOriginalPosIdx( int i_VertIdxInMesh );
	//Given a vertex index in the subdiv,
	//if the orginal remapping is present
	//return the original normal index of the normal of the vertex
	//in the source mesh (eg: from Maya/Max)
	//otherwise return -1
	//i_VertIdxInMesh should be less than GetNumVertices	
	int GetOriginalNormalIdx( int i_NormalIdx );
	//========================================================================
	//The number of triangular faces should be specified by i_NumFaces
	// A triangular face is specified by 
	// SetFaceVertex( int i_FaceIdx, i_VertexIdxInFace, i_VertexIdxInMesh)
	//Eg: If 4th (using 0 based indxing) triangular face
	// is made up of sgpuVertices (23, 24 and 2)
	//mesh.SetFaceVertex( 4, 0, 23 );
	//mesh.SetFaceVertex( 4, 1, 24);
	//mesh.SetFaceVertex( 4, 2, 2);
	//i_FaceIndex should be less than GetNumFaces()
	//i_VertexIdxInFace should be 0,1 or 2
	//i_VertexIdxInMesh should be less than GetNumVertices()
	//The faces should be sorted and ordered according to materials.
	//Specify all the faces assigned to one material,
	//then specify all the  faces assigned to next material and so on.
	//========================================================================
	void SetNumFaces( int i_NumFaces );
	void SetFaceVertex( int i_FaceIdx, int i_VertexIdxInFace, int i_VertexIdxInMesh );
	int GetNumFaces() const;
	int GetFaceVertex( int i_FaceIdx, int i_VIdx )const;
	//========================================================================
	//For meshes with multimaterials, faces shuld be sorted and ordered according
	//to the material assignment.		
	//Specify all the faces assigned to one material,
	//then specify all the  faces assigned to next material and so on.
	//SetMaterialChange specifies that from i_FaceIdx onwards i_Mtl shuld be used.
	//========================================================================
	void SetMaterialChange( int i_FaceIdx, const sgpuMaterial & i_Mtl );
	int GetNumMaterials();
	int GetMaterialChange( int i_MtlIdx, sgpuString & i_Mtl );
	//
	//two meshes are Equal if the internal pointer pointing to the
	//the actual implementations are the same
	bool operator== ( const sgpuMesh &other ) const;

	//========================================================================
	//Obsolete functions
	//========================================================================

#if defined( SGPU_SUPPORT_1200)
	//used in old versions of the sdk when a mesh could contain only a single material
	void AssignMaterial(const sgpuMaterial& i_Material);
	//replaced by SetNumFaces,
	//Please note that numFaces = numIndices/3
	void SetNumIndices(int i_NumIndices);
	//replaced by SetFaceVertex
	void SetIndex(int i_Index, int i_VertIndex);	
	//replaced by SetName
	void SetMeshName(const char* i_Name);
	void SetMeshName(const wchar_t* i_Name);

	bool IsForVertexAnimation() const;
#endif

	friend sgpuMeshConstructor;
	friend sgpuNode;
private:
	//An sgpuMesh is fathered by it containing sgpuNode
	//as sgouNode::CreateNodeContent_TriangleMesh()
	sgpuMesh();
	sgpuMeshImpl *m_pImpl;

};

#endif // #ifndef SGPU_MESH_HPP