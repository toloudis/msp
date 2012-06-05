/********************************************************************************************\
**  mtrlMaterialParser.hpp
**
**	Writes overriden materials to scene chunk.
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef MTRL_MATERIALPARSER_HPP
#error mtrlMaterialParser.hpp multiply included
#endif
#define MTRL_MATERIALPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <vector>

//============================================================================
//============================================================================
class chReader;
class chWriter;
class mdlMaterialInfo;


//============================================================================
//============================================================================
namespace mtrlMaterialParser
{
	//------------------------------------------------------------------------
	//   ReadMaterialData
	//------------------------------------------------------------------------
	void ReadMaterialData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					std::vector< shared_ptr<mdlMaterialInfo> >& o_Controls );

	//------------------------------------------------------------------------
	//   WriteMaterialData
	//------------------------------------------------------------------------
	void WriteMaterialData(	chWriter& o_Writer,
					const std::vector< shared_ptr<mdlMaterialInfo> >& i_Controls );

}

