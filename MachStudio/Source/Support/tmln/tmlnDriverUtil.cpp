/****************************************************************************\
**  tmlnDriverUtil.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "tmlnDriverUtil.hpp"

#include "tmaTabControlMgr.hpp"


namespace tmlnDriverUtil
{

//------------------------------------------------------------------------
//	Add a button to the driver palette with the given tabpage
//------------------------------------------------------------------------
void AddButtonToDriverPalette( System::String* i_pTPName,
								System::String* i_pBName,
								System::String* i_pBToolTip,
								System::String* i_pBImageFile,
								System::String* i_pBHandler )
{
	tmaTabControlMgr::CreateTabPage( S"TabControl_adddriver", i_pTPName );
	tmaTabControlMgr::CreateTabPageButton( S"TabControl_adddriver", 
										  i_pTPName,
										  i_pBName,
										  i_pBToolTip,
										  i_pBImageFile,
										  i_pBHandler );
}

//------------------------------------------------------------------------
//	remove a button from the driver palette with the given tabpage
//------------------------------------------------------------------------
void RemoveButtonToDriverPalette( System::String* i_pTPName,
									System::String* i_pBName )
{
	tmaTabControlMgr::RemoveTabPageButton( S"TabControl_adddriver", i_pTPName, i_pBName );
}

}	// end of namespace

