/****************************************************************************\
**  tmlnDriverUtil.cpp
**
**      driver utilities
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef TMLN_DRIVERUTIL_HPP
#error tmlnDriverUtil.hpp multiply included
#endif
#define TMLN_DRIVERUTIL_HPP


//============================================================================
//============================================================================
namespace tmlnDriverUtil
{
	//------------------------------------------------------------------------
	//	Add a button to the driver palette with the given tabpage
	//------------------------------------------------------------------------
	void AddButtonToDriverPalette( System::String* i_pTPName,
								   System::String* i_pBName,
								   System::String* i_pBToolTip,
								   System::String* i_pBImageName,
								   System::String* i_pHandler );

	//------------------------------------------------------------------------
	//	remove a button from the driver palette with the given tabpage
	//------------------------------------------------------------------------
	void RemoveButtonToDriverPalette( System::String* i_pTPName,
									   System::String* i_pBName );
}

