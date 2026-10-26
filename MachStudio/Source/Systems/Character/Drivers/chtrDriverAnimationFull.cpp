/*****************************************************************************
**	chtrDriverAnimationFull.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Drivers/chtrDriverAnimationFull.hpp"

#include "Systems/Character/GUI/chtrAnimList.hpp"
#include "Support/fsys/fsysFileList.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrDriverAnimationFull::chtrDriverAnimationFull(tmlnChannelAnimationFull &i_Channel, 
												 chDefs::Name i_ChunkName,
											     const fsLocator &i_CharacterDir,
												 const std::string &i_SourceIdentity)
:	tmlnDriverAnimationFull(i_Channel, i_ChunkName),
	m_CharacterDir(i_CharacterDir),
	m_SourceIdentity(i_SourceIdentity)
{
	//fsysFileList fileList;
	//chtrAnimList::BuildFileList(fileList, m_CharacterDir);
	//const bool bUpdateControl = false; // couldn't have created the control yet
	//this->SetAnimationList(fileList, bUpdateControl);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrDriverAnimationFull::~chtrDriverAnimationFull()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
std::string chtrDriverAnimationFull::GetAnimPart() const
{
	return m_SourceIdentity;
}

//--------------------------------------------------------------------
//	Find the full locator for the given animation filename.
//--------------------------------------------------------------------
//virtual 
//fsLocator chtrDriverAnimationFull::GetAnimationLocator( const itString &i_AnimFilename ) const
//{
//	return chtrAnimList::GetAnimationLocator(i_AnimFilename, m_CharacterDir);
//}
