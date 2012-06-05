/*****************************************************************************
**  CameraAnimExporter.hpp
**
**	Exports cameras
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MAXEXP_CAMERAANIMEXPORTER_HPP
#error MAXEXP_CAMERAANIMEXPORTER_HPP multuply defined!!
#endif
#define MAXEXP_CAMERAANIMEXPORTER_HPP

#ifndef MAXEXP_BASEEXPORTER_HPP
#include "BaseExporter.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef MDL_CAMERAANIMWRITER_HPP
#include "Graphics/mdl/mdlCameraAnimWriter.hpp"
#endif
#include "MaxCommon.hpp"
#include <list>
#include <max.h>
#include <hash_set>
#include <map>


//forward declarations
namespace MaxExp
{
	struct CameraAnimFrame;
	class ExportIntent;
}

namespace MaxExp
{	

	//========================================================================
	// Camera Exporter Class
	//	
	//========================================================================
	class CameraAnimExporter: public BaseExporter
	{
	public:

		//CameraKeys constitute the decomposed ( translation + rotation)
		//version of the camera transform.
		//But we  need the original matrix version of th transform for
		//verious post procssing stages,
		//Hence the necessity of this embedded struct
		struct CameraKeysWithTransform : public mdlWriter::CameraKeys
		{
			std::map< float, maMatrix4x4> m_TransformKeys;
			//apply the RT transform on the CameraKey
			void ApplyRTTransform ( const maMatrix4x4 &i_Tm );
		};


		//Callback structure used in camera export writing
		struct ReportProgress : public mdlWriter::WriteCameraAnimCallback
		{
			ReportProgress( ExportIntent &i_Intent ):
				m_Intent( i_Intent ){}
			//progress report update
			void operator()(float update )const;
			ExportIntent &m_Intent;
		};


	public:
		//constructor
		CameraAnimExporter( ExportDoc &io_doc):
		  BaseExporter(io_doc){}
		  //destructor
		  ~CameraAnimExporter(){}
		  //core export
		  void Export( 
			  INode *i_pCurNode, 
			  ExportIntent &i_Intent, 
			  INode *i_pExportedRootNode,
			  shared_ptr<mdlNodeInfo> &o_MdlNodeInfo );
		  //extimate the animation budget
		  float  EstimateAnimBudget( INode *i_pCurNode );
		  //animation budget limit
		  static const envType::UInt64 m_nMaxCameraAnimCacheSize = 1024*1024*1024; // 1GB
		  //result of the camera export
		  shared_ptr< CameraKeysWithTransform > m_CameraExportData;
	private:
		//given a general RT transform, get the
		//translation and rotation
		static void GetTranslationAndRotation( const maMatrix4x4 &i_Transform, maVector3d &o_Translation, maVector3d & o_Rotation );
		//insert a single cameraanimframe to the CameraKeys
		void Insert(INode *i_pCurNode,  CameraKeysWithTransform &io_CameraKeys, shared_ptr<CameraAnimFrame > &i_CameraAnimFrame,  float i_CurFrame );
		//apply an RT transform on the  cameraKys
		void ApplyRTTransformation( const maMatrix4x4 &i_Tm, shared_ptr< CameraKeysWithTransform > &i_CameraAnimKeys );

	};
} //namespace MaxExp