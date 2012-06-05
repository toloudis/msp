/*****************************************************************************
**  MtlExporter.hpp
**
**	Exports materials
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MAXEXP_MTLEXPORTER_HPP
#error MAXEXP_MTLEXPORTER_HPP multuply defined!!
#endif
#define MAXEXP_MTLEXPORTER_HPP

#ifndef MAXEXP_BASEEXPORTER_HPP
#include "BaseExporter.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <list>
#include <max.h>
#include <hash_set>

//forward declarations
class mdlMaterialInfo;
class fsLocator;

namespace MaxExp
{
	class ExportDoc;
}


namespace MaxExp
{
	//========================================================================
	// This is utility struct used for containing the submtl info 
	// 
	// MtlExporter::GetSubMtlInfo will fill the output members of this structure
	//========================================================================
	struct SubMtlAux
	{
		int m_nFaceMatId;
		Mtl *m_pSubMtl;

		SubMtlAux():
		m_nFaceMatId(-1),
			m_pSubMtl(NULL)
		{}
	};


	//========================================================================
	// Mtl Exporter Class
	//	
	//========================================================================
	class MtlExporter: public BaseExporter
	{
	public:
		MtlExporter( ExportDoc &io_doc):
		  BaseExporter(io_doc){}
		  ~MtlExporter(){}
		  void Export( Mtl *i_pMtl );
			
		  //given a node material and a sub mtl id, get the sub mtl
		  void GetSubMtlFromFaceMatId( int i_nFaceMatId, Mtl *i_pNodeMtl, SubMtlAux & o_subMtlInfo ) const;

		  //get the submtl name
		  //(In case of an erroneous submtl, this returns "SgpuErrorMtl")
		  std::string GetSubMtlName( Mtl *i_pSubMtl ) const;

		  //log all materials stored in the hash m_MtlHashForLogging
		  void LogMaterials( )const;
	private:
		//export the given mtl  into a mdlMatInfos tructure
		void ExportCore( Mtl *i_pMtl, mdlMaterialInfo &o_MatInfo);
		void MakeErrorMatInfo( mdlMaterialInfo &o_MatInfo)const;
		bool IsErrorMaterial(Mtl *i_pMtl)const;
		//warn if the given texture name doesnt exist
		void WarnIfTextureDoesntExists( const fsLocator &texLoc );
		//write the properties of the material to the log
		void LogMaterial( Mtl *i_pMtl ) const;
	private:
		stdext::hash_set< Mtl * > m_MtlHashForLogging;
	};
} //namespace MaxExp