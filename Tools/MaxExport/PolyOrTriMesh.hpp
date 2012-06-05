/*****************************************************************************
**  PolyOrTriMesh.hpp
**
**	Wrapper class that encompasses common functionalities of Mesh and MNMesh
**	of 3ds Max SDK
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MAXEXP_POLYORTRIMESH_HPP
#error MAXEXP_POLYORTRIMESH_HPP multuply defined!!
#endif
#define MAXEXP_POLYORTRIMESH_HPP

#ifndef MAXEXP_MAXEXPORTERUTILS_HPP
#include "MaxExportUtils.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef  MA_VECTOR2D_HPP
#include "Core/Ma/maVector2d.hpp"
#endif
#ifndef  MA_VECTOR3D_HPP
#include "Core/Ma/maVector3d.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#include "MaxCommon.hpp"
#include "CS/Phyexp.h"
#include "iskin.h"
#include "ipointcache.h"
#include <string>
#include <map>
#include <vector>
#include <list>
#include <exception>
#include <tchar.h>
#include <ctime>
#include <hash_set>
#include <deque>


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
	class ComputeNormalsBase;
}

namespace MaxExp
{

	//========================================================================
	// A utility class which hides the logic of detrmining,
	// whether the current node has a poly-mesh or  triangular representation.
	// Please see .cpp for detail
	//========================================================================
	class PolyOrTriMesh
	{
	public:
		//Complex logic for extracting the
		//tri or poly data from a max obj
		PolyOrTriMesh( ExportDoc *i_pExportDoc ):
		  m_pCurNode(NULL),
			  m_pObj(NULL),
			  m_pTri(NULL),
			  m_pPoly(NULL),
			  m_bDeleteObj(false),
			  m_pExportDoc( i_pExportDoc )
		  {
		  }
		  PolyOrTriMesh( INode &curNode, ExportDoc *i_pExportDoc,  Object &obj, bool bTriangulate );
		  ~PolyOrTriMesh();
		  bool IsValid() const;
		  bool IsPoly() const;
		  bool IsTri() const;
		  //Each mesh, in addition to the position and normal info
		  //can have additional channels which are in meshmap-s.
		  //This gets all the  texture channel ids of the mesh.

		  void GetTextureChannels( std::vector<int> &o_Channels );
		  int GetNVerts()const;
		  int GetNFaces() const;
		  int GetNVertsInFace( int fIdx );
		  int GetNTVerts( int tchIdx);		
		  int GetVertIdxInMesh( int faceIdx, int vertIdxInFace );	
		  int GetTVertIdx( int tchIdx, int faceIdx, int vertIdxInFace );		
		  UVVert  GetTVert( int tchIdx, int tVertIdx );
		  Point3  GetVert( int vertIdxInMesh );		
		  //Get the number of normals  used by the underlying computeNormals- class
		  int GetNNormals();
		  //get the normal gven the face corresponding and the index of the vertex in the face
		  int  GetNormal( int i_FaceIdx, int i_VertIdxInFace, Point3 &o_Normal );		
		  //get i-th normal acording to the normal computation
		  Point3 GetIthNormal( int i_NormalIdx );
	public:
		INode *m_pCurNode;
		Object *m_pObj; //the original ma object
		bool m_bDeleteObj; //the final tri or poly is a different object, so the original obj ned be deleted
		PolyObject *m_pPoly;  //polyObject representation
		TriObject *m_pTri; //triObject representation
		ExportDoc *m_pExportDoc;
		shared_ptr< ComputeNormalsBase > m_ComputeNormals; //method used for computing the normals
		//Note that this is initialized only when the need for computing
		//the normals arises
	protected:
		//This is called from the constructor
		void InitWithConversionToTri();		
		void InitWithConversionToPoly();
	};


} //namespace MaxExp