/****************************************************************************\
**	mexpRecorderPosition.hpp
**
**		mexpRecorderPosition records changes in a position property
**	into animation keys.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef MEXP_RECORDERPOSITION_HPP
#error mexpRecorderPosition.hpp multiply included
#endif
#define MEXP_RECORDERPOSITION_HPP

#ifndef MEXP_CHANNELRECORDER_HPP
#include "Support/mexp/mexpChannelRecorder.hpp"
#endif

#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class mexpRecorderPosition : public mexpRecorderPropertyTemplate<maPoint3d, prtyPoint3d>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	mexpRecorderPosition(const std::string& i_AssetName,
						 const prtyPoint3d &i_Property);

	//--------------------------------------------------------------------
	//	Export data to the exporter
	//--------------------------------------------------------------------
	virtual void Export(mexpExporter &io_Exporter);

private:
	std::string m_AssetName;
};
