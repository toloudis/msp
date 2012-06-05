
/*****************************************************************************
**  VertAnimExporter.hpp
**
**	Exports nodes containing geometry (poly-mesh, tri-mesh, shapes)
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MAXEXP_VERTANIMEXPORTER_HPP
#error MAXEXP_VERTANIMEXPORTER_HPP multuply defined!!
#endif
#define MAXEXP_VERTANIMEXPORTER_HPP


#ifndef MAXEXP_BASEEXPORTER_HPP
#include "BaseExporter.hpp"
#endif
#ifndef ENV_TYPE_HPP
#include "Core/Env/envType.hpp"
#endif 
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef MDL_VERTEXANIMWRITER_HPP
#include "Graphics/vtx/vtxVertexAnimWriter.hpp"
#endif

#include <list>
#include <string>
#include <max.h>
//forward declarations
class INode;
class mdlNodeInfo;
class mdlFragInfo;
class maMatrix4x4;
class chWriter;
class vtxVertexAnimKeysStatic;
class vtxVertexFramesStatic;
class vtxStreamCompression;

namespace MaxExp
{
	class ExportDoc;
	class VertAnimExportIntent;

}


namespace MaxExp
{	



	//========================================================================
	// Mesh Exporter Class
	//	
	//========================================================================
	class VertAnimExporter: public BaseExporter
	{
	public:




		//========================================================================
		// Embedded base class to store compressed or uncompressed animation export 
		// data for a mesh, across frames,  in the vertAnimExporter class.
		//========================================================================
		struct VertexAnimData
		{
			VertexAnimData( const std::string &i_Name, VertAnimExportIntent &i_Intent ):
		m_Name( i_Name ),
			m_Intent( i_Intent )
		{}
		virtual ~VertexAnimData(){}

		//BeginUpdateFrame();
		//for int i=0; i < nFrames; ++i )
		//{
		//Compute et vtxVertexFrame for the i-th frame
		//	UpdateFrame( &vtxVertexFrame, i);
		//}
		//EndUpdateFrame();

		//called before we add vertex data for each frame
		virtual void BeginUpdateFrame( ){};
		//add epoxrted anim data for the given frame
		virtual void UpdateFrame( vtxVertexFrame *i_pCurFrame, float i_Offset )=0;
		//celled at the end of the frame update
		virtual void FinishUpdateFrame( ){};

		//take away the data stored in the 
		//struct and use it cfo something
		//After tis operation, tyhe struct is not guaranteed to hold the data

		virtual void DepleteData()=0;

		//name of the mesh, whose data is stored
		const std::string m_Name;
		//reference to the export intent
		VertAnimExportIntent &m_Intent;
		};

		//derived class for storing uncompressed data
		struct VertexAnimDataUncompressed : public VertexAnimData
		{
			shared_ptr< vtxVertexAnimKeysStatic >  m_VertexAnim;
			shared_ptr< vtxVertexFramesStatic > m_VertexFrames;				
			~VertexAnimDataUncompressed()
			{
				int j=0;
			}
			VertexAnimDataUncompressed( const std::string &i_Name, VertAnimExportIntent &i_Intent );

			void UpdateFrame( vtxVertexFrame *i_pCurFrame, float i_Offset );

			void DepleteData();
		};

		//derived class for storing compressed data
		struct VertexAnimDataCompressed : public VertexAnimData
		{
			VertexAnimDataCompressed( const std::string &i_Name, VertAnimExportIntent &i_Intent );

			~VertexAnimDataCompressed()
			{
				int i=0;
			}			
			shared_ptr< vtxStreamCompression > m_CompressStream;

			void UpdateFrame( vtxVertexFrame *i_pCurFrame, float i_Offset );

			void FinishUpdateFrame( );

			void DepleteData();
			//given a seqeance of VertexAnimDataCompressed-s
			//write them to the chWriter
			static void WriteCompressedData( 
				chWriter & i_Writer, 
				ExportIntent &i_Intent,
				std::vector< shared_ptr< VertexAnimData > >::const_iterator &i_VitBegin, 
				std::vector< shared_ptr< VertexAnimData > >::const_iterator &i_VitEnd
				);
		};




		struct ReportProgress : public vtxVertexAnimWriter::WriteVertexAnimForMeshCallback
		{
			ReportProgress( ExportIntent &i_Intent ):
		m_Intent( i_Intent ){}
		void operator()(vtxVertexFrame *pVertexFrame )const;
		ExportIntent &m_Intent;
		};

		VertAnimExporter( ExportDoc &doc):
		BaseExporter( doc )
		{
		}
		~VertAnimExporter(){}
		// export the curent max node into the mdlNodeInfo node provided
		// i_pExportedRootNode, see BaseExporter.hpp for details
		void Export( 
			INode *i_pCurNode, 
			ExportIntent &i_Intent, 			
			INode *i_pExportedRootNode,
			shared_ptr<mdlNodeInfo> &o_MdlNodeInfo );
		float EstimateAnimBudget( INode *i_pCurNode );
		static const envType::UInt64 m_nMegabyte = 1024*1024;
		static const envType::UInt64 m_nMaxVertexCacheSize = static_cast< envType::UInt64 >( 4 )* 1024*1024*1024; // 1GB
		void CheckOnObject( INode  *i_pCurNode );

		void ApplyRTTransformation( const maMatrix4x4 &i_Tm, vtxVertexFrame  &io_VertexFrame );
		shared_ptr< VertexAnimData > m_VertexAnimData;
	};
} //namespace MaxExp