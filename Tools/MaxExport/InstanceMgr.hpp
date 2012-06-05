/*****************************************************************************
**  InstanceMgr.hpp
****
**	Manages the Instancing
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MAXEXP_INSTANCEMGR_HPP
#error MAXEXP_INSTANCEMGR_HPP multiply defined!!
#endif
#define MAXEXP_INSTANCEMGR_HPP


#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif
#include <hash_set>
#include <map>

//forward declarations
class Matrix3;
class mdlNodeInfo;
class INode;
class gfFileBin;
class chBinWriter;
namespace MaxExp
{
	class ExportDoc;
}

namespace MaxExp
{

	class SgpuInstanceMgr
	{
	public:
		struct TData {
			TData():
			m_LeadingInstance(NULL){}
			
			INode *m_LeadingInstance;
			shared_ptr< mdlNodeInfo > m_NodeInfo;
		};
		typedef std::map< INode *, TData > TMap;
	public:
		SgpuInstanceMgr( ExportDoc &exportDoc ):
		 m_pExportDoc(&exportDoc)
		 {}
		~SgpuInstanceMgr(){}
		size_t FindInstance( INode *i_pCurNode, TData &o_TData );
		void AddInstances( INode *i_pCurNode, shared_ptr<mdlNodeInfo> &i_NodeInfo );
		TMap m_InstanceMap;
		void Cleanup()
		{
			m_InstanceMap.clear();
		}
		enum EInstancingType { eNoInstancing, eFragInstancing, eNodeInstancing };
		static EInstancingType  AnalyzeMtlsRec( ExportDoc &exportDoc, INode *i_pNode1, INode *i_pNode2 );
	protected:
		
		ExportDoc *m_pExportDoc;

	};
} //namespace MaxExp
