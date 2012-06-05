/****************************************************************************\
**  sgpuCameraAnimExportScene.cpp
**
**      sgpuCameraAnimExportScene.hpp defines the sgpuCameraAnimExportScene class
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuCameraAnimExportScene.hpp"
#include "sgpuBaseSceneImpl.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMaterialImpl.hpp"
#include "sgpuNode.hpp"
#include "sgpuNodeImpl.hpp"
#include "sgpuUtilsImpl.hpp"
#include "sgpuException.hpp"
#include "sgpuStringImpl.hpp"
#include "sgpuFileWriterLifeTimeKeeper.hpp"
#include "sgpuBinPseudoWriter.hpp"
#include "sgpuAnimUtils.hpp"

#include "Core/Env/envExceptionX.hpp"
#include "Core/Env/envString.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Core/ma/maAxisBox.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlWriter.hpp"
#include "Graphics/mdl/mdlReader.hpp"
#include "Graphics/mdl/mdlCameraAnimWriter.hpp"
#include <ctime>
#include <sstream>
#include <limits>
#if defined(max)
#undef max
#endif


//========================================================================
//========================================================================

namespace
{


	struct CameraAnimCallback : public mdlCameraAnimWriter::WriteCameraAnimCallback
	{
		CameraAnimCallback( sgpuReportProgress *i_pReport ):
			m_pReport( i_pReport )
			{
				DBG_ASSERT( (i_pReport != NULL), "i_pReport should be non-null" );
			}
		void operator()( float updateProgress ) const 
		{
			if( m_pReport)
			{
				(*m_pReport)( updateProgress );
			}
		
		}
	public:
		sgpuReportProgress *m_pReport;
	};

} //namespace anonymous


struct sgpuCameraAnimExportImpl
{
	sgpuCameraAnimExportImpl( float i_Estimate):
		m_Estimate( i_Estimate )
	{
	}	
private:

	sgpuCameraAnimExportImpl( const sgpuCameraAnimExportImpl &i_Other )
	   {
		   DBG_ASSERT( false, "cannot use copy constructor for sgpuCameraAnimEstimator"  );
	   }

	sgpuCameraAnimExportImpl &operator=( const sgpuCameraAnimExportImpl &i_Other )
	   {
		   DBG_ASSERT( false, "cannot use copy constructor for sgpuCameraAnimEstimator"  );
		   return *this;
	   }
public:

	std::deque< float > m_Keys;
	mdlCameraAnimWriter::CameraKeys m_CameraKeys;
	std::string m_CameraName;
	float		m_Estimate;

};


//========================================================================
//	default constructor
//========================================================================
sgpuCameraAnimExportScene::sgpuCameraAnimExportScene( 
	float i_FrameRate,
	float i_StartFrame,
	float i_EndFrame,
	float i_StepFrame
	):
m_FrameRate( i_FrameRate ),
m_StartFrame( i_StartFrame ),
m_EndFrame( i_EndFrame ),
m_StepFrame( i_StepFrame ),
m_pImpl( NULL )
{
		check_validity_of_anim_params
					(
					m_FrameRate,
					m_StartFrame,
					m_EndFrame,
					m_StepFrame
					);

	envType::UInt64 quickEstimate = gfFileBin::GetHeaderSize();
	quickEstimate += static_cast< envType::UInt64  > (  GetNumFrames() * mdlCameraAnimWriter::CameraKeys::EstimateSizeOfSingleFrame() );
	quickEstimate += 2 * 1024 ;//taking into account miscellaneous bytes required for chunk services

	m_pImpl = new sgpuCameraAnimExportImpl( static_cast< int > ( quickEstimate ) );
}

void sgpuCameraAnimExportScene::SetName( const sgpuString &i_CameraName )
{
	m_pImpl->m_CameraName = i_CameraName.m_pImpl->m_Data; 

}

void sgpuCameraAnimExportScene::SetFrame( int i_FrameIdx, const sgpuCameraFrame &i_CameraFrame )
{
	INVALID_RANGE_EXCEPTION( i_FrameIdx, GetNumFrames(), "CameraExport::SetFrame" )
	float fCurFrame =  ComputeIthFrame( i_FrameIdx );
	m_pImpl->m_CameraKeys.m_TranslateKeys[ fCurFrame ] = Point3d(i_CameraFrame.m_Translation);
	m_pImpl->m_CameraKeys.m_RotateKeys[ fCurFrame ] = Point3d( i_CameraFrame.m_Rotation );
	m_pImpl->m_CameraKeys.m_FocalLengthKeys[ fCurFrame ] = i_CameraFrame.m_FocalLength;
	m_pImpl->m_CameraKeys.m_CenterOfInterestKeys[ fCurFrame ] = i_CameraFrame.m_CenterOfInterest;
	m_pImpl->m_CameraKeys.m_HorizFilmAperKeys[ fCurFrame ] = i_CameraFrame.m_HorizFilmAperture;
	m_pImpl->m_CameraKeys.m_VertFilmAperKeys[ fCurFrame ] = i_CameraFrame.m_VertFilmAperture;
}


//Write the animation for a mesh, for a particular key
//Uses the (mesh-key , offset) table to  fill up the vertex and normal values
bool sgpuCameraAnimExportScene::WriteAnim(
	const sgpuString &i_Filename,
	sgpuReportProgress *i_pReportProgress 
	)
{
	shared_ptr< sgpuFileWriterLifeTimeKeeper > fileWriter( new sgpuFileWriterLifeTimeKeeper( *this, i_Filename ) );
	m_pBaseImpl->RegisterWriter( fileWriter );
	chBinWriter &writer = m_pBaseImpl->GetChBinWriter();
	
	//write_master_chunk_header( writer );
	write_fps( writer, m_FrameRate );
	write_begin_frame( writer, m_StartFrame );

	std::string scontext(m_pImpl->m_CameraName );
	scontext = scontext + std::string( "WriteAnim" );

	if( !m_pBaseImpl->HasRegisteredWriter() )
	{
		return false;
	}
	CameraAnimCallback wCamCallback( i_pReportProgress );
	WriteCameraAnim(
		writer,
		wCamCallback,		
		m_pImpl->m_CameraName,
		m_pImpl->m_CameraKeys,
		m_StartFrame
		);
	
	//writer.FinishChunk(); 
	// Stamp version into end of file
	write_exporter_version_stamp( writer );
	m_pBaseImpl->UnRegisterWriter();
	return true;
}

//========================================================================
//	destructor
//========================================================================
sgpuCameraAnimExportScene::~sgpuCameraAnimExportScene()
{
	delete m_pImpl;
}

float sgpuCameraAnimExportScene::GetEstimate() const
{
	return m_pImpl->m_Estimate;
}


sgpuCameraAnimExportScene::sgpuCameraAnimExportScene( const sgpuCameraAnimExportScene &i_Other ):
m_FrameRate(0.0f),
m_StartFrame(0.0f),
m_EndFrame(0.0f), 
m_StepFrame(0.0f)
{

	std::stringstream ss;
	ss << "copy constructor for sgpuCameraAnimExportScene not allowed";
	DBG_ASSERT( false, ss.str().c_str() );
}
sgpuCameraAnimExportScene & sgpuCameraAnimExportScene::operator=( const sgpuCameraAnimExportScene &i_Other )
{
	std::stringstream ss;
	ss << "assignment operator for sgpuCameraAnimExportScene not allowed";
	DBG_ASSERT( false, ss.str().c_str() );
	return *this;
}
