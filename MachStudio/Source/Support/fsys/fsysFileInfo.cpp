/****************************************************************************\
**  fsysFileInfo.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/fsys/fsysFileInfo.hpp"


//------------------------------------------------------------------------
//------------------------------------------------------------------------
fsysFileInfo::fsysFileInfo()
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fsysFileInfo::operator =(const fsysFileInfo& i_Item)
{
	this->m_Filename	= i_Item.GetFilename();
	this->m_FilePath	= i_Item.GetFilePath();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool fsysFileInfo::operator ==(const fsysFileInfo& i_Item)
{
	if ( m_FilePath == i_Item.GetFilePath() )
	{
		if ( m_Filename == i_Item.GetFilename() )
		{
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool fsysFileInfo::operator >=(const fsysFileInfo& i_Item) const
{
	if ( m_FilePath >= i_Item.GetFilePath() )
	{
		if ( m_Filename >= i_Item.GetFilename() )
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool fsysFileInfo::operator <=(const fsysFileInfo& i_Item) const
{
	if ( m_FilePath <= i_Item.GetFilePath() )
	{
		if ( m_Filename <= i_Item.GetFilename() )
		{
			return true;
		}
	}
	return false;
}

