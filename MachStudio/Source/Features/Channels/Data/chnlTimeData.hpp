/********************************************************************************************\
**  chnlTimeData.hpp
**
**		Data relating to time for the entire scene (not individual objects)
**
**  StudioGPU
**  Copyright(C) 2005-7 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef CHNL_TIMEDATA_HPP
#error chnlTimeData.hpp multiply included
#endif
#define CHNL_TIMEDATA_HPP

#ifndef PRTY_TIME_HPP
#include "Core/prty/prtyTime.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif 
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//	Types
//============================================================================
enum chnlTimeMarkerType
{
	e_MarkerType_Normal = 0,
	e_MarkerType_In,
	e_MarkerType_Out
};
enum chnlNoteStatus
{
	e_NoteStatus_Open = 0,
	e_NoteStatus_Pending,
	e_NoteStatus_Closed
};


//============================================================================
//	Data
//============================================================================
//
//	Markers
//
class chnlMarkerDataItem 
{
public:
	chnlMarkerDataItem();
	prtyInt32	m_TimeMarkerType;
	prtyTime	m_Time;			// timeline time
	prtyText	m_Note;
};

//
//	Notes
//
class chnlNoteDataItem 
{
public:
	chnlNoteDataItem();
	prtyInt32	m_Status;
	prtyTime	m_Time;			// timeline time
	prtyText	m_Note;
};

//
//	Time data
//
struct chnlTimeData
{
	//---------------------------------------------------------------------------
	//	data
	//---------------------------------------------------------------------------
	std::vector<chnlMarkerDataItem>	m_Markers;
	std::vector<chnlNoteDataItem>	m_Notes;
};

