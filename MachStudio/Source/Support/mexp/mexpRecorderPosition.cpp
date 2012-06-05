/****************************************************************************\
**	mexpRecorderPosition.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mexp/mexpRecorderPosition.hpp"

#include "Support/mexp/mexpExporter.hpp"


namespace
{
} // end of anonymous namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mexpRecorderPosition::mexpRecorderPosition(const std::string& i_AssetName,
										   const prtyPoint3d &i_Property)
: mexpRecorderPropertyTemplate<maPoint3d, prtyPoint3d>(i_Property),
	m_AssetName(i_AssetName)
{

}

//--------------------------------------------------------------------
//	Export data to the exporter
//--------------------------------------------------------------------
void mexpRecorderPosition::Export(mexpExporter &io_Exporter)
{
	io_Exporter.ExportAnimationTranslation(m_AssetName, m_Recorder.m_Keys);
}

