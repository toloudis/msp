/****************************************************************************\
**  sgpuVertexAnimExportScene.cpp
**
**      sgpuVertexAnimExportScene.hpp defines the sgpuVertexAnimExportScene class
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuVertexAnimExportScene.hpp"
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
#include <ctime>
#include <sstream>
#include <limits>

#if defined(max)
#undef max
#endif


//========================================================================
// There is version information here and version information
// in the Resources sgpuExportLib.rc and in the ReadMe.txt
//========================================================================

//========================================================================
//========================================================================


struct sgpuVertexAnimEstimatorImpl
{

	struct MeshOffset
	{
		MeshOffset( std::string &i_MeshName, int i_NumKeys):
	m_BeginOffset(0),
		m_EndOffset(0),
		m_KeyOffsets( new std::deque< int > ( i_NumKeys, 0) ),
		m_Name( i_MeshName ),
		m_bHasWrittenMeshName( false )
	{}

	int m_BeginOffset;
	int m_EndOffset;
	shared_ptr< std::deque< int > > m_KeyOffsets;
	std::string m_Name;
	bool m_bHasWrittenMeshName;
	};

	sgpuVertexAnimEstimatorImpl():
	m_EndOffset( 0 ),
		m_Writer( gfFileBin::GetHeaderSize() )
	{
	}	
	typedef std::deque< MeshOffset >  TMeshOffsets;
private:

	sgpuVertexAnimEstimatorImpl( const sgpuVertexAnimEstimatorImpl &i_Other ):
	   m_EndOffset(0),
		   m_Writer( gfFileBin::GetHeaderSize() )
	   {
		   DBG_ASSERT( false, "cannot use copy constructor for sgpuVertAnimEstimator"  );
	   }

	   sgpuVertexAnimEstimatorImpl &operator=( const sgpuVertexAnimEstimatorImpl &i_Other )
	   {
		   DBG_ASSERT( false, "cannot use copy constructor for sgpuVertAnimEstimator"  );
		   return *this;
	   }
public:
	sgpuBinPseudoWriter m_Writer;
	std::deque< float > m_Keys;
	TMeshOffsets m_MeshOffsets;
	int m_EndOffset;
};

sgpuVertexAnimEstimator::sgpuVertexAnimEstimator(		 
	float i_FrameRate, 
	float i_StartFrame, 
	float i_EndFrame, 
	float i_StepFrame
	):
m_FrameRate( i_FrameRate ),
m_StartFrame( i_StartFrame ),
m_EndFrame( i_EndFrame ),
m_StepFrame( i_StepFrame ),
m_pImpl( new sgpuVertexAnimEstimatorImpl )
{
	
		check_validity_of_anim_params
					(
					m_FrameRate,
					m_StartFrame,
					m_EndFrame,
					m_StepFrame
					);

	write_master_chunk_header( m_pImpl->m_Writer );
	float dummyFps = 0;
	write_fps( m_pImpl->m_Writer, dummyFps);
	write_begin_frame( m_pImpl->m_Writer, i_StartFrame );


	int nKeys = GetNumFrames();
	m_pImpl->m_Keys.resize( nKeys );
	int nextKeyIdx=0;
	for (float i=i_StartFrame; i<=i_EndFrame; i+=i_StepFrame, ++nextKeyIdx )
	{
		m_pImpl->m_Keys[nextKeyIdx] = i;
	}
}


int sgpuVertexAnimEstimator::AddEstimate( 
	const sgpuString &i_MeshName, 
	 bool i_bSubdiv,
	int i_NumVerts, 
	int i_NumNormals )
{
	//vertex animations for all keys for a frame
	CharacterBuffer< 256 > cbuffer( i_MeshName.GetNumBytes_UTF8() );
	i_MeshName.GetData_UTF8( reinterpret_cast< char * > ( cbuffer.GetBuffer() ), cbuffer.GetSize() );
	int meshOffset = m_pImpl->m_Writer.GetCurFilePos();
	envType::UInt64 quickEstimate = static_cast< envType::UInt64 > (m_pImpl->m_Writer.GetCurFilePos() );
	quickEstimate += static_cast< envType::UInt64 > ( ( !i_bSubdiv ) ? ( i_NumVerts  + i_NumNormals ) * 12 : i_NumVerts * 12 );	
	quickEstimate += 2 * 1024 ;//taking into account miscellaneous bytes required for chunk services
	const char * pzMeshName = reinterpret_cast< const char * > ( cbuffer.GetBuffer() );
	if( quickEstimate >= maxAnimationBudget )
	{
		std::stringstream ss;
		ss << "estimated vertex animation for mesh: " << reinterpret_cast< const char * > ( cbuffer.GetBuffer() ) ;
		ss << " exceedes the budget of " << maxAnimationBudget;
		throw sgpuException( sgpuString( ss.str().c_str() ) );
		return 0;
	}
	sgpuVertexAnimEstimatorImpl::MeshOffset mo( std::string( pzMeshName ), m_pImpl->m_Keys.size() );
	int beginFilePos = m_pImpl->m_Writer.GetCurFilePos();
	m_pImpl->m_Writer.WriteChunkHeader(c_VTXA, 0, true);
	//name of the mesh
	m_pImpl->m_Writer.WriteChunkHeader(c_NNAM, 0, false);
	m_pImpl->m_Writer.Write( reinterpret_cast< char * > ( cbuffer.GetBuffer() ) ); 
	m_pImpl->m_Writer.FinishChunk();

	//no normals are written if the mesh is a subdiv
	bool bWriteNormals = !i_bSubdiv;
	//offset of the various keys in the file
	std::deque< int > &keyOffsets = *(mo.m_KeyOffsets);

	float fFrameOffset = 0;	
	//dummy data to write
	maPoint3d  dummyPt;
	maAxisBox dummyBBox;
	std::deque< float >::const_iterator keyIt;
	int keyIdx=0;
	for ( keyIt = m_pImpl->m_Keys.begin(); keyIt != m_pImpl->m_Keys.end(); ++keyIt, ++keyIdx )
	{
		float frame = (*keyIt);
		//record the offset for this frame
		keyOffsets[ keyIdx ] = m_pImpl->m_Writer.GetCurFilePos();
		write_frame_for_mesh(
			m_pImpl->m_Writer,
			0, 
			i_bSubdiv,
			i_NumVerts,
			i_NumNormals,
			NULL,
			NULL);	
	}	
	int endFilePos = m_pImpl->m_Writer.GetCurFilePos();
	mo.m_BeginOffset = beginFilePos;
	mo.m_EndOffset = endFilePos;
	if( m_pImpl->m_MeshOffsets.size() >= std::numeric_limits<int>::max() )
	{
		std::stringstream ss;
		ss << "cannot add mesh " << reinterpret_cast< const char * > ( cbuffer.GetBuffer() );
		ss << " to estimate, and consequently cannot export, because the number of meshes exceeds integer limit: "; 
		throw sgpuException( sgpuString( ss.str().c_str() ) );
		return 0;
	}
	m_pImpl->m_EndOffset = endFilePos;
	m_pImpl->m_MeshOffsets.push_back( mo );
	m_pImpl->m_Writer.FinishChunk();
	return static_cast< int > ( m_pImpl->m_MeshOffsets.size() );
}

float sgpuVertexAnimEstimator::GetEstimate()
{
	return static_cast< float > ( m_pImpl->m_EndOffset );
}

	int sgpuVertexAnimEstimator::GetNumMeshesInEstimate() const
	{
		return  static_cast< int > ( m_pImpl->m_MeshOffsets.size() );
	}

	void sgpuVertexAnimEstimator::GetDetailOfIthMesh( 
		int i_MeshIdxInEstimate, 
		sgpuString &o_MeshName, 
		int &o_BeginOffsetInAnimFile, 
		int &o_EndOffsetInAnimFile )
	{
		int nMeshInEstimateCurrently = static_cast<int> ( m_pImpl->m_MeshOffsets.size() );
		if ( i_MeshIdxInEstimate >= static_cast<int> ( m_pImpl->m_MeshOffsets.size() ) )
		{
			std::stringstream ss;
			ss <<  "index " << i_MeshIdxInEstimate;
			ss << " exceeds the number of meshes currently estimated " << nMeshInEstimateCurrently;
			throw sgpuException( sgpuString( ss.str().c_str() ) );
		}
		sgpuVertexAnimEstimatorImpl::MeshOffset &offset = m_pImpl->m_MeshOffsets[ i_MeshIdxInEstimate ];
		o_MeshName = sgpuString(offset.m_Name.c_str() );
		o_BeginOffsetInAnimFile = offset.m_BeginOffset;
		o_EndOffsetInAnimFile = offset.m_EndOffset;
		return;
	}


	sgpuVertexAnimEstimator::sgpuVertexAnimEstimator( const sgpuVertexAnimEstimator &i_Other ):
		m_FrameRate(0),
		m_StartFrame(0),
		m_EndFrame(0),
		m_StepFrame(0)
	{
		std::stringstream ss;
		ss << "copy constructor for sgpuVertexAnimEstimator not allowed";
		DBG_ASSERT( false, ss.str().c_str() );
	}
	sgpuVertexAnimEstimator & sgpuVertexAnimEstimator::operator=( const sgpuVertexAnimEstimator &i_Other )
	{
		std::stringstream ss;
		ss << "assignment operator for sgpuVertexAnimEstimator not allowed";
		DBG_ASSERT( false, ss.str().c_str() );
		return *this;
	}
//========================================================================
//	default constructor
//========================================================================
sgpuVertexAnimExportScene::sgpuVertexAnimExportScene( 
	float i_FrameRate,
	float i_StartFrame,
	float i_EndFrame,
	float i_StepFrame
	):
	m_Estimator( i_FrameRate, i_StartFrame, i_EndFrame, i_StepFrame )
{

}
bool sgpuVertexAnimExportScene::OpenAndBeginWriting ( sgpuString & i_Filename )
{
	sgpuVertexAnimEstimatorImpl *pVAnimEstImpl = m_Estimator.m_pImpl;
	shared_ptr< sgpuFileWriterLifeTimeKeeper > fileWriter( new sgpuFileWriterLifeTimeKeeper( *this, i_Filename ) );
	m_pBaseImpl->RegisterWriter( fileWriter );
	chBinWriter &writer = m_pBaseImpl->GetChBinWriter();
	write_master_chunk_header( writer );
	write_fps( writer, m_Estimator.m_FrameRate );
	write_begin_frame( writer, m_Estimator.m_StartFrame );
	return true;
}
//Write the animation for a mesh, for a particular key
//Uses the (mesh-key , offset) table to  fill up the vertex and normal values
bool sgpuVertexAnimExportScene::WriteAnim(
	int i_IthGeom, 
	const sgpuString &i_MeshName, 
	int i_JthKey, 
	bool i_bSubdiv, 
	int i_NumVerts,
	int i_NumNormals,
	const sgpuVector3 *i_pPositions, 
	const sgpuVector3 *i_pNormals,
	sgpuReportProgress *i_pReportProgress 
	)
{

	CharacterBuffer< 256 > cbuffer( i_MeshName.GetNumBytes_UTF8() );
	i_MeshName.GetData_UTF8( reinterpret_cast< char * > ( cbuffer.GetBuffer() ), cbuffer.GetSize() );
	std::string scontext(reinterpret_cast< char * > ( cbuffer.GetBuffer() ) );
	scontext = scontext + std::string( "WriteAnim" );

	sgpuVertexAnimEstimatorImpl *pVAnimEstImpl = m_Estimator.m_pImpl;
	if( i_IthGeom >= static_cast< int > ( pVAnimEstImpl->m_MeshOffsets.size() ) )
	{
		INVALID_RANGE_EXCEPTION( i_IthGeom, pVAnimEstImpl->m_MeshOffsets.size(), scontext.c_str() );
	}
	if( !m_pBaseImpl->HasRegisteredWriter() )
	{
		return false;
	}
	sgpuVertexAnimEstimatorImpl::MeshOffset & meshOffset = pVAnimEstImpl->m_MeshOffsets[ i_IthGeom ];
	chBinWriter &writer = m_pBaseImpl->GetChBinWriter();
	gfFileBin &outputFile = m_pBaseImpl->GetBinFile();
	DBG_ASSERT(m_pBaseImpl->HasRegisteredWriter(), "writer not registered yet");
	if( !meshOffset.m_bHasWrittenMeshName )
	{
		outputFile.SetFilePos( meshOffset.m_BeginOffset );
		writer.WriteChunkHeader(c_VTXA, 0, true);
		//name of the mesh
		writer.WriteChunkHeader(c_NNAM, 0, false);
		writer.Write( reinterpret_cast< char * > ( cbuffer.GetBuffer() ) ); 
		writer.FinishChunk();
	}
	INVALID_RANGE_EXCEPTION( i_JthKey,  meshOffset.m_KeyOffsets->size(), scontext.c_str() )
		DBG_ASSERT( ((*meshOffset.m_KeyOffsets)[ i_JthKey ] >= 0), "mesh offset for key: " << i_JthKey << " for mesh: " << scontext.c_str() << " not correct" );
	outputFile.SetFilePos( (*meshOffset.m_KeyOffsets)[ i_JthKey ] );
	float jThKeyFrame = m_Estimator.ComputeIthFrame( i_JthKey );
	int curFilePosBeforeWritingFrameForTheMesh = static_cast< int > ( outputFile.GetFilePos() );
	write_frame_for_mesh( 
		writer, 
		jThKeyFrame - m_Estimator.m_StartFrame, 
		i_bSubdiv,  
		i_NumVerts, 
		i_NumNormals, 
		i_pPositions, 
		i_pNormals );
	int curFilePosAfterWritingFrameForTheMesh = static_cast< int > ( outputFile.GetFilePos() );	
	if( i_pReportProgress  )
	{
		DBG_ASSERT( (curFilePosAfterWritingFrameForTheMesh >= curFilePosBeforeWritingFrameForTheMesh), "inconsistent file writing, while vertex anim export for mesh: " << scontext.c_str() );
		(*i_pReportProgress)( static_cast< float > ( curFilePosAfterWritingFrameForTheMesh - curFilePosBeforeWritingFrameForTheMesh ) );
	}
	if( !meshOffset.m_bHasWrittenMeshName )
	{
		outputFile.SetFilePos( meshOffset.m_EndOffset );
		writer.FinishChunk();
		meshOffset.m_bHasWrittenMeshName = true;
	}
	return true;
}
bool sgpuVertexAnimExportScene::CloseAndEndWriting()
{
	if( !m_pBaseImpl->HasRegisteredWriter() )
	{
		return false;
	}
	sgpuVertexAnimEstimatorImpl *pVAnimEstImpl = m_Estimator.m_pImpl;
	chBinWriter &writer = m_pBaseImpl->GetChBinWriter();	
	gfFileBin &outputFile = m_pBaseImpl->GetBinFile();
	outputFile.SetFilePos( pVAnimEstImpl->m_EndOffset);
	writer.FinishChunk(); //C_ACHR)
	// Stamp version into end of file
	write_exporter_version_stamp( writer );
	m_pBaseImpl->UnRegisterWriter();
	return true;
}

//========================================================================
//	destructor
//========================================================================
sgpuVertexAnimExportScene::~sgpuVertexAnimExportScene()
{
	m_pBaseImpl->UnRegisterWriter();
}


sgpuVertexAnimExportScene::sgpuVertexAnimExportScene( const sgpuVertexAnimExportScene &i_Other ):
	m_Estimator(0,0,0,0)
{

	std::stringstream ss;
	ss << "copy constructor for sgpuVertexAnimExportScene not allowed";
	DBG_ASSERT( false, ss.str().c_str() );
}
sgpuVertexAnimExportScene & sgpuVertexAnimExportScene::operator=( const sgpuVertexAnimExportScene &i_Other )
{
	std::stringstream ss;
	ss << "assignment operator for sgpuVertexAnimExportScene not allowed";
	DBG_ASSERT( false, ss.str().c_str() );
	return *this;
}