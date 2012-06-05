/****************************************************************************\
**	mdlModelingPackageParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlModelingPackageParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Graphics/mdl/mdlModelingPackageData.hpp"


//============================================================================
//	Any of these mdlModelingPackageParser functions might throw a 
//	mdlInvalidModelFileX or one of the fs exceptions.
//============================================================================
namespace mdlModelingPackageParser
{
	//=============================================================================
	//	Chunk types
	//=============================================================================
	namespace
	{
		const chDefs::Name c_MODV = chDefs::MakeName('M', 'O', 'D', 'V');
	}

	//----------------------------------------------------------------------------
	//	Read model package chunk - returns true if the data was read
	//	successful.
	//----------------------------------------------------------------------------
	void ReadMODV(	chReader& io_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					mdlModelingPackageData& o_Data )
	{
		io_Reader.Read(o_Data.m_CreationDate);
		io_Reader.Read(o_Data.m_ExporterVersion);
		io_Reader.Read(o_Data.m_SDKVersion);
		io_Reader.Read(o_Data.m_ModelingPackageName);
		io_Reader.Read(o_Data.m_ModelingPackageVersion);
	}
}

