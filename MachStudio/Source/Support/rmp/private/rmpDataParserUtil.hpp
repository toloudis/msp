/****************************************************************************\
**  rmpDataParserUtil.hpp
**   
**		Utility to read/write ramp data.  
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef RMP_DATAPARSERUTIL_HPP
#error rmpDataParserUtil.hpp multiply included
#endif
#define RMP_DATAPARSERUTIL_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

//============================================================================
//============================================================================
class chReader;
class chWriter;
class fsLocator;
class prtyObject;
class rmpObject;

//============================================================================
//============================================================================
namespace rmpDataParserUtil
{

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	void ReadData(	chReader& i_Reader,
					rmpObject& o_RampData );

	//------------------------------------------------------------------------
	//   ReadData for core level code i.e. prtyTextureFileName
	//------------------------------------------------------------------------
	prtyObject* ReadDataCore( chReader& i_Reader );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	void WriteData(	chWriter& o_Writer,
					const rmpObject& i_RampData );

	//------------------------------------------------------------------------
	//   WriteData for core level code i.e. prtyTextureFileName
	//------------------------------------------------------------------------
	void WriteDataCore(	chWriter& o_Writer,
						prtyObject* i_RampData );

}  //end rmpDataParserUtil namespace