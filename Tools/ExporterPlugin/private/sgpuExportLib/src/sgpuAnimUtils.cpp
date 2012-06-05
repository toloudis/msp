/****************************************************************************\
**  sgpuAnimUtils.cpp
**
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "sgpuAnimUtils.hpp"
#include "sgpuVector.hpp"
#include "sgpuUtilsImpl.hpp"
#include "sgpuException.hpp"

#include <crtdbg.h>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <ctime>
#include <limits>

#include "Core/It/itStringUtil.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ma/maAxisBox.hpp"

#if defined(max)
#undef max
#endif
const chDefs::Name c_ACHR = chDefs::MakeName('A', 'C', 'H', 'R');
const chDefs::Name c_VTXA = chDefs::MakeName('V', 'T', 'X', 'A');
const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
const chDefs::Name c_FRAM = chDefs::MakeName('F', 'R', 'A', 'M');
const chDefs::Name c_TIME = chDefs::MakeName('T', 'I', 'M', 'E');
const chDefs::Name c_GVER = chDefs::MakeName('G', 'V', 'E', 'R');
const chDefs::Name c_NVER = chDefs::MakeName('N', 'V', 'E', 'R');
const chDefs::Name c_BBOX = chDefs::MakeName('B', 'B', 'O', 'X');
const chDefs::Name c_AFPS = chDefs::MakeName('A', 'F', 'P', 'S');
const chDefs::Name c_BGFR = chDefs::MakeName('B', 'G', 'F', 'R');
const chDefs::Name c_EXPV = chDefs::MakeName('E', 'X', 'P', 'V');

const envType::UInt64 maxAnimationBudget = 2 * static_cast< envType::UInt64 > ( 1024 * 1024 * 1024 );
void write_master_chunk_header( chWriter &io_Writer )
{
	io_Writer.WriteChunkHeader(c_ACHR, 0, true);
}

void write_master_chunk_finish( chWriter &io_Writer )
{
	io_Writer.FinishChunk();
}

void write_fps( chWriter &io_Writer, float fps )
{
	io_Writer.WriteChunkHeader(c_AFPS, 0, false);
	io_Writer.Write( fps );
	io_Writer.FinishChunk();
}

void write_begin_frame( chWriter &io_Writer, float beginFrame )
{
	io_Writer.WriteChunkHeader(c_BGFR, 0, false);
	io_Writer.Write( beginFrame );
	io_Writer.FinishChunk();
}

void write_bbox( chWriter &io_Writer, const maAxisBox &bbox )
{
	io_Writer.WriteChunkHeader(c_BBOX, 0, false);
	maPoint3d min_pt(bbox.GetMinX(), bbox.GetMinY(), bbox.GetMinZ());
	maPoint3d max_pt(bbox.GetMaxX(), bbox.GetMaxY(), bbox.GetMaxZ());
	chChunkParserUtil::Write( io_Writer, min_pt);
	chChunkParserUtil::Write( io_Writer, max_pt);
	io_Writer.FinishChunk();
}

void write_frame_for_mesh(
						  chWriter &io_Writer, 
						  float i_fFrameOffset, 
						  bool i_bSubdiv,  
						  int i_NumVerts, 
						  int i_NumNormals,
						  const sgpuVector3 *i_pPositions, 
						  const sgpuVector3 *i_pNormals  )
{
	bool bWriteNormals = !i_bSubdiv;
	io_Writer.WriteChunkHeader( c_FRAM, 0, true );
	//record the time
	io_Writer.WriteChunkHeader( c_TIME, 0, false );
	io_Writer.Write( i_fFrameOffset);
	io_Writer.FinishChunk();
	//record the vertices
	io_Writer.WriteChunkHeader( c_GVER,1, false );
	io_Writer.Write( envType::Int32(i_NumVerts) );

	std::vector< maPoint3d > maPositions(1 );

	maAxisBox bbox;
	if( NULL != i_pPositions && i_NumVerts > 0 )
	{
		maPositions.resize( i_NumVerts );
		for( int i=0; i < i_NumVerts; ++i )
		{
			maPositions[i] = ( Point3d( i_pPositions[ i ] ) );
			bbox.Union( maPositions[ i ] );
		}
	}

	if( i_NumVerts > 0 )
	{
		io_Writer.Write(&maPositions[0], sizeof( maPoint3d ) * i_NumVerts );
	}
	io_Writer.FinishChunk();
	//record the normals
	if( bWriteNormals )
	{
		io_Writer.WriteChunkHeader(c_NVER, 1, false);
		io_Writer.Write(envType::Int32(i_NumNormals));
		if ( i_NumNormals > 0)
		{
			std::vector< maPoint3d > maNormals( 1 );
			if( NULL != i_pNormals )
			{
				maNormals.resize( i_NumNormals );
				for( int i=0; i < i_NumNormals; ++i )
				{
					maNormals[i] = ( Point3d( i_pNormals[ i ] ) );
				}
			}
			io_Writer.Write(&maNormals[0], i_NumNormals*sizeof(maPoint3d));
		}
		io_Writer.FinishChunk();
	}
	_RPT4(_CRT_WARN, "max bbox for frame ",\
		static_cast<float>(i_fFrameOffset),\
		bbox.GetMaxX(),\
		bbox.GetMaxY(),\
		bbox.GetMaxZ());
	
	_RPT4(_CRT_WARN, "min bbox for frame ",\
		static_cast<float>(i_fFrameOffset),\
		bbox.GetMinX(),\
		bbox.GetMinY(),\
		bbox.GetMinZ());

	if( NULL != i_pPositions )
	{
		if( !bbox.IsEmpty() )
		{
			write_bbox( io_Writer, bbox );
		}
	} else
	{
		write_bbox( io_Writer, bbox );
	}

	io_Writer.FinishChunk();
}
void write_exporter_version_stamp(chWriter &io_Writer)
{
	char date_string[64];
	char time_string[64];
	_strdate_s(date_string);
	_strtime_s(time_string);

	// Write as strings so readable from bin viewer
	//
	io_Writer.WriteChunkHeader(c_EXPV, 0, false);
	io_Writer.Write(c_ExportLib_Version);
	io_Writer.Write(date_string);
	io_Writer.Write(time_string);
	io_Writer.FinishChunk();

}

bool check_validity_of_anim_params(
								   float i_FrameRate,
								   float	i_StartFrame,
								   float	i_EndFrame,
								   float	i_StepFrame)
{
	//validity check of inputs
	bool bValid = !fEpsilonEqual( i_FrameRate, 0.0f );
	bValid = bValid && i_FrameRate > 0.0f;
	bValid = bValid && i_EndFrame > i_StartFrame;
	bValid = bValid && ( i_StepFrame > 0.0f ); 
	bValid = bValid && !fEpsilonEqual( i_StepFrame, 0.0f );

	if ( !bValid )
	{	
		std::stringstream ss;
		ss << "one of these conditions is not satisfied\
			  parameters, i_FrameRate > 0, i_EndFrame > i_StartFrame, i_StepFram > 0 ";
		throw sgpuException( sgpuString( ss.str().c_str() ) );
	}

	float fNumKeys = (( i_EndFrame - i_StartFrame ) /i_StepFrame  );
	if( !fEpsilonEqual(fNumKeys, floor( fNumKeys ) ) )
	{		
		std::stringstream ss;
		ss << "'(endFrame - startFrame)':  " << i_EndFrame - i_StartFrame << "should be a multiple of stepFrame: " << i_StepFrame;
		throw sgpuException( sgpuString( ss.str().c_str() ) );
	}
	fNumKeys += 1.0f;

	if( fNumKeys >= static_cast< float > ( std::numeric_limits< int >::max() ) )
	{
		std::stringstream ss;
		ss << "too many key frames : " << fNumKeys ;
		throw sgpuException( sgpuString( ss.str().c_str() ) );
	}
	return true;
}
