/****************************************************************************\
**	mdlModelingPackageWriter.hpp
**
**		Writes out the modeling package data.
**
**	This data contains what package the exported data came from, 
**	what SDK was used, what version of the exporter was used, and the date.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_MODELINGPACKAGEWRITER_HPP
#error mdlModelingPackageWriter.hpp multiply included
#endif
#define MDL_MODELINGPACKAGEWRITER_HPP


//============================================================================
//	Forward References
//============================================================================
class chWriter;
struct mdlModelingPackageData;


//============================================================================
//============================================================================
namespace mdlModelingPackageWriter
{
	//------------------------------------------------------------------------
	//	Write the model package chunk information
	//------------------------------------------------------------------------
	void WriteData(	chWriter &io_Writer, 
					const mdlModelingPackageData& i_Data);
}


//============================================================================
//	Writing example:
//
//	mdlModelingPackageData mpdata;
//	mpdata.m_ModelingPackageName = itString("Maya 2010");	// can this be gotten from the modeling package automatically?
//	mpdata.m_ModelingPackageVersion = itString("1.2.4.10");	// can this be gotten from the modeling package automatically?
//	mpdata.m_CreationData = appTimeUtils::CurrentDateToJulian();
//	mpdata.m_SDKVersion.Set( 1,2,0,1 );
//	mpdata.m_ExporterVersion.Set( 1,4,2,17 );
//	mdlModelingPackageWriter( io_Writer, mpdata );
//============================================================================

