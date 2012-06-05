/****************************************************************************\
**  sbrdListData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/Data/sbrdListData.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/it/itStringUtil.hpp"


//
//		sbrdListData
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
sbrdListData::sbrdListData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
sbrdListData::sbrdListData(const itString& i_Filename)
{
	// add the filename
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void sbrdListData::Clear()
{
	m_Filenames.clear();
	m_Items.clear();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void sbrdListData::Insert(int i_Index, const itString& i_Filename)
{
	int count = m_Filenames.size();

	DBG_ASSERT0( i_Index < count, "Invalid index to insert before" );

	//DBG_LOG("Before INSERT");
	//for (int i = 0 ; i < m_Filenames.size() ; ++i)
	//{
	//	std::string fname = itStringUtil::GetStdString( m_Filenames[i].GetValue() );
	//	DBG_LOG2("%d (%s)", i, fname.c_str());
	//}

	m_Filenames.insert( m_Filenames.begin() + i_Index, m_Filenames[i_Index] );
	m_Filenames[i_Index].SetValue(i_Filename);

	//DBG_LOG("After INSERT");
	//for (int i = 0 ; i < m_Filenames.size() ; ++i)
	//{
	//	std::string fname = itStringUtil::GetStdString( m_Filenames[i].GetValue() );
	//	DBG_LOG2("%d (%s)", i, fname.c_str());
	//}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void sbrdListData::Add(const itString& i_Filename)
{
	int count = m_Filenames.size();
	m_Filenames.resize(count+1);
	m_Filenames[count].SetValue(i_Filename);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void sbrdListData::Remove(int i_Index)
{
	m_Filenames.erase( m_Filenames.begin() + i_Index );
}

