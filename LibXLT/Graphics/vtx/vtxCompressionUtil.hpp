/*****************************************************************************
**	vtxCompressionUtil.hpp
**
**		Utilitiy for creating compressions streams with different encodings.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_COMPRESSIONUTIL_HPP
#error vtxCompressionUtil.hpp multiply included
#endif
#define VTX_COMPRESSIONUTIL_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <string>



//============================================================================
//============================================================================
class vtxGeometryCacheWriter;
class vtxStreamCompression;


//============================================================================
//============================================================================
namespace vtxCompressionUtil
{
	//--------------------------------------------------------------------
	// Create a stream for compressing animation losslessly
	//--------------------------------------------------------------------
	shared_ptr<vtxStreamCompression> CreateLosslessCompressStream(const std::string &i_SurfaceName,
																  vtxGeometryCacheWriter &io_GeometryCache,
																  int i_SyncInterval = 16);

	//--------------------------------------------------------------------
	// Create a stream for compressing animation which allows a given
	// error tolerance in centimeters.
	//--------------------------------------------------------------------
	shared_ptr<vtxStreamCompression> CreateToleranceCompressStream(float i_Tolerance,
																   const std::string &i_SurfaceName,
																   vtxGeometryCacheWriter &io_GeometryCache,
																   int i_SyncInterval = 16);

}
