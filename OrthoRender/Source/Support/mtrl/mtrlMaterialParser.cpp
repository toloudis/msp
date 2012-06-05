/********************************************************************************************\
**  mtrlMaterialParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/mtrlMaterialParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Graphics/mtr/mtrMaterialSaver.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"

namespace mtrlMaterialParser
{

//========================================================================
//   ReadMaterialData
//========================================================================
void ReadMaterialData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				std::vector< shared_ptr<mdlMaterialInfo> >& o_Materials )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if (name == mtrMaterialSaver::GetChunkName())
		{
			shared_ptr<mdlMaterialInfo> data(new mdlMaterialInfo());
			mtrMaterialSaver::ReadMaterialData(i_Reader, version, size, *data);
			o_Materials.push_back(data);
		}
		i_Reader.FinishChunk();
	}
}

//========================================================================
//   WriteMaterialData
//========================================================================
void WriteMaterialData(	chWriter& o_Writer,
					  const std::vector< shared_ptr<mdlMaterialInfo> >& i_Materials )
{
	int num_materials = i_Materials.size();
	//DBG_LOG1("Num materials writing: %d", num_materials);
	for (int i=0; i<num_materials; i++)
	{
		// Write material info
		mtrMaterialSaver::WriteMaterialData( o_Writer, *i_Materials[i] );
	}
}


}	// end of namespace
