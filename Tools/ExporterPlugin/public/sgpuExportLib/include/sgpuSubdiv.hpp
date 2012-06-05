/****************************************************************************\
**  sgpuSubdiv.hpp
**
**  sgpuSubdiv.hpp defines class for a polyginal mesh that can be subjected to
**	hardware accelerated smoothing. This class is similar to the sgpuMesh, except that
**	each face of the mesh can be of many sides.
**
**	Please note that sgpuSubdiv is the handle to an internal represntation 
**	of the subdivMesh
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_SUBDIV_HPP
#define SGPU_SUBDIV_HPP

#include "sgpuExportLib.hpp"
#include "sgpuVector.hpp"
#include "sgpuNodeContent.hpp"


//============================================================================
//============================================================================
struct sgpuSubdivImpl;
class sgpuMaterial;
class sgpuNode;
class sgpuSubdivConstructor;
class sgpuString;

//============================================================================
//============================================================================
class SGPUEXPORTLIB_API sgpuSubdiv : public sgpuNodeContent
{
public:

	sgpuSubdiv( const sgpuSubdiv &i_Other );
	sgpuSubdiv& operator= ( const sgpuSubdiv & i_Other );
	~sgpuSubdiv();
	//========================================================================
	// Set name of the polygon subdiv
	//If the subdiv name is made up of internationalized characters,
	// convert the name to a UTF-8 sequence and  make an std::string
	//out of the UTF-8 string
	//========================================================================
	void SetName(const sgpuString &i_Name);
	const sgpuString GetName() const;
	//========================================================================
	// A vertex in sgpuSubdiv is a 3 tuple of (position, normal and texture -cordinate).
	// Vertices are indexed such that there must be the same number of
	// positions, normals and texture coords. Call the SetNumVertices()
	// function first to allocate the arrays and then set individual
	// values for position, normal and texture coords.
	//========================================================================
	void SetNumVertices(int i_NumVertices);		
	//i_PosIndex should be less than i_NumVertices
	void SetPosition(int i_VertIndex, float i_X, float i_Y, float i_Z);		
	//i_UVIndex should be less than i_NumVertices
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
	//i_UVIndex should be les than GetNumVertices
	sgpuVector3 GetTexCoord( int i_UVIndex ) const ;

	//========================================================================
	// A vertex in sgpuSubdiv is a 3 tuple of (position, normal and texture -cordinate).
	// Consequently a vertex in the sgpuSubdiv can be different in identity from that
	// of the the position and normal and texture-coordinate in source data (eg:Maya, Max).
	// Eg: In the original source data a cube subdiv may contain, 
	// 8 positions, 6 normals and 4 u-vs.
	// But when translated to sgpu-Vertices, this would result in 24 sgpu-Vertices
	// In order to preserve the identity
	// one can set the original number of vertices  in the source data and the mapping
	// from a vertex in sgpuSubdiv to that in the source data.
	//========================================================================
	void SetNumOriginalPositions( int i_NumOriginalPositions );
	int GetNumOriginalPositions(  ) const;
	//Given a vertex index in the subdiv,
	//if the orginal remapping is present
	//return the original position index of this vertex
	//in the source mesh (eg: from Maya/Max)
	//otherwise return -1
	int GetOriginalPosIdx( int i_VertexIdxInSubdiv );
	void SetVertexRemap( int i_VertexIdxInSgpuSubdiv, int i_OriginalPositionIdx, int i_OriginalNormalIdx );
	//========================================================================
	//The number of polygional faces should be specified by i_NumFaces
	// A Polyginal faceVertex is specified by 
	// SetFaceVertex( int i_FaceIdx, i_VertexIdxInFace, i_VertexIdxInSubdiv)
	//Eg: If 4th (using 0 based indxing) polygonal face
	// is made up of sgpuVertices (23, 24, 2, and 5)
	//subdiv.SetFaceVertex( 4, 0, 23 );
	//subdiv.SetFaceVertex( 4, 1, 24);
	//subdiv.SetFaceVertex( 4, 2, 2);
	//subdiv.SetFaceVertex( 4, 3, 5);
	//i_FaceIndex should be less than GetNumFaces()
	//i_VertexIdxInFace can be anything, but theoretically less than GetNumFaces()
	//i_VertexIdxInSubdiv should be less than GetNumVertices()
	//The faces should be sorted and ordered according to materials.
	//Specify all the faces assigned to one material,
	//then specify all the  faces assigned to next material and so on.
	//========================================================================
	void SetNumFaces( int i_NumFaces );		
	void SetFaceVertex( int i_FaceIdx, int i_VIdx, int i_VIdxInSubdiv );
	int GetNumFaces() const;
	int GetNumVertsInFace( int i_FaceIdx) const;
	void SetNumVertsInFace( int i_FaceIdx, int i_NumVertsInFace );
	int GetFaceVertex( int i_FaceIdx, int i_VIdx )const;
	//========================================================================
	//For subdiv-s with multimaterials, faces shuld be sorted and ordered according
	//to the material assignment.		
	//Specify all the faces assigned to one material,
	//then specify all the  faces assigned to next material and so on.
	//SetMaterialChange specifies that from i_FaceIdx onwards i_Mtl shuld be used.
	//========================================================================
	void SetMaterialChange( int i_FaceIndex, const sgpuMaterial & i_Mtl );

	//two subdiv-s are Equal if the internal pointer pointing to the
	//the actual implementations are the same
	bool operator==( const sgpuSubdiv &i_Other)const;
	bool IsForVertexAnimation() const;

	friend sgpuSubdivConstructor;
	friend sgpuNode;
private:
	void SetNumIndices(int i_NumIndices);
	int GetIndexToFace( int i_FaceIndex )const;
	sgpuSubdiv();
private:
	sgpuSubdivImpl *m_pImpl;
};

#endif // #ifndef SGPU_SUBDIV_HPP