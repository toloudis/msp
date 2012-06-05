/*****************************************************************************
**  CameraAnimExporter.cpp
**
**	Exports materials
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "CameraAnimExporter.hpp"

#include "ExportDoc.hpp"
#include "ExportIntent.hpp"
#include "MaxExportUtils.hpp"
#include "MaxExportOptions.hpp"
#include "MaxMeshUtils.hpp"

#ifndef ENV_STRING_HPP
#include "Core/Env/envString.hpp"
#endif 
#include "Core/fs/fsLocator.hpp"
#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/fs/fsXMLWriter.hpp"
#ifndef MDL_CAMERAANIMWRITER_HPP
#include "Graphics/mdl/mdlCameraAnimWriter.hpp"
#endif

#include "MaxCommon.hpp"
#include "decomp.h"
#include <sstream>

using namespace std;


namespace MaxExp
{	

	//========================================================================
	//Result of exporting a single frame
	//========================================================================
	struct CameraAnimFrame
	{
		maMatrix4x4  m_Transform;
		float m_FocalLength;
		float m_CenterOfInterest;
		float m_HorizontalFilmAperture;
		float m_VerticalFilmAperture;

		CameraAnimFrame():
		m_FocalLength(0.0f),
			m_CenterOfInterest(0.0f),
			m_HorizontalFilmAperture(0.0f),
			m_VerticalFilmAperture(0.0f)
		{}
	};


	
	//========================================================================
	//A template parameter structure, 
	//like ResolvePolicyBindPose
	//used to govern how ObjectRefResolver 
	//resolves the object   correspinding
	//to a max node
	//========================================================================
	struct ResolvePolicyCameraAnim: public ResolvePolicyBase
	{
		static Object * Do( INode * i_pCurNode, ResolveModifier &detect, TimeValue i_Time );	
		static PolyOrTriMesh * MakePolyOrTriMesh( INode &i_CurNode, Object &i_MaxObject, bool i_bTriangulate );

	};

	Object *  ResolvePolicyCameraAnim::Do( INode * i_pCurNode, ResolveModifier &detect, TimeValue i_Time )
	{
		return ResolvePolicyVertAnim::Do( i_pCurNode, detect, i_Time );
	}

	PolyOrTriMesh * ResolvePolicyCameraAnim::MakePolyOrTriMesh( INode &i_CurNode, Object &i_MaxObject, bool i_bTriangulate )
	{
		return NULL;
	}

	//========================================================================
	//Export structure whose  duty is to
	//resolve the apropriate object state for a frame  
	//and do the core export
	//========================================================================
	class CameraAnimExportCore : public MeshBaseExportCore < ResolvePolicyCameraAnim >
	{

	public:
		CameraAnimExportCore( 
			ExportDoc &exportDoc, 
			INode &i_CurNode, 
			TimeValue i_ExportTime,	
			const maMatrix4x4 &i_GeometryTransform
			);
		~CameraAnimExportCore(){};	
		//export a single frame
		void DoExportFrame(  shared_ptr< CameraAnimFrame > &o_CameraAnimFrame );
	private:

		//From Max sdk sample code
		static const unsigned int PBLOCK_REF = 0;
		// Parameter block indices
		static const unsigned int PB_FOV =	0;
		static const unsigned int PB_TDIST = 1;
		static const unsigned int PB_HITHER	= 2;
		static const unsigned int PB_YON	= 3;
		static const unsigned int PB_NRANGE	= 4;
		static const unsigned int PB_FRANGE	= 5;
		static const unsigned int PB_MP_EFFECT_ENABLE =6;	// mjm - 07.17.00
		static const unsigned int PB_MP_EFF_REND_EFF_PER_PASS =7;	// mjm - 07.17.00
		static const unsigned int PB_FOV_TYPE =8;	// MC  - 07.17.07


		static float GetAspect() {
			return GetCOREInterface()->GetRendImageAspect();
		};
		static float GetApertureWidth() {
			return GetCOREInterface()->GetRendApertureWidth();
		}
	};

	//constructor
	CameraAnimExportCore::CameraAnimExportCore(  
		ExportDoc &exportDoc, 
		INode &i_CurNode, 
		TimeValue i_ExportTime,		
		const maMatrix4x4 &i_GeometryTransform
		):
	MeshBaseExportCore( 
		exportDoc, 
		i_CurNode, 
		i_ExportTime, 
		i_GeometryTransform,
		false)
	{	
	}


	//export a single frame
	void CameraAnimExportCore::DoExportFrame(  shared_ptr< CameraAnimFrame > &o_CameraAnimFrame )
	{	

		Matrix3 maxMat = SgpuConvert::MaxM3( m_VertTransform	);

		const MCHAR *szName = m_pCurNode->GetName();
		AffineParts ap;
		decomp_affine( maxMat, &ap);		
		float eulerAnglesXYZ[3];
		QuatToEuler( ap.q, eulerAnglesXYZ);

		o_CameraAnimFrame.reset( new CameraAnimFrame );
		o_CameraAnimFrame->m_Transform = m_VertTransform;




		//get other camera parameters

		CameraObject *pCameraObject = static_cast< CameraObject * > ( m_ObjResolver->GetMaxObject() );
		IParamBlock* parameters = (IParamBlock*) pCameraObject->GetReference( PBLOCK_REF);
		//fail if camera is ortho
		if ( pCameraObject->IsOrtho() )
		{
			EXPLOG.WriteError( "Orthogonal camera %s not exported!",  MBCSTOLPCSTR( szName ) );
			throw export_failure();
		}

		float fFov =  parameters->GetFloat( PB_FOV, m_ExportTime );
		Control *pFovController = parameters->GetController( PB_FOV );
		float fNear =  parameters->GetFloat( PB_HITHER, m_ExportTime );		
		Control *pNearController = parameters->GetController( PB_HITHER  );
		float fFar =  parameters->GetFloat( PB_YON, m_ExportTime );		
		Control *pFarController = parameters->GetController( PB_YON  );
		float fAspect = GetAspect();
		float hAperture = GetApertureWidth();
		if( EpsilonEqualZero( fFov ) || EpsilonEqualZero ( fAspect ) )
		{
			EXPLOG.WriteError( "Camera %s aspect ratio or fov is zero ", MBCSTOLPCSTR( szName ) );
			throw export_failure();
		}

		float focalLength = hAperture / (  2 *  tanf( fFov /2.0f ) );
		float vAperture = hAperture / fAspect;

		o_CameraAnimFrame->m_FocalLength = focalLength * 25.4f;
		o_CameraAnimFrame->m_HorizontalFilmAperture =  hAperture;
		o_CameraAnimFrame->m_VerticalFilmAperture = vAperture;
		o_CameraAnimFrame->m_CenterOfInterest = 0.0f;
		// Retrieve the camera target
		bool isTargeted = pCameraObject->ClassID().PartA() == LOOKAT_CAM_CLASS_ID;
		if( isTargeted )
		{
			//get the distance to the camera  target
			//as the centerofinterest
			INode* targetNode = m_pCurNode->GetTarget();
			assert( NULL != targetNode );
			//targets world TM 
			Matrix3 twtm = targetNode->GetNodeTM( m_ExportTime );
			Matrix3 wtm = m_pCurNode->GetNodeTM( m_ExportTime );
			Point3 distanceToTarget = twtm.GetTrans() - wtm.GetTrans();
			o_CameraAnimFrame->m_CenterOfInterest = distanceToTarget.Length();
		}
		//hyposthesis testing
		//not part of export		
		if( NULL != pFovController || NULL != pNearController || NULL != pFarController )
		{
			EXPLOG.WriteHypothesisTest( "Controller for one of the  camera %s parameters is not processeed!",  MBCSTOLPCSTR( szName ) ); 
		}
	}

	//given a general RT transform, get the
	//translation and rotation
	void CameraAnimExporter::GetTranslationAndRotation(const  maMatrix4x4 &i_Transform, maVector3d &o_Translation, maVector3d & o_Rotation )
	{
		AffineParts ap;

		Matrix3 maxMat = SgpuConvert::MaxM3( i_Transform );

		decomp_affine( maxMat, &ap);		
		float eulerAnglesXYZ[3];
		QuatToEuler( ap.q, eulerAnglesXYZ);

		o_Translation = SgpuConvert::Vec3(ap.t);
		Point3 rot( eulerAnglesXYZ[0], eulerAnglesXYZ[1], eulerAnglesXYZ[2] );
		o_Rotation = SgpuConvert::Vec3( rot );


		Quat stretchRotation =  ap.u;
		maVector4d stretchRotation_1( stretchRotation.x, stretchRotation.y, stretchRotation.z, stretchRotation.w );
		maVector4d stretchRotation_2( 0.0f, 0.0f, 0.0f, 1.0f );
		maVector4d stretchRotation_3 = stretchRotation_1 - stretchRotation_2;
		bool bStretchRotationIsUnitQuat = EpsilonEqualZero( stretchRotation_3 , 0.01f);

		Point3 stretchFactors = ap.k;
		maVector3d stretchFactors_1 ( stretchFactors[0], stretchFactors[1], stretchFactors[2] );
		maVector3d stretchFactors_2 (1.0f, 1.0f, 1.0f );
		maVector3d stretchFactors_3 = stretchFactors_1 - stretchFactors_2;
		EpsilonEqualZero( stretchFactors_3 );
		bool bStretchFactorsIsZero = EpsilonEqualZero( stretchFactors_3, 0.01f );

		if( !bStretchRotationIsUnitQuat )
		{
			EXPLOG.WriteHypothesisTest( "Camera Matrix has non-unit stretch rotation ");
		}	
		if( !bStretchFactorsIsZero )
		{
			EXPLOG.WriteHypothesisTest( "Camera Matrix has non-unit stretch factors ");
		}	
		if( ap.f < 0.0f )
		{
			EXPLOG.WriteHypothesisTest( "Camera Matrix has negative determinant ");
		}	
	}




	//extimate the animation budget
	float CameraAnimExporter::EstimateAnimBudget( INode *i_pCurNode )
	{
		TimeValue startTime = OPTS.m_StartTime;
		TimeValue endTime = OPTS.m_EndTime;
		TimeValue stepTime = OPTS.m_StepTime;
		assert( stepTime > 0.0f);
		float startFrame = OPTS.m_fStartFrame;
		float endFrame = OPTS.m_fEndFrame;
		float stepFrame =  OPTS.m_fStepFrame;

		bool bTriangulate = false;

		//no need to do this
		ObjectRefResolver< ResolvePolicyCameraAnim > objRefResolve ( *m_pExportDoc,  i_pCurNode,  startTime, bTriangulate );


		int numKeys = static_cast<int> ( ( endFrame - startFrame ) / stepFrame ) + 1;
		float animBudgetInBytes = numKeys * mdlWriter::CameraKeys::EstimateSizeOfSingleFrame();
		return animBudgetInBytes;
	}



	//=============================================================================
	// Public function to do the export	 
	// 
	//=============================================================================
	// i_pExportedRootNode, please see BaseExporter.hpp for details
	void CameraAnimExporter::Export(
		INode *i_pCurNode, 
		ExportIntent &i_Intent,					  
		INode *i_pExportedRootNode,
		shared_ptr<mdlNodeInfo> &o_MdlNodeInfo )
	{			

		MCHAR *szNodeName = i_pCurNode->GetName(); 
		ExportLogger::WriteStartElementWrap explogWrap( "exporting: %s", MBCSTOLPCSTR( szNodeName ) ); 			
		CameraAnimExporter::ReportProgress rprogress( i_Intent );

		TimeValue startTime = OPTS.m_StartTime;
		TimeValue endTime = OPTS.m_EndTime;
		TimeValue stepTime = OPTS.m_StepTime;
		assert( stepTime > 0.0f);
		float startFrame = OPTS.m_fStartFrame;
		float endFrame = OPTS.m_fEndFrame;
		float stepFrame =  OPTS.m_fStepFrame;		
		float sampleFrame;
		int nKeys = static_cast<int> ( ( (endFrame - startFrame) / stepFrame ) + 1 );
#if defined(_DEBUG)
		int nKeys_debug=0;
		for( sampleFrame = startFrame, nKeys_debug=0; sampleFrame <= endFrame; sampleFrame += stepFrame, ++nKeys_debug );
		assert( nKeys == nKeys_debug );
#endif

		maMatrix4x4 requiredWorldTransformForGeometry;	
		m_CameraExportData.reset< CameraKeysWithTransform  >( NULL );

		if(  nKeys > 0 )
		{	
			m_CameraExportData.reset<CameraKeysWithTransform> ( new CameraKeysWithTransform () );
			m_CameraExportData->m_RotationOrder = 0;

			float singleFrameSize = mdlWriter::CameraKeys::EstimateSizeOfSingleFrame();
			for( sampleFrame = startFrame; sampleFrame <= endFrame; sampleFrame += stepFrame )
			{
				TimeValue sampleTime = static_cast< TimeValue > ( sampleFrame * GetTicksPerFrame() );				

				requiredWorldTransformForGeometry = CalculateRequiredWorldTransformForGeometry(
					*m_pExportDoc,
					i_pCurNode,
					i_pExportedRootNode,
					sampleTime
					);	
				CameraAnimExportCore camAnimExport( 
					*m_pExportDoc, 
					*i_pCurNode, 
					sampleTime, 
					requiredWorldTransformForGeometry
					);

				shared_ptr< CameraAnimFrame > camFrame;
				camAnimExport.DoExportFrame( camFrame );	
				if( camFrame.get() )
				{

					Insert( i_pCurNode, *m_CameraExportData, camFrame, sampleFrame );
					rprogress( singleFrameSize );
				}
				//progress report
			}	
			ApplyRTTransformation( ExportIntent::m_ZaxisUpToYaxisUp, m_CameraExportData );
		}
	}		


	//insert a single cameraanimframe to the CameraKeys	
	void CameraAnimExporter::Insert(INode *i_pCurNode, CameraKeysWithTransform &io_CameraKeys, shared_ptr<CameraAnimFrame > &i_CameraAnimFrame,  float i_CurFrame )
	{	
		float fCurFrame = static_cast< float > ( i_CurFrame );
		io_CameraKeys.m_TransformKeys[ fCurFrame ] =  i_CameraAnimFrame->m_Transform;
		maVector3d translation;
		maVector3d rotation;
		GetTranslationAndRotation( i_CameraAnimFrame->m_Transform, translation,  rotation );	
		io_CameraKeys.m_TranslateKeys[ fCurFrame ] = translation;
		io_CameraKeys.m_RotateKeys[ fCurFrame ]= rotation;
		io_CameraKeys.m_FocalLengthKeys[ fCurFrame ] = i_CameraAnimFrame->m_FocalLength;
		io_CameraKeys.m_CenterOfInterestKeys[ fCurFrame ] = i_CameraAnimFrame->m_CenterOfInterest;
		io_CameraKeys.m_HorizFilmAperKeys[ fCurFrame ] = i_CameraAnimFrame->m_HorizontalFilmAperture;
		io_CameraKeys.m_VertFilmAperKeys[ fCurFrame ] = i_CameraAnimFrame->m_VerticalFilmAperture;
#if defined(_DEBUG)
#define SGPU_DEBUG 1
#if SGPU_DEBUG
		{
			_RPT4( _CRT_WARN, "camera  frame %g has pos %g %g %g\n", i_CurFrame, translation[0], translation[1], translation[2] );
			_RPT4( _CRT_WARN, "camera  frame %g has rot %g %g %g %g\n", i_CurFrame, rotation[0], rotation[1], rotation[2]  );
		}
#endif
#endif

	}

	//progress report update
	void CameraAnimExporter::ReportProgress::operator()(float update )const
	{
		m_Intent.ReportProgress( update );
	}

	//apply the RT transform on the CameraKey
	void CameraAnimExporter::CameraKeysWithTransform::ApplyRTTransform ( const maMatrix4x4 &i_Tm )
	{
		map<float, maMatrix4x4>::iterator mit;
		for( mit = m_TransformKeys.begin(); mit != m_TransformKeys.end(); ++mit )
		{
			//for each transform key,
			//get the transform
			maMatrix4x4 &transM = mit->second;
			//apply the input RTtransform
			transM = transM * i_Tm;
			//get the frame
			float fkey = mit->first;
			maVector3d trans;
			maVector3d rot;
			//decompose the resulting transform into trans and rot
			GetTranslationAndRotation(  transM, trans, rot );
			//erase the translate key corresponding
			//to the current frame
			size_t nErased = m_TranslateKeys.erase( fkey );
			if( nErased != 1 )
			{
				EXPLOG.WriteError( "transform key doesnt correspond to translate key" );
				throw export_failure();
			}
			//update the translate key
			m_TranslateKeys[ fkey ] = trans;
			//erase the rotate key corresponding to
			//the current frame
			nErased = m_RotateKeys.erase( fkey );
			if ( nErased != 1 )
			{
				EXPLOG.WriteError( "transform key doesnt correspond to rotate key" );
				throw export_failure ();
			}
			//update the rotate key
			m_RotateKeys [fkey ] = rot;
		}
	}

	//apply an RT transform on the  cameraKys	
	void CameraAnimExporter::ApplyRTTransformation( const maMatrix4x4 &i_Tm, shared_ptr< CameraKeysWithTransform > &i_CameraAnimKeys )
	{	
		std::vector<maPoint3d>::iterator vit;
		float  det = i_Tm.GetDeterminant();
		if( det < 0.0f )
		{
			int i=0;
		}	
		maVector3d trans = i_Tm.GetTranslation();
		assert ( EpsilonEqualZero( trans[0] ) && EpsilonEqualZero( trans[1] ) &&  EpsilonEqualZero( trans[2] ) );		
		i_CameraAnimKeys->ApplyRTTransform( i_Tm );
	}


}// namespace MaxExp