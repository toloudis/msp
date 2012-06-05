/*****************************************************************************
**  MaxMeshUtils.hpp
**
** 	Collection of  utility functions and classes, 
**	related to meshes that glues Sgpu api and max api
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MAXEXP_MAXMESHUTILS_HPP
#error MAXEXP_MAXMESHUTILS_HPP multuply defined!!
#endif
#define MAXEXP_MAXMESHUTILS_HPP

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
	class SkinBaseExporter;
	class PolyOrTriMesh;
}

namespace MaxExp
{

	    //These are visit structures used to 
	    //traverse the 3dsmax INode hierarchy.
		//Please see the function
		//template< class Visit >
		//void VisitINodeHierarchyRec(INode *i_pCurNode, Visit &io_Visit )
		//in MaxExportUtils.hpp
		
	
	
		//This collect the bones associated with
	    //the current node into m_BonesAssociatedWithAllMeshes
	    struct AssociatedBonesCollectVisit
		{
			//dummy stack type which is not used
			typedef int stack_type;
			AssociatedBonesCollectVisit( ExportIntent &exportIntent ):
			m_Stack(0),
			m_ExportIntent( exportIntent )
			{}
		
			void Pre( INode  *i_pCurNode );
			void Post ( INode *i_pCurNode ){}			
			stack_type m_Stack;
			typedef INodeHS Cont;
			INodeHS m_BonesAssociatedWithAllMeshes;
			ExportIntent &m_ExportIntent;
		};


		
		//Given any node, if any bones are associated with the node
		//find the root bone of the associated bone hierachy
		struct FindRootBoneOfThisHierarchy
		{
			FindRootBoneOfThisHierarchy( ExportDoc *i_pExportDoc ):
				m_pExportDoc(i_pExportDoc ){}
			//auxillary function to check whether a node is
			//bone or not
			struct IsBone : std::binary_function<  INode *, bool, bool >
			{
				//if bNegate is true
				//returns false if i_pCurNodeis a bone
				//otherwise returns true if i_pCurNode is a bone
				bool operator()( INode * i_pCurNode, SgpuExportOptions &i_Options,  bool i_bNegate );
			};

			INode * operator()( INode * i_pCurBone ) ;
			
			typedef std::map< INode *, INode *> RootBoneCacheType;
			RootBoneCacheType m_RootBoneCache;

			ExportDoc *m_pExportDoc;
		};


		//Collect the bones in depth first order into
		//m_BonesOfThisHierarchy
		struct DepthFirstBoneHierarchyTraversal
		{
			DepthFirstBoneHierarchyTraversal( ExportDoc *i_pExportDoc ):
				m_pExportDoc( i_pExportDoc ){}
			//dummy stack type which is not used
			typedef int stack_type;
			DepthFirstBoneHierarchyTraversal( INode  *i_pCurNode, ExportDoc *i_pExportDoc  ):
			m_Stack(0),
			m_pCurBoneRoot( i_pCurNode ),
			m_pExportDoc( i_pExportDoc )
			{}
			void Pre( INode  *i_pCurNode );
			void Post ( INode *i_pCurNode ){}			
			stack_type m_Stack;
			typedef std::deque< INode * > Cont;
			Cont m_BonesOfThisHierarchy;
			INode *m_pCurBoneRoot;
			ExportDoc	*m_pExportDoc;
		};


	//========================================================================
	//This class upon constructed will
	//search the given INode's modifier stack,
	//to find any marching modifiers.
	//If any matching modifier is found, 
	//the following class members will be 
	//appropriately initialized

	//
	//m_pIDerivedObject
	//m_pModifier
	//m_iModStackIdx
	//========================================================================


	class ResolveModifier
	{
	public:			

		
		struct MatchBase :  public std::unary_function< Modifier *, bool >
		{
			virtual ~MatchBase(){}
			virtual bool operator()( Modifier* i_pModifier )const=0;
		};

		template< ULONG a, ULONG b>
		struct Match : public MatchBase
		{		
			bool operator()( Modifier *i_pModifier )const
			{
				return ( i_pModifier->ClassID() == m_TheClassID ) ? true: false;
			}			
			static const Class_ID m_TheClassID;
		};



	public:
		ResolveModifier(INode *i_pCurNode, TimeValue i_Time):
			m_pCurNode( i_pCurNode ),
			m_CurTime( i_Time ),
			m_pIDerivedObject( NULL ),
			m_pModifier( NULL ),
			m_iModStackIdx( -1 )
			{}
	 virtual ~ResolveModifier(){}
	 virtual bool Found() const { return m_pIDerivedObject != NULL ; }
	 IDerivedObject *GetIDerivedObject() { return m_pIDerivedObject; }
	 Modifier *GetModifier() { return m_pModifier; }
	 int GetModStackIdx() { return m_iModStackIdx; }	
	 void Init();
	protected:
		INode *m_pCurNode;
		TimeValue m_CurTime;
		IDerivedObject *m_pIDerivedObject;
		Modifier *m_pModifier;
		int m_iModStackIdx;
		typedef std::deque< shared_ptr<MatchBase> > MatchCont;
		MatchCont m_Matchers;
		Match< POINTCACHE_OSM_ID_A, POINTCACHE_OSM_ID_B > m_PointCacheMatch;
	};
	
	template< ULONG a, ULONG b> const Class_ID ResolveModifier::Match<a,b>::m_TheClassID = Class_ID( a, b );

	template< >
	bool ResolveModifier::Match< SKIN_CLASS_ID_A, SKIN_CLASS_ID_B >::operator()( Modifier *i_pModifier )const;

	
	//========================================================================
	//A derived class of ResolveModifier,
	//which specializes in Skin or Physique modifer
	//========================================================================
	class ResolveSkinModifier : public ResolveModifier
	{
	public:
		typedef Match<  PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B > PhysiqueMatch;
		typedef Match<  SKIN_CLASS_ID_A, SKIN_CLASS_ID_B > SkinMatch;
	public:
		ResolveSkinModifier(ExportDoc &doc,  INode *i_pCurNode, TimeValue i_Time );
		virtual ~ResolveSkinModifier(){}
		SkinBaseExporter *GetSkinExporter()const;
		ExportDoc *m_pExportDoc;
	};



	//========================================================================
	//Two policies used for extracting the 
	//correct object ref, in the presence
	//of modifiers
	//========================================================================
	struct ResolvePolicyBase{};

	struct ResolvePolicyBindPose: public ResolvePolicyBase
	{
		static Object * Do( INode * i_pCurNode, ResolveModifier &detect, TimeValue i_Time );
		static PolyOrTriMesh * MakePolyOrTriMesh( INode &i_CurNode, ExportDoc *i_pExportDoc, Object &i_MaxObject, bool i_bTriangulate );
	};

	struct ResolvePolicyVertAnim: public ResolvePolicyBase
	{
		static Object * Do( INode * i_pCurNode, ResolveModifier &detect, TimeValue i_Time );	
		static PolyOrTriMesh * MakePolyOrTriMesh( INode &i_CurNode, ExportDoc *i_pExportDoc, Object &i_MaxObject, bool i_bTriangulate );

	};


	//========================================================================
	//Class used for holding the objct ref 
    //resolved from  a node.
	//The resolution is done at construction of this class
	//========================================================================
	template < class ResolvePolicy >
	class ObjectRefResolver : public ResolvePolicy 
	{
	public:

		ObjectRefResolver( 
			ExportDoc &i_ExportDoc, 
			INode *i_pCurNode, 
			TimeValue i_Time , 
			bool i_bTriangulate );
		
		virtual ~ObjectRefResolver(){}
		  Object *GetMaxObject() { return m_pMaxObject; }	  
		  shared_ptr<PolyOrTriMesh> &GetPolyOrTriMesh() { return m_PolyOrTriMesh; }
		  INode *m_pCurNode;
	protected:
		shared_ptr< PolyOrTriMesh > m_PolyOrTriMesh;
		Object *m_pMaxObject;
		TimeValue m_Time;
		ExportDoc *m_pExportDoc;
		bool m_bTriangulate;
	};


    template < class ResolvePolicy >
	ObjectRefResolver< ResolvePolicy > ::ObjectRefResolver(ExportDoc &i_ExportDoc, INode *i_pCurNode, TimeValue i_Time, bool i_bTriangulate ):	 
	m_pCurNode( i_pCurNode ),
	m_pMaxObject(NULL),
	m_Time( i_Time ),
	m_pExportDoc( &i_ExportDoc ),
	m_bTriangulate( i_bTriangulate )
	{
		assert( m_pCurNode );
		ResolveSkinModifier skinDet( *m_pExportDoc,  m_pCurNode , m_Time);
		m_pMaxObject = ResolvePolicy::Do( m_pCurNode, skinDet,  m_Time );
		if(  NULL == m_pMaxObject )
		{
			EXPLOG.WriteError(_T("node '%s' has null obj!"), MBCSTOLPCSTR( m_pCurNode->GetName() ) );
			throw export_failure();
		}
		//get the poly or triangular representation
		m_PolyOrTriMesh.reset(  ResolvePolicy::MakePolyOrTriMesh( *m_pCurNode, m_pExportDoc, *m_pMaxObject, m_bTriangulate ) );
	
	}
	//========================================================================
	//Base class for doing the core
	//work of exporting geometry 
	//as subdivInfo, triangular mesh or
	//exporting animation as vertex animation.
	//========================================================================
	template< class ResolvePolicy >
	class MeshBaseExportCore
	{	
	public:
		
		//A template of template
		//See http://www.informit.com/articles/article.aspx?p=376878
		template< template < typename T, typename Alloc > class Cont >
		class TMtlIdFaceListMapT : public std::map< int, Cont<int, std::allocator<int> > >
		{
		public:
			typedef Cont<int, std::allocator<int> > FaceIdxsT;  //list of face-ids
			typedef map<int, FaceIdxsT> BaseType; //material versus list of face-ids 
							//that the material is associated with	
			typedef int IdxT;
		};


		typedef TMtlIdFaceListMapT< std::list > MtlIdFaceListMapT;
		
	public:
		MeshBaseExportCore( 
			ExportDoc &exportDoc, // the base exportDoc
			INode &i_CurNode,  //the node that is exported
			TimeValue i_ExportTime,	//the time at which export is made
			const maMatrix4x4 &i_VertTransform, //the transform that need be applied to the geometry
			bool i_bTriangulate //should we truangulate the geometry
			);
		virtual ~MeshBaseExportCore(){};			
		void GetMtlIdFaceMap( MtlIdFaceListMapT &o_mtlIdFaceListMap );
	protected:
		ExportDoc *m_pExportDoc;
		INode *m_pCurNode;
		TimeValue m_ExportTime;
		const maMatrix4x4 &m_VertTransform;		
		maMatrix4x4 m_NormalTransform;
	protected:
		shared_ptr< ObjectRefResolver< ResolvePolicy> > m_ObjResolver;
	protected:
		bool CheckUserSpecifiedNormals( ::Mesh & mesh );
		Point3 GetVertexNormalForTheFace( ::Mesh &i_Mesh, int i_nFaceId, int i_nVertIdxInFace );
		void ExportMeshMaterials(  );
	
	};

	template< class ResolvePolicy >
	MeshBaseExportCore< ResolvePolicy >::MeshBaseExportCore(  
		ExportDoc &exportDoc, 
		INode &i_CurNode, 
		TimeValue i_ExportTime,		
		const maMatrix4x4 &i_GeometryTransform,
		bool i_bTriangulate
		):	
		m_pExportDoc( &exportDoc ),
		m_pCurNode( &i_CurNode ),
		m_ExportTime( i_ExportTime ),
		m_VertTransform( i_GeometryTransform )
	{					
	
		//make an objectRefResolver, which resolves the
		//object ref at construction
		m_ObjResolver.reset( new ObjectRefResolver< ResolvePolicy > (exportDoc,  &i_CurNode, i_ExportTime, i_bTriangulate ) );
		m_NormalTransform = m_VertTransform;
		m_NormalTransform.Invert();
		m_NormalTransform.Transpose();
		
	}

	
	//========================================================================
	//The geometry is exported as a triangulated mesh.
	//Also, for resolving the max objefct ref, the REsolvePOlicyBindPose is used.
	//========================================================================
	class MeshGeomExportCore : public MeshBaseExportCore< ResolvePolicyBindPose >
	{
	public:
		MeshGeomExportCore( 
			ExportDoc &exportDoc, 
			INode &i_CurNode, 
			TimeValue i_ExportTime,
			const maMatrix4x4 &i_GeometryTransform,
			bool i_bAddRemapInfo
			);
		~MeshGeomExportCore(){};	
		void DoExport( shared_ptr<mdlFragInfo> &meshInfo);
	protected:
		bool m_bAddRemapInfo;
	protected:
		bool m_bPackNormals;
	};

	
	//========================================================================
	//The geometry is exported as a subdiv mesh.
	//Also, for resolving the max object ref, the ResolvePolicyBindPose is used.
	//========================================================================
	class SubdivisionExportCore : public MeshBaseExportCore< ResolvePolicyBindPose >
	{
	public:
		SubdivisionExportCore( 
			ExportDoc &exportDoc, 
			INode &i_CurNode, 
			TimeValue i_ExportTime,
			const maMatrix4x4 &i_GeometryTransform,
			bool i_bAddRemapInfo
			);
		~SubdivisionExportCore(){};	
		void DoExport( shared_ptr<mdlSubdivInfo> &subdivInfo);
	protected:
		bool m_bAddRemapInfo;
	};
	
	
	//========================================================================
	//The vertex animation is exported
	//========================================================================
	class MeshAnimExportCore : public MeshBaseExportCore < ResolvePolicyVertAnim >
	{

	public:
		MeshAnimExportCore( 
			ExportDoc &exportDoc, 
			INode &i_CurNode, 
			TimeValue i_ExportTime,	
			const maMatrix4x4 &i_GeometryTransform,
			bool i_bTriangulate
			);
		~MeshAnimExportCore(){};	
		void DoExportFrame(  vtxVertexFrame *&o_pVertexFrame );

	};
	
	maMatrix4x4 CalculateRequiredWorldTransformForGeometry
		(
		ExportDoc *i_pExportDoc,
		INode *i_pCurNode,
		INode *i_pExportedRootNode,
		TimeValue i_CurTime
		);

	bool CheckEquivalencyOfMtlIdFaceListMap( ExportDoc &exportDoc, INode *i_pNode1, INode *i_pNode2 );

} //namespace MaxExp