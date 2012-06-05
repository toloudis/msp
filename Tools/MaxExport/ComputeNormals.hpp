/*****************************************************************************
**  ComputeNormals.hpp
**
**	Contains classes and functions that contain the logic of 
**	how to compute the normal for the mesh
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MAXEXP_COMPUTENORMALS_HPP
#error MAXEXP_COMPUTENORMALS_HPP multuply defined!!
#endif
#define MAXEXP_COMPUTENORMALS_HPP

#ifndef MAXEXP_MAXEXPORTERUTILS_HPP
#include "MaxExportUtils.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include "MaxCommon.hpp"
#include <string>
#include <map>
#include <vector>
#include <exception>


//forward declarations
class INode;
class Matrix3;
class Animatable;
class fsXMLWriter;
class fsLocator;
class itString;
class mdlNodeInfo;
class mdlFragInfo;
class mdlSubdivInfo;
class vtxVertexAnimKeysStatic;
class vtxVertexFramesStatic ;
struct vtxVertexFrame;
namespace MaxExp
{
	class ExportDoc;
	class ExportIntent;	
	class PolyOrTriMesh;
}

namespace MaxExp
{

	//=============================================================================
	//Base class of all the normal copmputing classes
	//=============================================================================
	class ComputeNormalsBase
	{
	public:
		ComputeNormalsBase( PolyOrTriMesh & i_Parent ):
		  m_Parent( i_Parent )
		  {}

		  virtual ~ComputeNormalsBase(){}

		  //get the number of normals
		  virtual int GetNNormals()=0;

		  //get the normal gven the face corresponding and the index of the vertex in the face
		  //return the index of the normal computed
		  virtual int  GetNormal( int i_FaceIdx, int i_VertIdxInFace, Point3 &o_Normal )=0;

		  //get i-th normal acording to the normal computation
		  //This could be i'th user specified normal
		  //or the ith RNNormal or the i-th face normal
		  virtual Point3 GetIthNormal( int i_NormalIdx )=0;


		  //static utility functions

		  //returns true of the user specifoed normals are active
		  static bool CheckUserSpecifiedNormals( PolyOrTriMesh &i_POrT );

		  //delegates to mesh::checkNormals( TRUE )
		  static void CheckNormals( PolyOrTriMesh &i_POrT  );

		  //If the mesh has RVertices/RNormals built
		  //then for each face, for each corner of the face
		  //does the corresponding RVertex has an RNormal whose
		  //smoothing group matches the smoothing group 
		  //of the face
		  static bool HaveConsistentRVertices( ::Mesh & i_Mesh );

		  //factory method for creating the appropriate derived class of ComputeNormalBase-s
		  static ComputeNormalsBase* MakeComputeNormals( PolyOrTriMesh & i_POrT );

		  //reference to parent poly or tri mesh
		  PolyOrTriMesh &m_Parent;
	};

	//=============================================================================
	//This derived class is useful only if the under lying geometry
	//has user specified normals, unlike self generated RVertices/RNormals
	//which are based on smoothing group info
	//=============================================================================

	class ComputeUserSpecifiedNormals : public ComputeNormalsBase
	{
	public:
		ComputeUserSpecifiedNormals( PolyOrTriMesh & i_Parent ):
		  ComputeNormalsBase( i_Parent ){}
		  ~ComputeUserSpecifiedNormals(){}	
		  //get the number of normals
		  int GetNNormals();

		  //get the normal gven the face corresponding and the index of the vertex in the face
		  //return the index of the normal computed
		  int  GetNormal( int i_FaceIdx, int i_VertIdxInFace, Point3 &o_Normal );

		  //get i-th normal acording to the normal computation
		  Point3 GetIthNormal( int i_NormalIdx );
	};



	//=============================================================================
	//This is the most commonly used method.
	//If you call mesh.buildNormals(), 
	//the mesh build its normal information in the form of RVertex/RNormal
	//structure based on the smoothing group info of the faces of the mesh.

	//Caution: This is currently supported only by 'mesh'-es and not 'MNMesh'-es
	//=============================================================================


	class ComputeRNormals : public ComputeNormalsBase
	{
	public:
		ComputeRNormals( PolyOrTriMesh & i_Parent );
		~ComputeRNormals();

		//get the number of normals
		int GetNNormals();
		//get the normal gven the face corresponding and the index of the vertex in the face
		//return the index of the normal computed
		int  GetNormal( int i_FaceIdx, int i_VertIdxInFace, Point3 &o_Normal );

		//get i-th normal acording to the normal computation
		Point3 GetIthNormal( int i_NormalIdx );
	private:
		//Get the number of RNormals generated
		int GetNNormals( RVertex *i_pRVert );
		//Compute the number of RNormals generated
		//This could be an expensive operation
		//and should be done only once per frame
		int ComputeNNormals();
		//Initialize the class
		void Init( );
		//For deteremining the index of a normal corresponding
		//to a face corner, we need to keep track of the
		//total number of RNormals which comes before this RVertex.

		//How many normals are there in all RVertices upto the i'th RVertex,
		//(not incuding the i'th RVertex)
		//The TVertNormalsOIndexOffsetMap is a map that is initialized
		//by the constructor and it helps in determinibg the required offset.
		int GetRVertNormalsIndexOffset( int i_RVertIdxInMesh );
		typedef std::map< int, int> TRVertNormalsIndexOffsetMap;
		TRVertNormalsIndexOffsetMap m_RVertNormalsIndexOffsetMap;

		//One can get the RNormals from the mesh
		//But getting the i'th normal is ineffecient.
		//It is often better to save a cache-copy of the RNormals
		//here
		std::vector< Point3 >	m_Normals;
		//The number of RNormals
		int m_nNormals;
	};


	//=============================================================================
	//This is a fallback method for determining the normals based on the face.
	//=============================================================================
	class ComputeFaceNormals: public ComputeNormalsBase
	{
	public:
		ComputeFaceNormals( PolyOrTriMesh & i_Parent ):
		  ComputeNormalsBase( i_Parent ){}
		  ~ComputeFaceNormals(){}

		  //get the number of normals
		  int GetNNormals();

		  //get the normal gven the face corresponding and the index of the vertex in the face
		  //return the index of the normal computed
		  int  GetNormal( int i_FaceIdx, int i_VertIdxInFace, Point3 &o_Normal );

		  //get i-th normal acording to the normal computation
		  Point3 GetIthNormal( int i_NormalIdx );
	};


	//=============================================================================
	// This is a trivial implementation of computing normals of meshes,
	// user on error situations
	//=============================================================================
	class ComputeNormals_Dummy : public ComputeNormalsBase
	{
	public:
		ComputeNormals_Dummy( PolyOrTriMesh &i_Parent ):
		  ComputeNormalsBase( i_Parent ){}

		  ~ComputeNormals_Dummy(){}

		  //get the number of normals
		  int GetNNormals() { return 0; }

		  //get the normal gven the face corresponding and the index of the vertex in the face
		  //return the index of the normal computed
		  int  GetNormal( int i_FaceIdx, int i_VertIdxInFace, Point3 &o_Normal )
		  {
			  return -1;
		  }

		  //get i-th normal acording to the normal computation
		  //This could be i'th user specified normal
		  //or the ith RNNormal or the i-th face normal
		  Point3 GetIthNormal( int i_NormalIdx )
		  {
			  Point3 normal(0,0,0);
			  return normal;
		  }
	};

} //namespace MaxExp