/****************************************************************************\
**	vtxCompressionUtil.hpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/vtxCompressionUtil.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Graphics/vtx/vtxDeltaLosslessEncoder.hpp"
#include "Graphics/vtx/vtxDeltaToleranceEncoder.hpp"
#include "Graphics/vtx/vtxNormal16BitEncoder.hpp"
#include "Graphics/vtx/vtxNormalLosslessEncoder.hpp"
#include "Graphics/vtx/vtxStreamCompression.hpp"


//============================================================================
//============================================================================
namespace vtxCompressionUtil
{
	namespace
	{
		const char* c_LosslessEncoderString = "EXT_DLT_ZL";
		const char* c_ToleranceEncoderString = "EXT_TOL16_ZL";
	}

	//--------------------------------------------------------------------
	// Create a stream for compressing animation losslessly
	//--------------------------------------------------------------------
	shared_ptr<vtxStreamCompression> CreateLosslessCompressStream(const std::string &i_SurfaceName,
																  vtxGeometryCacheWriter &io_GeometryCache,
																  int i_SyncInterval)
	{
		DBG_ASSERT(i_SyncInterval > 1, "SyncInterval must be greater than one.");

		// Create the stream for compressing the frame data
		shared_ptr<vtxDeltaEncoder> deltaEncoder(new vtxDeltaLosslessEncoder());
		shared_ptr<vtxDeltaEncoder> normalEncoder(new vtxNormalLosslessEncoder());
		shared_ptr<vtxStreamCompression> compress_stream(new vtxStreamCompression(
			i_SurfaceName, i_SyncInterval, io_GeometryCache,
			c_LosslessEncoderString, deltaEncoder, normalEncoder));
		return compress_stream;
	}

	//--------------------------------------------------------------------
	// Create a stream for compressing animation which allows a given
	// error tolerance in centimeters.
	//--------------------------------------------------------------------
	shared_ptr<vtxStreamCompression> CreateToleranceCompressStream(float i_Tolerance,
																   const std::string &i_SurfaceName,
																   vtxGeometryCacheWriter &io_GeometryCache,
																   int i_SyncInterval)
	{
		DBG_ASSERT(i_SyncInterval > 1, "SyncInterval must be greater than one.");
		DBG_ASSERT(i_Tolerance >= 0, "Tolerance must be positive number.");

		shared_ptr<vtxDeltaEncoder> deltaEncoder(new vtxDeltaToleranceEncoder(i_Tolerance));
		shared_ptr<vtxDeltaEncoder> normalEncoder(new vtxNormal16BitEncoder());
		shared_ptr<vtxStreamCompression> compress_stream(new vtxStreamCompression(
			i_SurfaceName, i_SyncInterval, io_GeometryCache,
			c_ToleranceEncoderString, deltaEncoder, normalEncoder));
		return compress_stream;
	}

}

