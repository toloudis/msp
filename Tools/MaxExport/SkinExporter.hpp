/*****************************************************************************
**  SkinExporter.hpp
**
**	Exports materials
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MAXEXP_SKINEXPORTER_HPP
#error MAXEXP_SKINEXPORTER_HPP multuply defined!!
#endif
#define MAXEXP_SKINEXPORTER_HPP

#ifndef MAXEXP_BASEEXPORTER_HPP
#include "BaseExporter.hpp"
#endif
#ifndef MAXEXP_MAXMESHUTILS_HPP
#include "MaxMeshUtils.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#include <list >
#include <hash_set>


namespace MaxExp
{
	class ExportDoc;
}


namespace MaxExp
{ 
	
	class SgpuPhysiqueInterface 
	{
	public :
		SgpuPhysiqueInterface( ExportDoc &i_ExportDoc, INode *i_pCurNode, TimeValue i_CurTime);
		~SgpuPhysiqueInterface();
		bool Found() const;		
		template < class Cont > void GetBoneNodes( Cont &bones );			 
		template< typename E > void EnumerateBoneNodes( E &eop ) ;
	public:
		IPhysiqueExport *m_pIMod;
		IPhyContextExport *m_pIContext;
		INode *m_pCurNode;
		TimeValue m_CurTime;
		Modifier *m_pModifier;
		ExportDoc *m_pExportDoc;
	};


	class SgpuSkinModInterface
	{
	public:
		SgpuSkinModInterface(  ExportDoc &i_ExportDoc, INode *i_pCurNode, TimeValue i_CurTime);
		~SgpuSkinModInterface();
		bool Found() const;				
		template < class Cont > void GetBoneNodes( Cont &bones );	
	public:
		ISkin *m_pIMod;
		ISkinContextData *m_pIContext;
		INode *m_pCurNode;
		TimeValue m_CurTime;
		Modifier *m_pModifier;
		ExportDoc *m_pExportDoc;
	};


	//========================================================================
	// Skin Exporter Class
	//	
	//========================================================================
	class SkinBaseExporter: public BaseExporter
	{
	public:
		typedef std::pair<INode *, float> BoneInfluence;
		class ExportOutput: public std::map< int, std::vector< BoneInfluence > >
		{
		public:
			typedef std::vector< BoneInfluence > BoneInflVector;
			typedef std::map< int, BoneInflVector > BaseType;	
		public:
			ExportOutput():m_NumMaxVerts(0){}
			int m_NumMaxVerts;
		};

	public:
		SkinBaseExporter( ExportDoc &io_doc):
		  BaseExporter(io_doc){}
		 virtual ~SkinBaseExporter(){}				      
		 void Export ( INode * i_pCurNode,  ExportOutput &o_Output );
		 virtual void GetBonesAssociatedWithThisMesh( INode * i_pCurNode, INodeHS &io_Bones )=0;
	};


	class SkinModExporter: public  SkinBaseExporter
	{
	public:
		SkinModExporter( ExportDoc &io_doc ):
		  SkinBaseExporter( io_doc ) {}
		  ~SkinModExporter(){}	     
		  void Export ( INode * i_pCurNode,  ExportOutput &o_Output ){}
		  void GetBonesAssociatedWithThisMesh( INode * i_pCurNode, INodeHS &io_Bones );
	};

	class PhysiqueExporter: public SkinBaseExporter
	{
	public:
		PhysiqueExporter( ExportDoc &io_doc):SkinBaseExporter( io_doc ){}
		 ~PhysiqueExporter(){}
		 void Export ( INode * i_pCurNode,  ExportOutput &o_Output );
		 void GetBonesAssociatedWithThisMesh( INode * i_pCurNode, INodeHS &io_Bones );
	};

} //namespace MaxExp