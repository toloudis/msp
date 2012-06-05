/*****************************************************************************
**  ExportDoc.cpp
**
**	The main document containing/coordinating all 
**	the book keeping of the 3ds max exporter
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "InstanceMgr.hpp"
#include "MaxExportUtils.hpp"
#include "MaxMeshUtils.hpp"
#include "MaxObjectFlags.hpp"



#include "MaxCommon.hpp"
#include <iInstanceMgr.h> 
using namespace std;

namespace MaxExp
{	
	/*
	A set of INode instances can be correctly represented as the instances
	in the GXB file, only if either of  the following constraints apply
	
	constraint_A: 
	they are leaf INodes(ie: they have no child iNodes), 
	with potentially different materials but the face mapping is similar
	Eg: if Node 1 had a 5 faces and two sub materials with the face mapping,
	( submtl_0 = { face_0, face_3}, submtl_1 = { face_1, face_2, face_4 },
	then for Node2 also there should be two potentially different submaterials
	say submtl_2, and submtl_3 with a similar face mapping as in
	( submtl_2 = { face_0, face_3}, submtl_3 = { face_1, face_2, face_4 }

	This type of instancing is termed eFragInstancing

	constraint_B:
	They have the similar tree subtree structure,
	and each INode in one tree and the coresponding INode in 
	th other subtree have same material and material mapping.

	This type of instancing is called eNodeInstancing.
	If either of two conditions dont apply, then the INode set have to be 
	further splitup into equivalence sets.
	*/



	//This will return either eNoInstancing, eFragInstancing or eNodeInstancing.

	SgpuInstanceMgr::EInstancingType  SgpuInstanceMgr::AnalyzeMtlsRec(ExportDoc &exportDoc, INode *i_pNode1, INode *i_pNode2 )
	{
		EInstancingType ret  = eNoInstancing;
		assert( NULL != i_pNode1 );
		assert( NULL != i_pNode2 );
		Mtl *pMtl1 = i_pNode1->GetMtl();
		Mtl *pMtl2 = i_pNode2->GetMtl();
		int nChild1 = i_pNode1->NumChildren();
		int nChild2 = i_pNode2->NumChildren();
		MCHAR *pzNodeName1 = i_pNode1->GetName();
			MCHAR *pzNodeName2 = i_pNode2->GetName();

		//both of the nodes have the same number of children
		if( nChild1 != nChild2 )
		{			
			EXPLOG.WriteWarning( "instaned bnodes %s:%d and %s:%d have different number of children", MBCSTOLPCSTR( pzNodeName1 ), nChild1,  MBCSTOLPCSTR( pzNodeName2 ), nChild2 );
			return ret; //eNoInstancing
		}
		assert ( nChild1 == nChild2 );

		bool bExportAsSubdiv1;
		bool bObjFlagRet = AllowedObjectFlags::GetValue( i_pNode1, L"export as subdiv", bExportAsSubdiv1);					
		DBG_ASSERT( bObjFlagRet, "cannot get export as subdiv flag!");
		bool bExportAsSubdiv2;
		bObjFlagRet = AllowedObjectFlags::GetValue( i_pNode2, L"export as subdiv", bExportAsSubdiv2 );	
		DBG_ASSERT( bObjFlagRet, "cannot get export as subdiv flag!");


		if( (!bExportAsSubdiv1 &&  bExportAsSubdiv2) || (bExportAsSubdiv1 && !bExportAsSubdiv2) )
		{
			EXPLOG.WriteWarning( "instaned bnodes %s:%d and %s:%d have different  'export as subdiv' flags", MBCSTOLPCSTR( pzNodeName1 ), nChild1,  MBCSTOLPCSTR( pzNodeName2 ), nChild2 );		
			return ret;
		}

		if( nChild1 <=  0 )
		{
			//If there is no  child then 
			//do the fragInstancing
			if( pMtl1 == pMtl2 )
			{
				bool bSimilarMtlIdFaceMap  = CheckEquivalencyOfMtlIdFaceListMap(exportDoc,  i_pNode1, i_pNode2 );
				if( !bSimilarMtlIdFaceMap )
				{
					int k=0;
				}
				return ( bSimilarMtlIdFaceMap ) ? eFragInstancing : eNoInstancing;
			} else
			{
#if MULTI_MATERIAL_INSTANCING
				bool bSimilarMtlIdFaceMap  = CheckEquivalencyOfMtlIdFaceListMap(exportDoc,  i_pNode1, i_pNode2 );
				return ( bSimilarMtlIdFaceMap ) ? eFragInstancing : eNoInstancing;
#endif
				return eNoInstancing;
			}
		} else
		{
			if( pMtl1 == pMtl2 )
			{		
				//if  the materials are the same
				EInstancingType eHierarchyInstancingType = eNodeInstancing;
				for(int i=0; i < nChild1 && eHierarchyInstancingType == eNodeInstancing ; ++i )
				{
					INode *pChild1 = i_pNode1->GetChildNode( i );
					INode *pChild2 = i_pNode2->GetChildNode( i );
					Mtl *pChMtl1 = pChild1->GetMtl();
					Mtl *pChMtl2 = pChild2->GetMtl();
					eHierarchyInstancingType =  AnalyzeMtlsRec(exportDoc,  pChild1,  pChild2 );
					if( eHierarchyInstancingType == eFragInstancing ) 
					{
						if(  pChMtl1 != pChMtl2  )
						{
							eHierarchyInstancingType = eNoInstancing;
						} else
						{
							eHierarchyInstancingType = eNodeInstancing;
						}
					}
				}			
				return eHierarchyInstancingType;
			} else
			{
				//Materials are not the same
				//and this node contains children 
				//So dont do instancing
				return  eNoInstancing;
			}
		}		
	}


	/*
    Max SDK uses IInstanceMgr to return a sequence of 'INode *'
	which are instances according to max. But to be represented in
	the gxb file, this sequence need be further split into equivalence sets
	where all INode-s in each  equivalence set satisfies the same kind of
	one of the two constraints.

	To make equivalence sets, we have te following routine.

	*/
	struct InstanceComparator
	{
		InstanceComparator( ExportDoc &exportDoc ):
			m_pExportDoc( &exportDoc )
			{}
		int operator()( INode *i_pNode1, INode *i_pNode2 )
		{
			SgpuInstanceMgr::EInstancingType etype = SgpuInstanceMgr::AnalyzeMtlsRec( *m_pExportDoc,  i_pNode1, i_pNode2 );
			return static_cast<int>( etype );
		}
	ExportDoc *m_pExportDoc;
	};

	/*


	I iterator type for the input sequence of INode * pointers
    Cont is  a container class which takes in two template paramters, 
	   U = element type
	   A = allocator type   
   Cont is the container that holds an equivalence set of INode instances.
    b = begin itertor of the sequence of INode*-s which are instances of the same node
	    as claimed by the IInstanceMgr.
	e = end iterator
	out = list of Cont-s.
	each Cont will be an equivalnce set of gxb-instances



	*/
	template< typename T >
	class Describe
	{
	  public:
		  Describe(){};
		  string operator()(const  T t ){}
	};


	template< >
	class Describe< INode * >
	{
	  public:
		  Describe(){};
		 string operator()(const  INode *i_pNode )
		{
			INode *pNode = const_cast<INode*>( i_pNode);
			MCHAR *szName = pNode->GetName();
			return string( MBCSTOLPCSTR( szName ) );
		}
	};

	/**
	Given a sequence of instances of the i_PrimaryNode,
	filter out a sub list of instances that match i_PrimaryNodes
	in sub-material assignment of face or hierarchy structure
	*/
	template< typename T,  typename I,  template< typename U, typename A > class Cont >
	void FormListOfMatchingInstances ( T i_PrimaryNode,  I i_Begin, I i_End, InstanceComparator &i_Comp, Cont< T, std::allocator<T> > &o_Out )
	{	
		I it;
		for( it = i_Begin; it != i_End; ++it )
		{
			int ret = i_Comp( i_PrimaryNode, *it );			
			if( ret > 0 )
			{				
				T nodePtr = *it;
				Describe< INode *> des;
				string nodeName = des(  nodePtr );
				string primaryNodeName = des( i_PrimaryNode );
				EXPLOG.WriteInfo( "Instance processing node %s as instance of %s\n", nodeName.c_str(), primaryNodeName.c_str() );
				o_Out.push_back( *it );
			}
		}
	}

	
	

	size_t SgpuInstanceMgr::FindInstance( INode *i_pCurNode ,  TData &o_TData )
	{
		IInstanceMgr *iMgr = IInstanceMgr::GetInstanceMgr();
		assert( iMgr );
		INodeTab instances;
		size_t nInstances = iMgr->GetInstances( *i_pCurNode, instances );
		if( nInstances > 1 )
		{
			TMap::const_iterator mit;

			mit = m_InstanceMap.find( i_pCurNode );
			if( mit != m_InstanceMap.end() )
			{
				o_TData = mit->second;
			}
		}		
		return nInstances;
	}




	void SgpuInstanceMgr::AddInstances( INode *i_pCurNode, shared_ptr<mdlNodeInfo> &i_NodeInfo )
	{
		TData retVal;
		IInstanceMgr *iMgr = IInstanceMgr::GetInstanceMgr();
		assert( iMgr );
		INodeTab instances;
		const MCHAR *szNodeName = i_pCurNode->GetName();
		size_t nInstances = iMgr->GetInstances( *i_pCurNode, instances );
		bool bAutoMtlConfig = iMgr->GetAutoMtlPropagation();
		if( nInstances > 1 )
		{	
			TMap::const_iterator mit;	
			mit = m_InstanceMap.find( i_pCurNode );
			if( mit == m_InstanceMap.end() )
			{
				EXPLOG.WriteInfo( "Making a table of %d instances of %s\n", nInstances,  MBCSTOLPCSTR( szNodeName ) );
				vector<INode*> instanceVec( instances.Addr(0), (instances.Addr(0) + nInstances) ); 
				typedef vector<INode*> MatchingInstanceCont;
				 MatchingInstanceCont matchingInstances;
				InstanceComparator comp( *m_pExportDoc );
				FormListOfMatchingInstances< INode *, vector<INode*>::iterator, vector  > (
					i_pCurNode,
					instanceVec.begin(), 
					instanceVec.end(), 
					comp,
					matchingInstances
					);

#if  MULTI_MATERIAL_INSTANCING

				std::list< EquivalenceSetCont >::const_iterator lit;
				for( lit = listOfEquivalenceSets.begin(); lit != listOfEquivalenceSets.end(); ++lit )
				{
					const EquivalenceSetCont &equiSet = *lit;
					if( equiSet.size() > 1)
					{
						EquivalenceSetCont::const_iterator eit, eitBegin;
						eitBegin = equiSet.begin();
						for( eit = equiSet.begin(); eit != equiSet.end(); ++eit )
						{
							INode *pNode =  *eit;
							TData tdata;
							tdata.m_LeadingInstance = *eitBegin;
							tdata.m_NodeInfo = i_NodeInfo;
							TMap::value_type v( pNode, tdata );
							m_InstanceMap.insert( v );
						}
					} else
					{
						//since this equivalenc set contain only one instanceNode
						//dont bother to count as an instance
						continue;
					}
				}
#else
			
				mit = m_InstanceMap.find( i_pCurNode );
				assert( mit == m_InstanceMap.end() );
				//mathingInstances whould atleast have
				//i_pCurNode
				assert( matchingInstances.size() >= 1);
				int nMatchingInstances = matchingInstances.size();
				for( int i =0; i < matchingInstances.size(); ++i )
				{
					INode *pNode =  matchingInstances[i];
					TData tdata;
					tdata.m_LeadingInstance = i_pCurNode;
					tdata.m_NodeInfo = i_NodeInfo;
					TMap::value_type v( pNode, tdata );
					m_InstanceMap.insert( v );
				}
#endif
			} //if( mit == m_InstanceMap.end() )
		}		//if( nInstances > 1 )
		return;
	}


	

} //namespace MaxExp