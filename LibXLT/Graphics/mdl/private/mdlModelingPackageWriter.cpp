/****************************************************************************\
**	mdlModelingPackageWriter.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/private/mdlModelingPackageWriter.hpp"

#include "Core/Ch/chWriter.hpp"
#include "Graphics/mdl/mdlModelingPackageData.hpp"


//============================================================================
//	Any of these mdlModelingPackageWriter functions might throw one of the fs exceptions.
//============================================================================
namespace mdlModelingPackageWriter
{
	//=============================================================================
	//	Chunk types
	//=============================================================================
	namespace
	{
		const chDefs::Name c_MODV = chDefs::MakeName('M', 'O', 'D', 'V');
	}

	//------------------------------------------------------------------------
	//	Write the model package chunk information
	//------------------------------------------------------------------------	
	void WriteData(	chWriter &io_Writer, 
					const mdlModelingPackageData& i_Data)
	{
		const int c_MODV_VERSION = 0;
		io_Writer.WriteChunkHeader(c_MODV, c_MODV_VERSION, true);

		io_Writer.Write(i_Data.m_CreationDate);
		io_Writer.Write(i_Data.m_ExporterVersion);
		io_Writer.Write(i_Data.m_SDKVersion);
		io_Writer.Write(i_Data.m_ModelingPackageName);
		io_Writer.Write(i_Data.m_ModelingPackageVersion);

		io_Writer.FinishChunk(); // MODV
	}
}

