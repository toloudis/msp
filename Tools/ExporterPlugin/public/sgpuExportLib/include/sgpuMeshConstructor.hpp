/****************************************************************************\
**  sgpuMeshConstructor.hpp
**
**  sgpuMeshConstructor.hpp is a helper class for constructing an sgpuMesh
**
**	Use one sgpuConstructor for constructing a single mesh.
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_MESHCONSTRUCTOR_HPP
#define SGPU_MESHCONSTRUCTOR_HPP

#include "sgpuExportLib.hpp"
#include "sgpuVector.hpp"
#include "sgpuMesh.hpp"
#include "sgpuConstructor.hpp"

//============================================================================
//============================================================================
struct sgpuMeshImpl;
class sgpuMaterial;
class sgpuNode;
class sgpuModelExportScene;
struct sgpuMeshConstructorImpl;



//============================================================================
//============================================================================
class SGPUEXPORTLIB_API sgpuMeshConstructor : public sgpuConstructor
{
public:	
	//i_Scene = the sgpuModelExportScene for which we are constructing the sgpuMesh or sgpuSubdiv
	//i_Node = the node for shich the sgpuMesh or sgpuSubdiv is goimg to be the node content
	sgpuMeshConstructor( sgpuModelExportScene &i_Scene, sgpuNode & i_Node , bool i_bVertexAnimation );	
	~sgpuMeshConstructor();
	//set the positions  of the intended Mesh
	//i_Positions = array of sgpuVector3-s whch contain the positions
	//i_NumPos = number of T-s
	//This data will be copied to the internal data of the sgpuMeshConstructor

	void SetPosition( const sgpuVector3 * i_pPositions, int i_NumPos );
	//i_Normals array of sgpuVectpr3-s which contain the normals
	//i_NumNormals = number of normals
	//This data will be copiued tonthe internal data of sgpuMeshConstructor
	void SetNormal( const sgpuVector3 * i_pNormals, int i_NumNormals );
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
	int  AddFace( const FaceVertex *i_pFaceVertices, int i_NVerticesInFace, const sgpuMaterial & i_FaceMtl );
	//assign properties to the sgpuMesh or sgpuSubdiv that is constructed
	bool GetProperty(const sgpuString &i_PropertyName, sgpuPropertyValue &o_Property );
	void SetProperty(const sgpuString &i_PropertyName, const sgpuPropertyValue & i_PropertyValue);
	//Construct the sgpuMesh
	sgpuMesh Construct(const sgpuString &i_MeshName );
private:
	//disallowing copy constructor
	sgpuMeshConstructor( const sgpuMeshConstructor &other );
	//diallowing assignment operator
	sgpuMeshConstructor &operator=( const sgpuMeshConstructor & i_Other);
private:
	sgpuMeshConstructorImpl *m_pImpl;
	/*
	std::map< int, int> m_FaceRemap;
	std::vector< sgpuVector3 >  m_PositionTemp;
	std::vector< sgpuVector3 > m_NormalTemp;
	std::vector< sgpuVector3 > m_UVTemp;
	std::vector< sgpuConstructor::Face > m_FaceTemp;
	*/
};
//set the positions  of the intended Mesh
//T = template parameter, that stands for any point class/struct with
//	three float components which can be accessed by 0,1 and 2 subscripts.
//	Eg: struct Point3 {
//		float operator[]( int idx ) {return m_data[idx];}
//			float m_data[3] 
//		};
//	This will accept any structure as long as the components can
//	be accessed by the '[]' subscript operator
//i_Positions = array of T-s whch contain the positions
//i_NumPos = number of T-s
//This data will be copied to the internal data of the sgpuMeshConstructor
#endif // #ifndef SGPU_MESHCONSTRUCTOR_HPP