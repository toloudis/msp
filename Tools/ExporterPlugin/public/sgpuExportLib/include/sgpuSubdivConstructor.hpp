/****************************************************************************\
**  sgpuSubdivConstructor.hpp
**
**      sgpuSubdivConstructor.hpp defines class for a indexed triangle mesh. A single
**	set of indices maps into vertex information which contains position,
**	normal and texture coordinates.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_SUBDIVCONSTRUCTOR_HPP
#define SGPU_SUBDIVCONSTRUCTOR_HPP

#include "sgpuExportLib.hpp"
#include "sgpuVector.hpp"
#include "sgpuMesh.hpp"
#include "sgpuSubdiv.hpp"
#include "sgpuConstructor.hpp"

//============================================================================
//============================================================================
struct sgpuMeshImpl;
class sgpuMaterial;
class sgpuNode;
class sgpuModelExportScene;
struct sgpuSubdivConstructorImpl;
class sgpuString;


//============================================================================
//============================================================================
class SGPUEXPORTLIB_API sgpuSubdivConstructor : public sgpuConstructor
{
public:
	//i_Scene : the scene for which we are constructing the subdiv
	//i_Node : the i_Node for which we are constructing the subdiv
	sgpuSubdivConstructor( sgpuModelExportScene &i_Scene, sgpuNode & i_Node, bool i_bVertexAnimation  );	
	~sgpuSubdivConstructor();	
	//i_pPositins array of  sgpuVector3-s
	//i_NumPos = number of T-s
	//This data will be copied to the internal data of the sgpuMeshConstructor
	void SetPosition( const sgpuVector3 * i_pPositions, int i_NumPos );
	//i_pUVs array of sgpuVector3-s, which contain the UV-s
	//i_NumUVs = number of UV-s
	//This data will be copied to the internal data of the sgpuMeshConstructor
	void SetUV( const sgpuVector3 *i_pUVs, int i_NumUVs );	

	//Adds a single face to the mesh that is being constructed
	//Face can be added in any order, (ie need not be in the order of the materials)
	//i_pFaceVertices * number of facevertices in the face
	//i_NVerticesInFace = number of vertices in the face
	//i_FaceMtl = material id of the face.
	//AddFace returns the number of faces if the face was succesfully added
	//else, 0
	int  AddFace( const FaceVertex *i_pFaceVertices, int i_NVerticesInFace, const sgpuMaterial &i_FaceMtl );
	//assign properties to the sgpuMesh or sgpuSubdiv that is constructed
	virtual bool GetProperty(const sgpuString &i_PropertyName, sgpuPropertyValue &o_Property );
	virtual void SetProperty(const sgpuString &i_PropertyName, const sgpuPropertyValue & i_PropertyValue);

	sgpuSubdiv Construct( const sgpuString &i_MeshName );
#if defined( SGPU_SUPPORT_1200)
	sgpuSubdiv Construct( const char *meshName );
	sgpuSubdiv Construct( const wchar_t *meshName );	
#endif
private:
	sgpuSubdivConstructor( const sgpuSubdivConstructor &other );
	sgpuSubdivConstructor &operator=( const sgpuSubdivConstructor & i_Other);
private:
	sgpuSubdivConstructorImpl *m_pImpl;
};

#endif // #ifndef SGPU_MESHCONSTRUCTOR_HPP