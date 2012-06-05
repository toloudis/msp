/****************************************************************************\
**  chnlTimeData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2005-7 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/Data/chnlTimeData.hpp"


chnlMarkerDataItem::chnlMarkerDataItem()
: m_TimeMarkerType("Marker Type", 0),
  m_Time("Time", 0),
  m_Note("Note", "")
{
}

chnlNoteDataItem::chnlNoteDataItem()
: m_Status("Status", 0),
  m_Time("Time", 0),
  m_Note("Note", "")
{
}