/****************************************************************************\
**  PrimitivesTest.cpp
**
**      Example fo using teh sgpuExportLib SDK to create some
**	simple primitive shapes.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "sgpuMatrix.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMesh.hpp"
#include "sgpuNode.hpp"
#include "sgpuModelExportScene.hpp"
#include "sgpuMeshConstructor.hpp"
#include "sgpuSubdivConstructor.hpp"
#include "sgpuPathReference.hpp"
#include "sgpuException.hpp"



#include <math.h>
#include <tchar.h>
#include <iostream>

using namespace std;



const float  c_fPI = 3.14f;
const int nMtlNameBufSize = 1024;

//--------------------------------------------------------------------
// create an sgpuMesh with a cube geometry under the node passed in.
// io_Node = the sgpuNode under which the mesh is created.
// io_Scene = the scene which contains the io_Node
// i_MeshName = the name that should be given to the mesh
// returns the sgpuMesh created.
//
// 1)We use a helper class called sgpuMshConstructor to construct the cube.
// 
// 2)Three materials, 'frontBackFaceMtl', 'leftRightFaceMtl', and 'topBottomFaceMtl'
// are assigned to the 6 faces of the cube. Eg: 'leftRightFaceMtl' is assigned to
// the left and right faces
//
// 3)Please note that the cube mesh is built and assigned to the parented node
// in this routine. The user doesnt need to do anything with the sgpuMesh
// 
//
//5)Also, please note that faces are specified, without adhereing to any specific
// material order.
//
//--------------------------------------------------------------------

#define RED_MATERIAL  _T("red material")
#define BLUE_MATERIAL _T("blue material")
#define YELLOW_MATERIAL _T("yellow material")
#define TEXTURED_MATERIAL _T("textured material")

sgpuMesh create_cube(float i_Side,
							  sgpuModelExportScene& io_Scene, 
							  sgpuNode &io_Node ,  
							  const std::string &i_MeshName, 
							  sgpuMaterial &frontBackFaceMtl, 
							  sgpuMaterial &leftRightFaceMtl, 
							  sgpuMaterial &topBottomFaceMtl )
{
	float hs = i_Side * 0.5f; // half side size

	//	For our purposes,
	//	front -> +Z
	//	top -> +Y
	//	right -> +X
	bool bVertexAnimation = false;
	sgpuMeshConstructor construct( io_Scene, io_Node, bVertexAnimation );
	sgpuVector3  cubePositions[ 8 ];
	int cubeIdx = 0;
	//cube geometry contains 8 positions
	cubePositions[ cubeIdx++ ] = sgpuVector3( -hs, -hs, +hs );	//0
	cubePositions[ cubeIdx++ ] = sgpuVector3( +hs, -hs, +hs );	//1
	cubePositions[ cubeIdx++ ] = sgpuVector3( +hs, +hs, +hs );	//2
	cubePositions[ cubeIdx++ ] = sgpuVector3( -hs, +hs, +hs );	//3
	cubePositions[ cubeIdx++ ] = sgpuVector3( -hs, +hs, -hs );	//4
	cubePositions[ cubeIdx++ ] = sgpuVector3( +hs, +hs, -hs );	//5
	cubePositions[ cubeIdx++ ] = sgpuVector3( +hs, -hs, -hs );	//6
	cubePositions[ cubeIdx++ ] = sgpuVector3( -hs, -hs, -hs );	//7

	sgpuVector3 cubeNormals[6];
	int cubeNormalIdx = 0;
	cubeNormals[ cubeNormalIdx ++ ] =  sgpuVector3( 0, 0, 1.0f );	//0
	cubeNormals[ cubeNormalIdx ++ ] =  sgpuVector3( 0, 0, -1.0f );	//1
	cubeNormals[ cubeNormalIdx ++ ] =  sgpuVector3( -1.0f,0,0 );	//2
	cubeNormals[ cubeNormalIdx ++ ] =  sgpuVector3( 1.0f,0,0 );		//3
	cubeNormals[ cubeNormalIdx ++ ] =  sgpuVector3( 0, 1.0f, 0 );	//4
	cubeNormals[ cubeNormalIdx ++ ] =  sgpuVector3( 0,-1.0f,0 );	//5

	sgpuVector3 cubeUVs[4];
	int cubeUVIdx = 0;
	cubeUVs[ cubeUVIdx ++ ] =  sgpuVector3( 0, 0, 0 );	//0
	cubeUVs[ cubeUVIdx ++ ] =  sgpuVector3( 1, 0, 0 );	//1
	cubeUVs[ cubeUVIdx ++ ] =  sgpuVector3( 1, 1, 0 );	//2
	cubeUVs[ cubeUVIdx ++ ] =  sgpuVector3( 0, 1, 0 );	//3

	construct.SetPosition( cubePositions, 8);
	construct.SetNormal( cubeNormals, 6 );
	construct.SetUV( cubeUVs, 4);



	
	//assigning faces

	
	//Each face is a triangular mesh.
	//So let us starts with three faceVertices

	sgpuConstructor::FaceVertex fv[3];
	
	//Front side is made with vertices 0,1,2, and 3

	//assigning triangular faces for the front side
	//frst triangular face of front side
	fv[0].m_VertexId = 0;
	fv[0].m_NormalId = 0;
	fv[0].m_UVId = 0;
	
	fv[1].m_VertexId = 1;
	fv[1].m_NormalId = 0;
	fv[1].m_UVId = 1;

	fv[2].m_VertexId = 2;
	fv[2].m_NormalId = 0;
	fv[2].m_UVId = 2;
	construct.AddFace( fv, 3, frontBackFaceMtl);

	//second triangular face of front side
	fv[0].m_VertexId = 2;
	fv[0].m_NormalId = 0;
	fv[0].m_UVId = 2;
	
	fv[1].m_VertexId = 3;
	fv[1].m_NormalId = 0;
	fv[1].m_UVId = 3;

	fv[2].m_VertexId = 0;
	fv[2].m_NormalId = 0;
	fv[2].m_UVId = 0;
	construct.AddFace( fv, 3, frontBackFaceMtl);

	//back side is made with vertices 4,5,6, and 7
	//assigning triangular faces for the back side
	//frst triangular face of back side

	fv[0].m_VertexId = 4;
	fv[0].m_NormalId = 1;
	fv[0].m_UVId = 0;
	
	fv[1].m_VertexId = 5;
	fv[1].m_NormalId = 1;
	fv[1].m_UVId = 1;

	fv[2].m_VertexId = 6;
	fv[2].m_NormalId = 1;
	fv[2].m_UVId = 2;
	construct.AddFace( fv, 3, frontBackFaceMtl);

	//second triangular face of back side
	fv[0].m_VertexId = 6;
	fv[0].m_NormalId = 1;
	fv[0].m_UVId = 2;
	
	fv[1].m_VertexId = 7;
	fv[1].m_NormalId = 1;
	fv[1].m_UVId = 3;

	fv[2].m_VertexId = 4;
	fv[2].m_NormalId = 1;
	fv[2].m_UVId = 0;
	construct.AddFace( fv, 3, frontBackFaceMtl);

	//left side is made with vertices 0,3, 4 and 7.
	//assigning triangular faces for the left side
	//frst triangular face of left side

	fv[0].m_VertexId = 4;
	fv[0].m_NormalId = 2;
	fv[0].m_UVId = 0;
	
	fv[1].m_VertexId = 7;
	fv[1].m_NormalId = 2;
	fv[1].m_UVId = 1;

	fv[2].m_VertexId = 0;
	fv[2].m_NormalId = 2;
	fv[2].m_UVId = 2;
	construct.AddFace( fv, 3, leftRightFaceMtl);

	//second triangular face of left side
	fv[0].m_VertexId = 0;
	fv[0].m_NormalId = 2;
	fv[0].m_UVId = 2;
	
	fv[1].m_VertexId = 3;
	fv[1].m_NormalId = 2;
	fv[1].m_UVId = 3;

	fv[2].m_VertexId = 4;
	fv[2].m_NormalId = 2;
	fv[2].m_UVId = 0;
	construct.AddFace( fv, 3, leftRightFaceMtl);

	//top side is made with vertices 2,3, 4 and 5.
	//assigning triangular faces for the top side
	//frst triangular face of top side

	fv[0].m_VertexId = 4;
	fv[0].m_NormalId = 4;
	fv[0].m_UVId = 0;
	
	fv[1].m_VertexId = 3;
	fv[1].m_NormalId = 4;
	fv[1].m_UVId = 1;

	fv[2].m_VertexId = 2;
	fv[2].m_NormalId = 4;
	fv[2].m_UVId = 2;
	construct.AddFace( fv, 3, topBottomFaceMtl);

	//second triangular face of top side
	fv[0].m_VertexId = 2;
	fv[0].m_NormalId = 4;
	fv[0].m_UVId = 2;
	
	fv[1].m_VertexId = 5;
	fv[1].m_NormalId = 4;
	fv[1].m_UVId = 3;

	fv[2].m_VertexId = 4;
	fv[2].m_NormalId = 4;
	fv[2].m_UVId = 0;
	construct.AddFace( fv, 3, topBottomFaceMtl);


	//bottom side is made with vertices 0,7,6,1
	//assigning triangular faces for the bottom side
	//frst triangular face of bottom side
	fv[0].m_VertexId = 0;
	fv[0].m_NormalId = 5;
	fv[0].m_UVId = 0;
	
	fv[1].m_VertexId = 7;
	fv[1].m_NormalId = 5;
	fv[1].m_UVId = 1;

	fv[2].m_VertexId = 6;
	fv[2].m_NormalId = 5;
	fv[2].m_UVId = 2;
	construct.AddFace( fv, 3, topBottomFaceMtl);

	//second triangular face of bottom side
	fv[0].m_VertexId = 6;
	fv[0].m_NormalId = 5;
	fv[0].m_UVId = 2;
	
	fv[1].m_VertexId = 1;
	fv[1].m_NormalId = 5;
	fv[1].m_UVId = 3;

	fv[2].m_VertexId = 0;
	fv[2].m_NormalId = 5;
	fv[2].m_UVId = 0;
	construct.AddFace( fv, 3, topBottomFaceMtl);

	//right side is made with vertices 2,3,4,5
	//assigning triangular faces for the bottom side
	//frst triangular face of right side
	fv[0].m_VertexId = 2;
	fv[0].m_NormalId = 3;
	fv[0].m_UVId = 0;
	
	fv[1].m_VertexId = 1;
	fv[1].m_NormalId = 3;
	fv[1].m_UVId = 1;

	fv[2].m_VertexId = 6;
	fv[2].m_NormalId = 3;
	fv[2].m_UVId = 2;
	construct.AddFace( fv, 3, leftRightFaceMtl);

	//second triangular face of right side
	fv[0].m_VertexId = 6;
	fv[0].m_NormalId = 3;
	fv[0].m_UVId = 2;
	
	fv[1].m_VertexId = 5;
	fv[1].m_NormalId = 3;
	fv[1].m_UVId = 3;

	fv[2].m_VertexId = 2;
	fv[2].m_NormalId = 3;
	fv[2].m_UVId = 0;
	construct.AddFace( fv, 3, leftRightFaceMtl);
	return construct.Construct( sgpuString( i_MeshName.c_str() ) );
}

int _tmain(int argc, _TCHAR* argv[])
{
	sgpuModelExportScene scene;
	sgpuNode root_node = scene.GetRootNode();
	std::string rname("rootNode");
	root_node.SetName( sgpuString( rname.c_str() ));
	
	//Let us create all the materials that we are going to
	//use in the scene
	sgpuMaterial blueMtl = scene.CreateMaterial( sgpuString(BLUE_MATERIAL) );
	blueMtl.SetDiffuseColor(0.48f, 0.73f, 0.77f );
	blueMtl.SetAmbientColor(0.2f, 0.2f, 0.2f );
	blueMtl.SetShininess( 0.1f );

	sgpuMaterial redMtl = scene.CreateMaterial( sgpuString(RED_MATERIAL) );
	redMtl.SetDiffuseColor(0.98f, 0.53f, 0.64f );
	redMtl.SetAmbientColor(0.2f, 0.2f, 0.2f );
	redMtl.SetShininess( 0.1f );

	sgpuMaterial yellowMtl = scene.CreateMaterial( sgpuString(YELLOW_MATERIAL) );
	yellowMtl.SetDiffuseColor(0.92f, 0.98f, 0.53f );
	yellowMtl.SetAmbientColor(0.2f, 0.2f, 0.2f );
	yellowMtl.SetShininess( 0.1f );
	
	//This is an example of a material using a texture map
	sgpuMaterial texturedMtl = scene.CreateMaterial( sgpuString(TEXTURED_MATERIAL) );	
	// Relative path from .gxb location. Relative paths
	// need to begin with ".". An absolute path would also work.
	sgpuString textureName(_T(".\\Textures\\Leather_Seat_Center.dds") );
	texturedMtl.SetDiffuseTexture( textureName );
	texturedMtl.SetDiffuseColor(0.92f, 0.98f, 0.53f );
	texturedMtl.SetAmbientColor(0.2f, 0.2f, 0.2f );
	texturedMtl.SetShininess( 0.1f );

	//Our intended scene is a simple scene with 4 cubes.
	//						rootNode
	//					   / ,/  \  \
	//					  /  /    \  \,
	//					 /  /      \   \
	//				cube1 cube2   cube3 cube4
	// cube2 and cube3 are instances of cube1
	// cube4 is an example of a textured cube
	
	sgpuNode cube1 = root_node.AddChildNode();
	std::string cname("cube1");
	cube1.SetName( sgpuString( cname.c_str() ) );
	//cube1 has a redMtl for its front and back faces,
	//a blue Mtl for its left and right faces and a yellow mtl
	//for its top and bottom faces
	sgpuMesh  cube_mesh = create_cube( 5.0f, scene, cube1, "Cube1_Shape", redMtl, blueMtl, yellowMtl ); 
	sgpuMatrix cube1Trans, cube1Rot;
	//give some transormation to cube1
	cube1Trans.MakeTranslate(0,8.0f,0);
	cube1Rot.MakeRotate(c_fPI/4, 1,1,0);
	cube1.SetTransformationMatrix( cube1Rot * cube1Trans );
	
	//cube2
	sgpuNode cube2 = root_node.AddChildNode();
	std::string cname2("cube2");
	cube2.SetName( sgpuString( cname2.c_str() ) );
	//cube2's node content is an instamnce reference to cube1's mesh
	sgpuPathReference cube2_ref = cube2.CreateNodeContent_PathReference( root_node, cube1 );
	//give cube2, some transformation.
	//Please note, due to the instance refrence, cube2 gets
	//all the geometry and materials that cube1 has ,
	//but has a separate transformation.
	sgpuMatrix cube2Trans, cube2Rot;
	cube2Trans.MakeTranslate( 8.0f, 0, 0);
	cube2Rot.MakeRotate( c_fPI/2, 1, -1, 0);
	cube2.SetTransformationMatrix( cube2Rot * cube2Trans );

	//cube3 is also an instance of cube1
	sgpuNode cube3 = root_node.AddChildNode();
	std::string cname3("cube3");
	cube3.SetName( sgpuString( cname3.c_str() ) );
	sgpuPathReference cube3_ref = cube3.CreateNodeContent_PathReference( root_node, cube1 );
	sgpuMatrix cube3Trans, cube3Rot;
	cube3Trans.MakeTranslate( -8.0f, 0, 0);
	cube3Rot.MakeRotate( c_fPI/2, 1, 1, 0);
	cube3.SetTransformationMatrix( cube3Rot * cube3Trans );

	//cube4 has its own nodeContent mesh.
	//It uses a textured material
	sgpuNode cube4 = root_node.AddChildNode();
	std::string cname4("cube4");
	cube4.SetName( sgpuString( cname4.c_str() ) );
	sgpuMesh  cube4_mesh = create_cube( 5.0f, scene, cube4, "Cube4_Shape", texturedMtl, texturedMtl, texturedMtl ); 
	sgpuMatrix cube4Trans, cube4Rot;
	cube4Trans.MakeTranslate( 0, 0, 8);
	cube4Rot.MakeRotate( c_fPI/4, 0, 1, 0);
	cube4.SetTransformationMatrix( cube4Rot * cube4Trans );

	int numMeshesMerged = 0;
	int numMergedResults = 0;
	sgpuString desc;
	scene.Describe( desc );
	int nBytes = desc.GetNumBytes_UTF8();
	if( nBytes > 0)
	{
		char *pzBuf = new  char [ nBytes ];
		int nRet = desc.GetData_UTF8( pzBuf, nBytes );
		assert( nRet >0 && nRet <= nBytes );
		std::cout << pzBuf;
		delete [] pzBuf;
	}

	sgpuString prefix("PrimitivesTest");
	//merge all the meshes in the scene based on materials.
	scene.MergeByMaterials( prefix, numMeshesMerged, numMergedResults );
	if (scene.WriteScene(sgpuString("..\\data\\shape_test.gxb"), 
						 sgpuString("Primitives Test v1.0")))
		std::cout << "File written successfully." << std::endl;
	else
		std::cerr << "Could not write file." << std::endl;

	return 0;
}

