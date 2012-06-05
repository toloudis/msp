/*****************************************************************************
**	propDriverAnimationFull.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/Drivers/propDriverAnimationFull.hpp"

#include "Systems/Props/GUI/propAnimList.hpp"

#include "Support/fsys/fsysFileUtil.hpp"
#include "Core/gf/gfPaths.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
propDriverAnimationFull::propDriverAnimationFull(tmlnChannelAnimationFull &i_Channel, 
												 chDefs::Name i_ChunkName)
:	tmlnDriverAnimationFull(i_Channel, i_ChunkName)
{
	//fsysFileList fileList;
	//propAnimList::BuildFileList(fileList);
	//const bool bUpdateControl = false; // couldn't have created the control yet
	//this->SetAnimationList(fileList, bUpdateControl);
}

//--------------------------------------------------------------------
//	Find the full locator for the given animation filename.
//--------------------------------------------------------------------
//virtual 
//fsLocator propDriverAnimationFull::GetAnimationLocator( const itString &i_AnimFilename ) const
//{
//	fsLocator anim_loc;
//	fsysFileUtil::GetFilePath(propAnimList::GetSystemDirName(),
//			itString(gfPaths::GetSubPath(gfPaths::e_Data)), i_AnimFilename, anim_loc);
//	return anim_loc;
//}
